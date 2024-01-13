#pragma once

#include <Windows.h>
#include <TlHelp32.h>

#include "Process.h"

#pragma comment(lib, "kernel32.lib") 

namespace ghe
{
	template<typename T, typename P, typename H, typename A>
	class WinProcess : public Process<T, A>
	{
	public:
		using Process<T, A>::Process;
		//copy
		WinProcess(std::unique_ptr<ghe::Address<A>>& baseAddress, T& pid, std::string& programName, P& hProcess, H& hwnd) = delete;
		WinProcess(ghe::Address<A>& baseAddressContent, T& pid, std::string& programName, P& hProcess, H& hwnd) = delete;
		//move
		WinProcess(std::unique_ptr<ghe::Address<A>>&& baseAddress, T&& pid, std::string&& programName, P&& hProcess, H&& hwnd) :
			Process<T, A>(std::move(baseAddress), std::forward<T>(pid), std::move(programName)), m_hProcess(std::move(hProcess)), m_hwnd(std::move(hwnd)) {}
		
		WinProcess(ghe::Address<A>&& baseAddressContent, T&& pid, std::string&& programName, P&& hProcess, H&& hwnd) :
			Process<T, A>(std::move(baseAddressContent), std::forward<T>(pid), std::move(programName)), m_hProcess(std::move(hProcess)), m_hwnd(std::move(hwnd)) {}

		virtual ~WinProcess() {}

	protected:
		P m_hProcess;
		H m_hwnd;
	};
}//namespace ghe

