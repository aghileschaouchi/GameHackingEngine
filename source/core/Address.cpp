#include "../../include/core/Address.h"

//use printf for the moment, should be replaced by Boost.Log
#include <cstdio> 

std::string ghe::Address::toString()
{
	static const std::string logMessage = (m_isStatic) ? "Static addres: " + m_address : "Dynamic address: " + m_address;
	return logMessage;
}

ghe::Address& ghe::Address::operator=(const Address& other)
{
	if (this != &other)
	{
		m_address = other.m_address;
		m_isStatic = other.m_isStatic;
	}
	return *this;
}

ghe::Address& ghe::Address::operator=(Address&& other) noexcept
{
	if (this != &other)
	{
		//put in place deletes
		m_address = std::move(other.m_address);
		m_isStatic = std::move(other.m_isStatic);
	}
	return *this;
}
