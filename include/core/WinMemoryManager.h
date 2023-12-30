#pragma once

#include "Address.h"

namespace
{
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
	template <typename P, typename A>
	class WinMemoryManager
	{
	public:

		// SHOULD BE ISOLATED WITH A DESIGN PATTERN
		inline ghe::Address getDynamicAddress(unsigned int pointerLevel, A* offsets, A staticAddress, A staticOffset) {
			return ghe::Address(true, FindTheAddr(m_hProcess, pointerLevel, offsets, m_baseAddress, staticAddress, staticOffset));
		}

		inline A readAddressValue(const ghe::Address &address)
		{
			A value;
			ReadProcessMemory(m_hProcess, (LPCVOID)address.getValue(), (LPVOID)&value, sizeof(value), NULL);
			return value;
		}

		inline bool writeValue(const ghe::Address& address, A& value)
		{
			if (WriteProcessMemory(hProcess, (LPVOID)address.getValue(), (LPCVOID)&value, sizeof(value), NULL) != 0)
			{
				return true;
			}
			//think about GetLastError 
			return false;
		}

	protected:
		P m_hProcess;
		A m_Address;
	};
}