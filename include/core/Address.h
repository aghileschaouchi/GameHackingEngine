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
		Address(bool isStatic = false) : m_isStatic(isStatic) {}

		void log()
		{
			std::cout << m_address << std::hex << std::endl;
		}

	private:
		DWORD m_address;
		bool m_isStatic;
	};
}


