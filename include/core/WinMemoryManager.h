#pragma once

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
		WinMemoryManager() : m_hProcessPtr(nullptr) {}
		WinMemoryManager(P* hProcessPtr) : m_hProcessPtr(hProcessPtr) {}
		~WinMemoryManager()
		{
			m_hProcessPtr = nullptr;
		}

		void setHProcess(P* hProcessPtr)
		{
			m_hProcessPtr = hProcessPtr;
		}

		template <typename V>
		inline PB unlock(const ghe::Address<A>& address, const V& pointedValue)
		{
			if (m_hProcessPtr == nullptr)
			{
				return 0;
			}
			PB _protectionBackup;

			if (VirtualProtectEx(*m_hProcessPtr, (LPVOID)address.value(), sizeof(V), PAGE_EXECUTE_READWRITE, &_protectionBackup) == 0)
			{
				unsigned int codeError = GetLastError();
				printf("unlock() exited with %ud as code error, check VirtualProtectEx call 1\n", codeError);
				return 0;
			}
			return _protectionBackup;
		}

		template <typename V>
		inline bool restore(const ghe::Address<A>& address, const V& pointedValue, PB protectionBackup)
		{
			if (m_hProcessPtr == nullptr || protectionBackup == 0)
			{
				return false;
			}

			if (VirtualProtectEx(*m_hProcessPtr, (LPVOID)address.value(), sizeof(V), protectionBackup, NULL) == 0)
			{
				unsigned int codeError = GetLastError();
				printf("restore() exited with %ud as code error, check VirtualProtectEx call\n", codeError);
				return false;
			}
			return true;
		}

		template <typename V, bool UNLOCK = false>
		inline V readValue(const ghe::Address<A>& address)
		{
			if (m_hProcessPtr == nullptr)
			{
				return 0;
			}

			V pointedValue;

			PB protectionBackup;
			if constexpr (UNLOCK)
			{
				protectionBackup = unlock<V>(address, pointedValue);
			}

			if (ReadProcessMemory(*m_hProcessPtr, (LPCVOID)address.value(), (LPVOID)&pointedValue, sizeof(pointedValue), NULL) == 0)
			{
				unsigned int codeError = GetLastError();
				printf("readValue() exited with %ud as code error, check ReadProcessMemory call\n", codeError);
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

		template <typename V, bool UNLOCK = false>
		inline bool writeValue(const ghe::Address<A>& address, const V& pointedValue)
		{
			if (m_hProcessPtr == nullptr)
			{
				return false;
			}

			PB protectionBackup;
			if constexpr (UNLOCK)
			{
				protectionBackup = unlock<V>(address, pointedValue);
			}

			if (WriteProcessMemory(*m_hProcessPtr, (LPVOID)address.value(), (LPCVOID)&pointedValue, sizeof(pointedValue), NULL) == 0)
			{
				unsigned int codeError = GetLastError();
				printf("writeValue() exited with %ud as code error, check WriteProcessMemory call\n", codeError);
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

		A moduleBaseAddress(T processIdentifier, std::string moduleName)
		{
			TCHAR* _wbaseModuleName = helper::convert(moduleName);
			A moduleBaseAddress = 0;
			HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processIdentifier);

			if (hSnapshot == INVALID_HANDLE_VALUE)
			{
				unsigned int codeError = GetLastError();
				printf("moduleBaseAddress() exited with %ud as code error, check CreateToolhelp32Snapshot call\n", codeError);
				return moduleBaseAddress;
			}

			MODULEENTRY32 ModuleEntry32;
			ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
			if (Module32First(hSnapshot, &ModuleEntry32))
			{
				do
				{
					if (_tcscmp(ModuleEntry32.szModule, helper::convert(moduleName)) == 0)
					{
						moduleBaseAddress = (DWORD_PTR)ModuleEntry32.modBaseAddr;
						break;
					}
				} while (Module32Next(hSnapshot, &ModuleEntry32));
			}
			CloseHandle(hSnapshot);
			delete[] _wbaseModuleName; //should implement helper::convert as a functor with a destructor that deletes this
			_wbaseModuleName = NULL;
			return moduleBaseAddress;
		}

	private:
		P* m_hProcessPtr;
	};
}
