#pragma once

#include "WinProcess.h"
#include "../helper/Helper.h"

#include <vector>

namespace
{
	static const uint16_t addressesCount = 10;

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
		std::cout << "addr: " << std::hex << staticAddress << std::endl;

		dynamicAddress = staticAddress;
		for (int i = 0; i < pointerLevel; i++)
		{
			dynamicAddress += offsets[i];
			ReadProcessMemory(hProcess, (LPCVOID)(dynamicAddress), (LPVOID)&tmp_addr, sizeof(tmp_addr), NULL);
			std::cout << "addr: " << std::hex << dynamicAddress << std::endl;
			dynamicAddress = tmp_addr;
		}

		dynamicAddress += offsets[pointerLevel];
		std::cout << "addr: " << std::hex << dynamicAddress << std::endl;

		return dynamicAddress;
	}
}

namespace ghe
{
	template<DWORD, LPCWSTR, HANDLE, HWND>
	class WinGame : public WinProcess<DWORD, LPCWSTR, HANDLE, HWND>
	{
	public:
		using WinProcess<DWORD, LPCWSTR, HANDLE, HWND>::WinProcess;

		WinGame(DWORD_PTR baseAddress, HANDLE hProcess, DWORD pid, HWND gameHwnd, LPCWSTR processName, LPCWSTR baseModuleName) :
			WinProcess<DWORD, LPCWSTR, HANDLE, HWND>(baseAddress, hProcess, pid, gameHwnd, processName) {
			if (m_addresses.empty())
			{
				m_addresses.reserve(addressesCount);
			}
			setupHwnd();
			setupPid();
			setupHProcess();
			setupBaseAddress();
			log();
		}

		WinGame(DWORD_PTR&& baseAddress, HANDLE&& hProcess, DWORD&& pid, HWND&& gameHwnd, LPCWSTR&& processName, LPCWSTR&& baseModuleName) :
			WinProcess<DWORD, LPCWSTR, HANDLE, HWND>(baseAddress, hProcess, pid, gameHwnd, processName) {
			if (m_addresses.empty())
			{
				m_addresses.reserve(addressesCount);
			}
			setupHwnd();
			setupPid();
			setupHProcess();
			setupBaseAddress();
			log();
		}

		virtual ~WinGame() { if (m_hProcess != NULL) CloseHandle(m_hProcess); }

		void setupBaseAddress()
		{
			m_baseAddress = dwGetModuleBaseAddress(m_pid, helper::convertStringToTCHARPtr(m_baseModuleName));
		}

		/*inline void setupDynamicAddress(int pointerLevel, DWORD* offsets, DWORD staticOffset) {
		m_dynamicAddress = FindTheAddr(m_hProcess, pointerLevel, offsets, m_baseAddress, m_staticAddress, staticOffset);
		}*/

		//DWORD readDynamicAddrValue()
		//{
		//	DWORD value;
		//	ReadProcessMemory(m_hProcess, (LPCVOID)m_dynamicAddress, (LPVOID)&value, sizeof(value), NULL);
		//	//std::cout << "Inside function: " << value << std::endl;
		//	return value;
		//}

		//DWORD readStaticAddrValue()
		//{
		//	DWORD value;
		//	ReadProcessMemory(m_hProcess, (LPCVOID)m_staticAddress, (LPVOID)&value, sizeof(value), NULL);
		//	//std::cout << "Inside function: " << value << std::endl;
		//	return value;
		//}

		/*inline int writeValue()
		{

		}*/

		WinGame& operator=(const WinGame& other)
		{
			if (this != &other)
			{
				this->m_baseAddress = other.m_baseAddress;
				this->m_hProcess = other.m_hProcess;
				this->m_pid = other.m_pid;
				this->m_hwnd = other.m_hwnd;
				this->m_processName = other.m_processName;
				this->m_baseModuleName = other.m_baseModuleName;
			}
			return *this;
		}

		WinGame& operator=(WinGame&& other) noexcept
		{
			if (this != &other)
			{
				this->m_baseAddress = std::move(other.m_baseAddress);
				this->m_hProcess = std::move(other.m_hProcess);
				this->m_pid = std::move(other.m_pid);
				this->m_hwnd = std::move(other.m_hwnd);
				this->m_processName = std::move(other.m_processName);
				this->m_baseModuleName = std::move(other.m_baseModuleName);
			}
			return *this;
		}

	private:
		std::vector<ghe::Address> m_addresses;
		LPCWSTR m_baseModuleName;
	};
}

