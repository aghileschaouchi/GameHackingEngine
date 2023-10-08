#pragma once

#include <iostream>

#include "Address.h"
#include "Process.h"

namespace ghe
{
	class Program : public Process
	{
	public:
		explicit Program(DWORD_PTR baseAddress, HANDLE hProcess, DWORD pid, HWND gameHwnd, LPCWSTR processName, LPCWSTR baseModuleName) :
			Process(baseAddress, hProcess, pid, gameHwnd, processName, baseModuleName) {}

		virtual ~Program() = 0;

		explicit Program(DWORD_PTR&& baseAddress, HANDLE&& hProcess, DWORD&& pid, HWND&& gameHwnd, LPCWSTR&& processName, LPCWSTR&& baseModuleName) :
			Process(baseAddress, hProcess, pid, gameHwnd, processName, baseModuleName) {}

		virtual Program& operator=(const Program& other);
		virtual Program& operator=(Program&& other) noexcept;

		void setupHwnd()
		{
			m_hwnd = FindWindow(NULL, m_processName);
		}

		void setupPid()
		{
			GetWindowThreadProcessId(m_hwnd, &m_pid);
		}

		void setupHProcess()
		{
			m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_pid);
		}

		virtual void setupBaseAddress() = 0;

		/*inline void setupDynamicAddress(int pointerLevel, DWORD* offsets, DWORD staticOffset) {
			m_dynamicAddress = FindTheAddr(m_hProcess, pointerLevel, offsets, m_baseAddress, m_staticAddress, staticOffset);

		}*/

		//DWORD readDynamicAddrValue()
		//{
		//	DWORD value;
		//	ReadProcessMemory(m_hProcess, (LPCVOID)m_dynamicAddress, (LPVOID)&value, sizeof(value), NULL);
		//	//std::cout << "Inside function: " << value << std::endl;
		//	return value;
		//}

		//DWORD readStaticAddrValue()
		//{
		//	DWORD value;
		//	ReadProcessMemory(m_hProcess, (LPCVOID)m_staticAddress, (LPVOID)&value, sizeof(value), NULL);
		//	//std::cout << "Inside function: " << value << std::endl;
		//	return value;
		//}

		/*inline int writeValue()
		{

		}*/

		// Debug functions
		void log()
		{
			std::cout << "hProcess: " << m_hProcess << std::endl;
			std::cout << "PID: " << m_pid << std::endl;
			std::cout << "gameHwnd: " << m_hwnd << std::endl;
			std::cout << "base addresse: " << m_baseAddress << std::endl;
		}

	private:
	};
} //end namespace ghe

