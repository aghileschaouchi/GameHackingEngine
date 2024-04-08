#pragma once

#include <memory>

#include <Windows.h>
#include <TlHelp32.h>
#include <Memoryapi.h>
#include "../helper/Helper.h"

#pragma comment(lib, "kernel32.lib") 

namespace ghe
{
	template <typename T, typename P, typename H, typename A, typename PB>
	class WinMemoryManager
	{
	public:
		WinMemoryManager() {}
		WinMemoryManager(std::weak_ptr<P>& hProcessPtr) : m_hProcessPtr(hProcessPtr) {}
		~WinMemoryManager()
		{
		}

		void setHProcess(std::weak_ptr<P>& hProcessPtr)
		{
			m_hProcessPtr = hProcessPtr;
		}

		template <typename V>
		inline PB unlock(const ghe::Address<A>& address, const V& pointedValue)
		{
			if (auto&& _lockedHProcessPtr = m_hProcessPtr.lock())
			{
				PB _protectionBackup;

				if (VirtualProtectEx(*_lockedHProcessPtr, reinterpret_cast<LPVOID>(address.value()), sizeof(V), PAGE_EXECUTE_READWRITE,
					&_protectionBackup) == FALSE)
				{
					printf("unlock() exited with %ud as code error, check VirtualProtectEx call 1\n", GetLastError());
					return 0;
				}
				return _protectionBackup;
			}
			printf("unlock() exited because there is an issue with its hProcess, check its lifetime!\n");
		}

		template <typename V>
		inline bool restore(const ghe::Address<A>& address, const V& pointedValue, PB protectionBackup)
		{
			if (protectionBackup == 0)
			{
				printf("restore() exited because protectionBackup=0\n");
				return false;
			}
			if (auto&& _lockedHProcessPtr = m_hProcessPtr.lock())
			{
				if (VirtualProtectEx(*_lockedHProcessPtr, reinterpret_cast<LPVOID>(address.value()), sizeof(V), protectionBackup, NULL) == FALSE)
				{
					printf("restore() exited with %ud as code error, check VirtualProtectEx call\n", GetLastError());
					return false;
				}
				return true;
			}
			printf("restore() exited because there is an issue with its hProcess, check its lifetime!\n");
			return false;
		}

		template <typename V, bool UNLOCK = false>
		inline V readValue(const ghe::Address<A>& address)
		{
			if (auto&& _lockedHProcessPtr = m_hProcessPtr.lock())
			{
				V pointedValue;

				PB protectionBackup;
				if constexpr (UNLOCK)
				{
					protectionBackup = unlock<V>(address, pointedValue);
				}

				if (ReadProcessMemory(*_lockedHProcessPtr, reinterpret_cast<LPCVOID>(address.value()), reinterpret_cast<LPVOID>(&pointedValue),
					sizeof(pointedValue), NULL) == FALSE)
				{
					printf("readValue() exited with %ud as code error, check ReadProcessMemory call\n", GetLastError());
					return 0;
				}

				if constexpr (UNLOCK)
				{
					if (restore<V>(address, pointedValue, protectionBackup))
					{
						return pointedValue;
					}
					return 0;
				}
				return pointedValue;
			}
			printf("readValue() exited because there is an issue with its hProcess, check its lifetime!\n");
			return 0;
		}

		template <typename V, bool UNLOCK = false>
		inline bool writeValue(const ghe::Address<A>& address, const V& pointedValue)
		{
			if (auto&& _lockedHProcessPtr = m_hProcessPtr.lock())
			{
				PB protectionBackup;
				if constexpr (UNLOCK)
				{
					protectionBackup = unlock<V>(address, pointedValue);
				}

				if (WriteProcessMemory(*_lockedHProcessPtr, reinterpret_cast<LPVOID>(address.value()), reinterpret_cast<LPCVOID>(&pointedValue),
					sizeof(pointedValue), NULL) == FALSE)
				{
					printf("writeValue() exited with %ud as code error, check WriteProcessMemory call\n", GetLastError());
					return false;
				}

				if constexpr (UNLOCK)
				{
					if (restore<V>(address, pointedValue, protectionBackup))
					{
						return true;
					}
					return false;
				}
				return true;
			}
			printf("writeValue() exited because there is an issue with its hProcess, check its lifetime!\n");
			return false;
		}

		A moduleBaseAddress(T processIdentifier, const std::string& moduleName)
		{
			A moduleBaseAddress = 0;
			HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processIdentifier);

			if (hSnapshot == INVALID_HANDLE_VALUE)
			{
				printf("moduleBaseAddress() exited with %ud as code error, check CreateToolhelp32Snapshot call\n", GetLastError());
				return moduleBaseAddress;
			}

			MODULEENTRY32 ModuleEntry32;
			ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
			if (Module32First(hSnapshot, &ModuleEntry32))
			{
				do
				{
					if (_tcscmp(ModuleEntry32.szModule, moduleName.c_str()) == 0)
					{
						moduleBaseAddress = reinterpret_cast<DWORD_PTR>(ModuleEntry32.modBaseAddr);
						break;
					}
				} while (Module32Next(hSnapshot, &ModuleEntry32));
			}
			CloseHandle(hSnapshot);
			return moduleBaseAddress;
		}

	private:
		std::weak_ptr<P> m_hProcessPtr;
	};
}
