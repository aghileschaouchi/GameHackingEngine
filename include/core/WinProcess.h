#pragma once

#include <Windows.h>
#include <SDKDDKVer.h>
#include <TlHelp32.h>
#include <tchar.h>

#include "Process.h"

namespace ghe
{
	template<typename T, typename S, typename P, typename H>
	class WinProcess : public Process<T, S>
	{
	public:
		using Process<T, S>::Process;

		WinProcess(std::unique_ptr<ghe::Address>& baseAddress, T& pid, S& programName, P& hProcess, H& hwnd) :
			Process<T, S>(baseAddress, pid, programName), m_hProcess(hProcess), m_hwnd(hwnd) {}

		WinProcess(std::unique_ptr<ghe::Address>&& baseAddress, T&& pid, S&& programName, P&& hProcess, H&& hwnd) :
			Process<T, S>(std::forward<std::unique_ptr<ghe::Address>>(std::move(baseAddress)), std::forward<T>(std::move(pid)), std::forward<S>(std::move(programName))), m_hProcess(std::move(hProcess)), m_hwnd(std::move(hwnd)) {}

		virtual ~WinProcess()
		{

		}

		WinProcess& operator=(const WinProcess& other) //ask if it can be virtual?
		{
			if (this != &other)
			{
				this->baseAddress = std::make_unique<ghe::Address>(*other.m_baseAddress);
				this->m_pid = other.m_pid;
				this->m_processName = other.m_processName;
				this->m_hProcess = other.m_hProcess;
				this->m_hwnd = other.m_hwnd;
			}
			return *this;
		}

		WinProcess& operator=(WinProcess&& other) noexcept
		{
			if (this != &other)
			{
				this->m_baseAddress.reset();
				this->m_baseAddress = std::move(other.m_baseAddress);
				this->m_pid = std::move(other.m_pid);
				this->m_programName = std::move(other.m_programName);
				this->m_hProcess = std::move(other.m_hProcess);
				this->m_hwnd = std::move(other.m_hwnd);
			}
			return *this;
		}

		inline void setupHwnd()
		{
			m_hwnd = FindWindow(NULL, this->m_programName);
		}

		inline void setupPid()
		{
			GetWindowThreadProcessId(m_hwnd, &this->m_pid);
		}

		inline void setupHProcess()
		{
			m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->m_pid);
		}

	protected:
		P m_hProcess;
		H m_hwnd;
	};
}//namespace ghe

