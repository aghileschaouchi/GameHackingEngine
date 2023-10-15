#pragma once

#include <memory>
#include <Windows.h>
#include <SDKDDKVer.h>
#include <TlHelp32.h>
#include <tchar.h>

#include "Address.h"

class Program
{
public:
	explicit Program() : m_baseAddress(nullptr),  m_pid(0) { }

	virtual ~Program() = 0;

	explicit Program(std::unique_ptr<ghe::Address>& baseAddress, unsigned long& pid, std::string& programName) : m_pid(pid), m_programName(programName)
	{
		m_baseAddress = std::make_unique<ghe::Address>();
		std::copy(baseAddress.get(), baseAddress.get() + 1, m_baseAddress.get());
	}

	explicit Program(std::unique_ptr<ghe::Address>&& baseAddress, unsigned long&& pid, std::string&& programName) : m_pid(std::move(pid)), m_programName(std::move(programName))
	{
		m_baseAddress = std::make_unique<ghe::Address>();
		std::move(baseAddress.get(), baseAddress.get() + 1, m_baseAddress.get());
	}

	const ghe::Address* baseAddress() const { return m_baseAddress.get(); }
	const unsigned long pid() const { return m_pid; }
	const std::string processName() const { return m_programName; }

	void setBaseAddress(const std::unique_ptr<ghe::Address>& baseAddress) {}
	void setPid(unsigned long& pid) {}
	void processName(const std::string& processName) {}


	virtual void log() = 0;
	virtual void setupPid() = 0;
	virtual void setupBaseAddress() = 0;

	virtual Program& operator=(const Program& other) = 0;
	virtual Program& operator=(Program&& other) noexcept = 0;

protected:
	std::unique_ptr<ghe::Address> m_baseAddress;
	unsigned long m_pid;
	std::string m_programName;
};

