#pragma once

#include <memory>
#include <string>

#include "Address.h"

template <typename T, typename S>
class Program
{
public:
	Program() : m_baseAddress(nullptr), m_pid(0) { }

	virtual ~Program() = 0;

	Program(std::unique_ptr<ghe::Address>& baseAddress, T& pid, S& programName) : m_pid(pid), m_programName(programName)
	{
		m_baseAddress = std::make_unique<ghe::Address>();
		std::copy(baseAddress.get(), baseAddress.get() + 1, m_baseAddress.get());
	}

	Program(std::unique_ptr<ghe::Address>&& baseAddress, T&& pid, S&& programName) : m_pid(std::move(pid)), m_programName(std::move(programName))
	{
		m_baseAddress = std::make_unique<ghe::Address>();
		std::move(baseAddress.get(), baseAddress.get() + 1, m_baseAddress.get());
	}

	const ghe::Address* baseAddress() const { return m_baseAddress.get(); }
	const T pid() const { return m_pid; }
	const std::string processName() const { return m_programName; }

	void setBaseAddress(const std::unique_ptr<ghe::Address>& baseAddress) {}
	void setPid(const T& pid) {}
	void processName(const S& processName) {}

	virtual void log() = 0;

protected:
	std::unique_ptr<ghe::Address> m_baseAddress;
	//unsigned long m_pid;
	//std::string m_programName;
	T m_pid;
	S m_programName;
};

