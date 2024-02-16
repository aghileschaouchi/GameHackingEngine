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

		//copying the baseAddress but moving the offsets
		Pointer(ghe::Address<A>* baseAddress, std::vector<O>&& offsets) :m_offsets(std::move(offsets)) //this method is not working, check it later
		{
			m_baseAddress = std::make_unique<ghe::Address<A>>(baseAddress);
		}

		//add move semantics methods (add them to WinGame also)

		const A pointedAddressValue() const
		{
			return m_baseAddress.get()->getAddress();
		}

		inline ghe::Address<A>* pointedAddress() const
		{
			return m_baseAddress.get();
		}
		
		inline ghe::Address<A> pointedAddressCopy() const
		{
			return ghe::Address<A>(m_baseAddress.get()->getAddress(), m_baseAddress.get()->isStatic());
		}

		inline ghe::Address<A>& baseAddressDeref() const //alwyas prefer to use this one
		{
			if (m_baseAddress)
			{
				return *m_baseAddress.get();
			}
		}

		const std::vector<O>& offsets() const
		{
			return m_offsets;
		}

		ghe::Address<A> dynamicAddress(const HANDLE& hProcess)
		{
			A tmp_addr = NULL;
			A dynamicAddress = m_baseAddress.get()->value();

			for (size_t i = 0; i < m_offsets.size() - 1; ++i)
			{
				dynamicAddress += m_offsets[i];
				ReadProcessMemory(hProcess, (LPCVOID)(dynamicAddress), (LPVOID)&tmp_addr, sizeof(tmp_addr), NULL);
				dynamicAddress = tmp_addr;
			}
			dynamicAddress += m_offsets[m_offsets.size() - 1];

			return ghe::Address<A>(false, dynamicAddress);
		}


	private:
		std::unique_ptr<ghe::Address<A>> m_baseAddress;
		std::vector<O> m_offsets;
	};
}


