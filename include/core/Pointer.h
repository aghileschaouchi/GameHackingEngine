#pragma once

#include "Address.h"

#include <vector>

namespace
{

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
		Pointer(ghe::Address<A>&& baseAddressContent, std::vector<O>&& offsets) : m_offsets(std::move(offsets))
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

		const A pointedAddressValue() const
		{
			return m_baseAddress.get()->getAddress();
		}

		ghe::Address<A>* pointedAddress() const
		{
			return m_baseAddress.get();
		}
		
		ghe::Address<A> pointedAddress() const
		{
			return ghe::Address(m_baseAddress.get()->baseAddress(), m_baseAddress.get()->isStatic());
		}

		const std::vector<O>& offsets() const
		{
			return m_offsets;
		}

		template<typename A, typename O>
		ghe::Address<A> finalAddress(HANDLE hProcess) //to be checked!
		{
			A tmp_addr = NULL;
			A dynamicAddress = NULL;

			for (size_t i = 0; i < m_offsets.size() - 1; ++i)
			{
				dynamicAddress += offsets[i];
				ReadProcessMemory(hProcess, (LPCVOID)(dynamicAddress), (LPVOID)&tmp_addr, sizeof(tmp_addr), NULL);
				dynamicAddress = tmp_addr;
			}
			dynamicAddress += offsets[m_offsets.size()];

			return dynamicAddress;
		}


	private:
		std::unique_ptr<ghe::Address<A>> m_baseAddress;
		std::vector<O> m_offsets;
	};
}


