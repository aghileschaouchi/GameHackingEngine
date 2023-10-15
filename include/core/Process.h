#pragma once

#include <iostream>

#include "Program.h"

namespace ghe
{
	class Process : public Program
	{
	public:
		explicit Process(DWORD_PTR baseAddress, HANDLE hProcess, DWORD pid, HWND gameHwnd, LPCWSTR processName, LPCWSTR baseModuleName) :
			Program(baseAddress, hProcess, pid, gameHwnd, processName, baseModuleName) {}

		virtual ~Process() = 0;

		explicit Process(DWORD_PTR&& baseAddress, HANDLE&& hProcess, DWORD&& pid, HWND&& gameHwnd, LPCWSTR&& processName, LPCWSTR&& baseModuleName) :
			Program(baseAddress, hProcess, pid, gameHwnd, processName, baseModuleName) {}

		virtual Process& operator=(const Process& other);
		virtual Process& operator=(Process&& other) noexcept;

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

	protected:
		std::cout << "hProcess: " << m_hProcess << std::endl;
		std::cout << "gameHwnd: " << m_hwnd << std::endl;
	};
} //end namespace ghe

