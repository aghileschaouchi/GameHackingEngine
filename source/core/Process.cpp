#include "../../include/core/Process.h"

ghe::Process& ghe::Process::operator=(const Process& other)
{
	if (this != &other)
	{
		m_baseAddress = other.m_baseAddress;
		m_hProcess = other.m_hProcess;
		m_pid = other.m_pid;
		m_hwnd = other.m_hwnd;
		m_processName = other.m_processName;
	}
	return *this;
}

ghe::Process& ghe::Process::operator=(Process&& other) noexcept
{
	if (this != &other)
	{
		m_baseAddress = std::move(other.m_baseAddress);
		m_hProcess = std::move(other.m_hProcess);
		m_pid = std::move(other.m_pid);
		m_hwnd = std::move(other.m_hwnd);
		m_processName = std::move(other.m_processName);
	}
	return *this;
}

//DWORD ghe::Program::readDynamicAddrValue()
//{
//	DWORD value;
//	ReadProcessMemory(m_hProcess, (LPCVOID)m_dynamicAddress, (LPVOID)&value, sizeof(value), NULL);
//	//std::cout << "Inside function: " << value << std::endl;
//	return value;
//}
//
//DWORD ghe::Program::readStaticAddrValue()
//{
//	DWORD value;
//	ReadProcessMemory(m_hProcess, (LPCVOID)m_staticAddress, (LPVOID)&value, sizeof(value), NULL);
//	//std::cout << "Inside function: " << value << std::endl;
//	return value;
//}

/*inline int ghe::Process::writeValue()
{

}*/