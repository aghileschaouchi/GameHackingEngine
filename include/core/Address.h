#pragma once

#include <string>
#include <utility>

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
		Address(unsigned int& address, bool isStatic = false) : m_isStatic(isStatic), m_address(address) {}
		Address(unsigned int&& address, bool&& isStatic = false) : m_isStatic(std::move(isStatic)), m_address(std::move(address)) {}

		Address& operator=(const Address& other);
		Address& operator=(Address&& other) noexcept;

		std::string toString();

	private:
		unsigned int m_address;
		bool m_isStatic;
	};
}


