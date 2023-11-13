#pragma once

#include "WinProcess.h"
#include "../helper/Helper.h"

#include <vector>

namespace
{
	DWORD_PTR dwGetModuleBaseAddress(DWORD dwProcessIdentifier, TCHAR* szModuleName)
	{
		DWORD_PTR dwModuleBaseAddress = 0;
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcessIdentifier);
		if (hSnapshot != INVALID_HANDLE_VALUE)
		{
			MODULEENTRY32 ModuleEntry32;
			ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
			if (Module32First(hSnapshot, &ModuleEntry32))
			{
				do
				{
					if (_tcscmp(ModuleEntry32.szModule, szModuleName) == 0)
					{
						dwModuleBaseAddress = (DWORD_PTR)ModuleEntry32.modBaseAddr;
						break;
					}
				} while (Module32Next(hSnapshot, &ModuleEntry32));
			}
			CloseHandle(hSnapshot);
		}
		return dwModuleBaseAddress;
	}

	TCHAR baseModuleName[13] = TEXT("somegame.exe");
	DWORD FindTheAddr(HANDLE hProcess, int pointerLevel, DWORD offsets[], DWORD_PTR baseAddress, DWORD staticAddress, DWORD staticOffset)
	{
		DWORD tmp_addr = NULL;
		DWORD dynamicAddress = NULL;
		ReadProcessMemory(hProcess, (LPCVOID)(baseAddress + staticOffset), (LPVOID)&staticAddress, sizeof(staticAddress), NULL);
		dynamicAddress = staticAddress;
		for (int i = 0; i < pointerLevel; i++)
		{
			dynamicAddress += offsets[i];
			ReadProcessMemory(hProcess, (LPCVOID)(dynamicAddress), (LPVOID)&tmp_addr, sizeof(tmp_addr), NULL);
			dynamicAddress = tmp_addr;
		}
		dynamicAddress += offsets[pointerLevel];

		return dynamicAddress;
	}
}

namespace ghe
{
	template <typename T, typename S, typename P, typename H, typename A, unsigned int AC = 10>
	class WinGame : public WinProcess<T, S, P, H, A>
	{
	public:
		//copy
		WinGame(std::unique_ptr<ghe::Address<A>>& baseAddress, T& pid, S& programName, P& hProcess, H& hwnd, std::string& baseModuleName) = delete;
		WinGame(ghe::Address<A>& baseAddressContent, T& pid, S& programName, P& hProcess, H& hwnd, std::string& baseModuleName) = delete;
		//move
		WinGame(std::unique_ptr<ghe::Address<A>> &&baseAddress, T&& pid, S&& programName, P&& hProcess, H&& hwnd, std::string&& baseModuleName) :
			WinProcess<T, S, P, H, A>(std::move(baseAddress), std::forward<T>(pid), std::forward<S>(programName),
				std::forward<P>(hProcess), std::forward<H>(hwnd)), m_baseModuleName(std::move(baseModuleName)) {
			if (m_addresses.empty())
			{
				m_addresses.reserve(AC);
			}
			setupHwnd();
			setupPid();
			setupHProcess();
			setupBaseAddress();
			log();
		}
		
		WinGame(ghe::Address<A>&&baseAddressContent, T&& pid, S&& programName, P&& hProcess, H&& hwnd, std::string&& baseModuleName) :
			WinProcess<T, S, P, H, A>(std::move(baseAddressContent), std::forward<T>(pid), std::forward<S>(programName),
				std::forward<P>(hProcess), std::forward<H>(hwnd)), m_baseModuleName(std::move(baseModuleName)) {
			if (m_addresses.empty())
			{
				m_addresses.reserve(AC);
			}
			setupHwnd();
			setupPid();
			setupHProcess();
			setupBaseAddress();
			log();
		}

		virtual ~WinGame() { if (this->m_hProcess != NULL) CloseHandle(this->m_hProcess); }

		inline void setupBaseAddress()
		{
			DWORD_PTR _baseModuleAddress = dwGetModuleBaseAddress(this->m_pid, helper::convertStringToTCHARPtr(m_baseModuleName)); //helper functions should be fixed
			this->m_baseAddress.get()->setAddress(static_cast<DWORD_PTR>(_baseModuleAddress));
			this->m_baseAddress.get()->setStatic(); //access to unique_ptr's Address value to be checked
		}

		inline void setupHwnd()
		{
			this->m_hwnd = FindWindow(NULL, this->m_programName);
		}

		inline void setupPid()
		{
			GetWindowThreadProcessId(this->m_hwnd, &this->m_pid);
		}

		inline void setupHProcess()
		{
			this->m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->m_pid);
		}

		WinGame& operator=(const WinGame& other) = delete;

		WinGame& operator=(WinGame&& other) noexcept
		{
			if (this != &other)
			{
				this->m_baseAddress.reset();
				this->m_baseAddress = std::move(other.m_baseAddress);
				this->m_hProcess = std::move(other.m_hProcess);
				this->m_pid = std::move(other.m_pid);
				this->m_hwnd = std::move(other.m_hwnd);
				this->m_processName = std::move(other.m_processName);
				this->m_baseModuleName = std::move(other.m_baseModuleName);
			}
			return *this;
		}

		// SHOULD BE ISOLATED WITH A DESIGN PATTERN
		/*inline void setupDynamicAddress(int pointerLevel, DWORD* offsets, DWORD staticOffset) {
		m_dynamicAddress = FindTheAddr(m_hProcess, pointerLevel, offsets, m_baseAddress, m_staticAddress, staticOffset);
		}*/

		//inline DWORD readDynamicAddrValue()
		//{
		//	DWORD value;
		//	ReadProcessMemory(m_hProcess, (LPCVOID)m_dynamicAddress, (LPVOID)&value, sizeof(value), NULL);
		//	//std::cout << "Inside function: " << value << std::endl;
		//	return value;
		//}

		//inline DWORD readStaticAddrValue()
		//{
		//	DWORD value;
		//	ReadProcessMemory(m_hProcess, (LPCVOID)m_staticAddress, (LPVOID)&value, sizeof(value), NULL);
		//	//std::cout << "Inside function: " << value << std::endl;
		//	return value;
		//}

		/*inline int writeValue()
		{

		}*/

		void log() override
		{
			static const char* processNameLogMessage = "process name: ";
			static const char* pidLogMessage = "PID:";
			static const char* baseAddressLogMessage = "base address:";
			printf("%s %s\n %s %d\n %s %s\n", processNameLogMessage, this->m_programName, pidLogMessage, this->m_pid, baseAddressLogMessage, this->m_baseAddress->toString().c_str());
		}

	private:
		std::vector<ghe::Address<A>> m_addresses; //should be added to the constructors?
		std::string m_baseModuleName;
	};
}

