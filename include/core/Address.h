#pragma once

#include <iostream>

#include <Windows.h>
#include <SDKDDKVer.h>
#include <TlHelp32.h>
#include <tchar.h>

namespace ghe
{
	class Address
	{
	public:
		explicit Address() : m_address(0x0), m_isStatic(false) {}
		Address(bool isStatic = false) : m_isStatic(isStatic), m_address(0x0) {}
		Address(unsigned __int3264& address, bool isStatic = false) : m_isStatic(isStatic), m_address(address) {}
		Address(unsigned __int3264&& address, bool&& isStatic = false) : m_isStatic(std::move(isStatic)), m_address(std::move(address)) {}

		Address& operator=(const Address& other);
		Address& operator=(Address&& other) noexcept;

		void log()
		{
			std::cout << m_address << std::hex << std::endl;
		}

	private:
		unsigned __int3264 m_address;
		bool m_isStatic;
	};
}


