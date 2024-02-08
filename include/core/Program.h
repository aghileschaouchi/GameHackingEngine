#pragma once

#include "Address.h"

template <typename T, typename A>
class Program
{
public:
	Program() : m_baseAddress(nullptr), m_pid(0) {}
	~Program() {};

	//copy
	Program(ghe::Address<A>& baseAddressContent, T& pid, std::string& programName) = delete;
	Program(std::unique_ptr<ghe::Address<A>>& baseAddress, T& pid, std::string& programName) = delete;

	//move
	Program(ghe::Address<A>&& baseAddressContent, T&& pid, std::string&& programName) : m_pid(std::move(pid)), m_programName(std::move(programName))
	{
		m_baseAddress = std::make_unique<ghe::Address<A>>();
		std::move(&baseAddressContent, &baseAddressContent + 1, m_baseAddress.get());
	}

	Program(std::unique_ptr<ghe::Address<A>>&& baseAddress, T&& pid, std::string&& programName) : m_pid(std::move(pid)), m_programName(std::move(programName))
	{
		m_baseAddress = std::make_unique<ghe::Address<A>>();
		std::move(baseAddress.get(), baseAddress.get() + 1, m_baseAddress.get());
	}

	ghe::Address<A>* baseAddress() const { return m_baseAddress.get(); } //should return It const and handle It outside? It should return a ref & instead of pointer
	ghe::Address<A> baseAddressCopy() const { return ghe::Address(m_baseAddress.get()->isStatic(), m_baseAddress.get()->getAddress()) } //not efficient, think about It!
	const T pid() const { return m_pid; }
	const std::string processName() const { return m_programName; }

	void setBaseAddress(const std::unique_ptr<ghe::Address<A>>& baseAddress) { this->m_baseAddress = std::make_unique<ghe::Address<A>>(*baseAddress); }
	void setPid(const T& pid) { m_pid = pid; }
	void setProcessName(const std::string& processName) { m_programName = processName; }

	virtual void log() = 0;

protected:
	std::unique_ptr<ghe::Address<A>> m_baseAddress;
	T m_pid;
	std::string m_programName; //prefer to use only pure C++ types
};

