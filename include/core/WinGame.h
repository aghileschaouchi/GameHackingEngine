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
}

namespace ghe
{
	template <typename T, typename P, typename H, typename A, unsigned int AC = 10>
	class WinGame : public WinProcess<T, P, H, A>
	{
	public:
		//copy
		WinGame(std::unique_ptr<ghe::Address<A>>& baseAddress, T& pid, std::string& programName, P& hProcess, H& hwnd, std::string& baseModuleName) = delete;
		WinGame(ghe::Address<A>& baseAddressContent, T& pid, std::string& programName, P& hProcess, H& hwnd, std::string& baseModuleName) = delete;
		//move
		WinGame(std::unique_ptr<ghe::Address<A>>&& baseAddress, T&& pid, std::string&& programName, P&& hProcess, H&& hwnd, std::string&& baseModuleName) :
			WinProcess<T, P, H, A>(std::move(baseAddress), std::forward<T>(pid), std::move(programName),
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

		WinGame(ghe::Address<A>&& baseAddressContent, T&& pid, std::string&& programName, P&& hProcess, H&& hwnd, std::string&& baseModuleName) :
			WinProcess<T, P, H, A>(std::move(baseAddressContent), std::forward<T>(pid), std::move(programName),
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

		virtual ~WinGame() override { if (this->m_hProcess != NULL) CloseHandle(this->m_hProcess); }

		inline void setupBaseAddress()
		{
			TCHAR* _wbaseModuleName = helper::convert(m_baseModuleName);
			DWORD_PTR _baseModuleAddress = dwGetModuleBaseAddress(this->m_pid, _wbaseModuleName);
			delete[] _wbaseModuleName; //should implement helper::convert as a functor with a destructor that deletes this

			this->m_baseAddress.get()->setAddress(static_cast<DWORD_PTR>(_baseModuleAddress));
			this->m_baseAddress.get()->setStatic(); //access to unique_ptr's Address value to be checked
		}

		inline void setupHwnd()
		{
			this->m_hwnd = FindWindow(NULL, helper::constConvert(this->m_programName));
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

		void log() override
		{
			static const char* processNameMessage = "process name: "; //%s
			static const char* baseModuleNameMessage = "base module name: "; //%s
			static const char* pidMessage = "pid:"; //%ld
			static const char* hwndMessage = "hwnd:"; //%p
			static const char* hProcessMessage = "hProcess:"; //%p
			static const char* baseAddressMessage = "base address:"; //%s
			printf("%s %s\n %s %s\n %s %ld\n %s %p\n %s %p\n %s %s\n", processNameMessage, this->m_programName.c_str(), baseModuleNameMessage, this->m_baseModuleName.c_str(), pidMessage, this->m_pid,
																		hwndMessage, this->m_hwnd, hProcessMessage, this->m_hProcess, baseAddressMessage, this->m_baseAddress->toString().c_str());
		}

	private:
		std::vector<ghe::Address<A>> m_addresses; //should be added to the constructors?
		std::string m_baseModuleName;
	};
}

