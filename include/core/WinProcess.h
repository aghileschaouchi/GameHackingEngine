#pragma once

#include <Windows.h>
#include <TlHelp32.h>

#include "Process.h"

#pragma comment(lib, "kernel32.lib") 

namespace ghe
{
	template<typename T, typename S, typename P, typename H, typename A>
	class WinProcess : public Process<T, S, A>
	{
	public:
		using Process<T, S, A>::Process;
		//copy
		WinProcess(std::unique_ptr<ghe::Address<A>>& baseAddress, T& pid, S& programName, P& hProcess, H& hwnd) = delete;
		WinProcess(ghe::Address<A>& baseAddressContent, T& pid, S& programName, P& hProcess, H& hwnd) = delete;
		//move
		WinProcess(std::unique_ptr<ghe::Address<A>>&& baseAddress, T&& pid, S&& programName, P&& hProcess, H&& hwnd) :
			Process<T, S, A>(std::move(baseAddress), std::forward<T>(pid), std::forward<S>(programName)), m_hProcess(std::move(hProcess)), m_hwnd(std::move(hwnd)) {}
		
		WinProcess(ghe::Address<A>&& baseAddressContent, T&& pid, S&& programName, P&& hProcess, H&& hwnd) :
			Process<T, S, A>(std::move(baseAddressContent), std::forward<T>(pid), std::forward<S>(programName)), m_hProcess(std::move(hProcess)), m_hwnd(std::move(hwnd)) {}

		virtual ~WinProcess() {}

	protected:
		P m_hProcess;
		H m_hwnd;
	};
}//namespace ghe

