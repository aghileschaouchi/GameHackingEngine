#pragma once

#include "Program.h"

//use printf for the moment, should be replaced by Boost.Log
#include <cstdio>

namespace ghe
{
	template<typename T, typename S>
	class Process : public Program<T, S>
	{
	public:
		using Program<T, S>::Program;

		Process(std::unique_ptr<ghe::Address>& baseAddress, T& pid, S& programName) :
			Program<T, S>(baseAddress, pid, programName) {}

		Process(std::unique_ptr<ghe::Address>&& baseAddress, T&& pid, S&& programName) :
			Program<T, S>(std::forward<std::unique_ptr<ghe::Address>>(baseAddress), std::forward<T>(pid), std::forward<S>(programName)) {}

		virtual ~Process() override
		{

		}

		Process& operator=(const Process& other)
		{
			if (this != &other)
			{
				this->m_baseAddress = other.m_baseAddress;
				this->m_pid = other.m_pid;
				this->m_processName = other.m_processName;
			}
			return *this;
		}

		Process& operator=(Process&& other) noexcept
		{
			if (this != &other)
			{
				// put in place deletes
				this->m_baseAddress = std::move(other.m_baseAddress);
				this->m_pid = std::move(other.m_pid);
				this->m_processName = std::move(other.m_processName);
			}
			return *this;
		}

		virtual void log() override
		{
			static const char* processNameLogMessage = "process name: ";
			static const char* pidLogMessage = "PID:";
			static const char* baseAddressLogMessage = "base address:";
			printf("%s %s\n %s %d\n %s %x\n", processNameLogMessage, this->m_programName, pidLogMessage, this->m_pid, baseAddressLogMessage, this->m_baseAddress->toString());
		}
	};
} //end namespace ghe

