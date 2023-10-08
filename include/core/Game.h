#pragma once

#include "Address.h"
#include "Program.h"
#include "../../include/helper/Helper.h"

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
	class Game : public Program
	{
	public:
		explicit Game(DWORD_PTR baseAddress, HANDLE hProcess, DWORD pid, HWND gameHwnd, LPCWSTR processName, LPCWSTR baseModuleName) :
			Program(baseAddress, hProcess, pid, gameHwnd, processName, baseModuleName) {
			if (m_addresses.empty())
			{
				m_addresses.reserve(addressesCount);
			}
			setupHwnd();
			setupPid();
			setupHProcess();
			//setupBaseAddress();
			log();
		}

		explicit Game(DWORD_PTR&& baseAddress, HANDLE&& hProcess, DWORD&& pid, HWND&& gameHwnd, LPCWSTR&& processName, LPCWSTR&& baseModuleName) :
			Program(baseAddress, hProcess, pid, gameHwnd, processName, baseModuleName) {
			if (m_addresses.empty())
			{
				m_addresses.reserve(addressesCount);
			}
			setupHwnd();
			setupPid();
			setupHProcess();
			//setupBaseAddress();
			log();
		}

		~Game() { if (m_hProcess != NULL) CloseHandle(m_hProcess); }

		Game& operator=(const Game& other);
		Game& operator=(Game&& other) noexcept;

		void setupBaseAddress()
		{
			m_baseAddress = dwGetModuleBaseAddress(m_pid, helper::convertStringToTCHARPtr(m_baseModuleName));
		}

	private:
		std::vector<ghe::Address> m_addresses;
	};
}

