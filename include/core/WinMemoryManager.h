#pragma once

#include "Address.h"

namespace
{
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
	class WinMemoryManager //this class is not useful for the moment, methods has been moved to WinGame
	{
	public:

		// SHOULD BE ISOLATED WITH A DESIGN PATTERN
		//inline ghe::Address getDynamicAddress(unsigned int pointerLevel, A* offsets, A staticAddress, A staticOffset) {
		//	return ghe::Address(true, FindTheAddr(m_hProcess, pointerLevel, offsets, m_baseAddress, staticAddress, staticOffset));
		//}
	};
}