#pragma once

#include <Windows.h>
#include <TlHelp32.h>

#pragma comment(lib, "kernel32.lib") 

namespace ghe
{
	template <typename A>
	class Address //make It only moveable for assignement operators (no need for constructors since primitfs types)
	{
	public:
		Address() : m_address(0x0), m_isStatic(FALSE) {}
		Address(bool isStatic, A address) : m_address(address), m_isStatic(isStatic) {}
		~Address() {}

		void setAddress(A address) { m_address = address; }
		void setStatic() { m_isStatic = true; }

		A getAddress() const { return m_address; }
		const bool isStatic() const { return m_isStatic; }

		std::string toString()
		{
			const std::string logMessage = (m_isStatic) ? "Static addres: " + std::to_string(m_address) : "Dynamic address: " + std::to_string(m_address);
			return logMessage;
		}

		void log()
		{
			printf("0x%08x\n", m_address);
		}

	private:
		A m_address;
		bool m_isStatic;
	};
}


