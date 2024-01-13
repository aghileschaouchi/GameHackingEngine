#pragma once

#include <Windows.h>
#include <TlHelp32.h>

#pragma comment(lib, "kernel32.lib") 

namespace ghe
{
	template <typename A>
	class Address
	{
	public:
		Address() : m_address(0x0), m_isStatic(FALSE) {}
		Address(bool isStatic, A address) : m_address(address), m_isStatic(isStatic) {}
		~Address() {}

		void setAddress(const unsigned long address) { m_address = address; }
		void setStatic() { m_isStatic = true; }

		const A getValue() { return m_address; }

		std::string toString()
		{
			const std::string logMessage = (m_isStatic) ? "Static addres: " + std::to_string(m_address) : "Dynamic address: " + std::to_string(m_address);
			return logMessage;
		}

	private:
		A m_address;
		bool m_isStatic;
	};
}


