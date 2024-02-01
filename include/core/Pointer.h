#pragma once

#include "Address.h"

#include <vector>

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
	template <typename A, typename O>
	class Pointer
	{
	public:
		Pointer() = default;
		~Pointer() {}
		
		//copy
		Pointer(ghe::Address<A>& baseAddressContent, std::vector<O>& offsets) = delete;
		Pointer(std::unique_ptr<ghe::Address<A>>& baseAddress, std::vector<O>& offsets) = delete;
		
		//move
		Pointer(ghe::Address<A>&& baseAddressContent, std::vector<O>&& offsets) : m_baseAddress(std::move(baseAddress)), m_offsets(std::move(offsets)) 
		{
			m_baseAddress = std::make_unique<ghe::Address<A>>();
			std::move(&baseAddressContent, &baseAddressContent + 1, m_baseAddress.get());
		}

		Pointer(std::unique_ptr<ghe::Address<A>>&& baseAddress, std::vector<O>&& offsets) : m_baseAddress(std::move(baseAddress)), m_offsets(std::move(offsets)) 
		{
			m_baseAddress = std::make_unique<ghe::Address<A>>();
			std::move(baseAddress.get(), baseAddress.get() + 1, m_baseAddress.get());
		}

		//add move semantics methods (add them to WinGame also)

		inline A pointedAddressValue()
		{

		}

		inline ghe::Address<A> pointedAddress()
		{

		}


	private:
		std::unique_ptr<ghe::Address<A>> m_baseAddress;
		std::vector<O> m_offsets;
	};
}


