#pragma once

#include <Windows.h>
#include <SDKDDKVer.h>
#include <TlHelp32.h>
#include <tchar.h>

class Process
{
public:
	explicit Process() :m_baseAddress(NULL), m_hProcess(NULL), m_pid(NULL), m_hwnd(NULL), m_processName(NULL), m_baseModuleName(NULL) {}

	virtual ~Process() = 0;

	explicit Process(DWORD_PTR baseAddress, HANDLE hProcess, DWORD pid, HWND gameHwnd, LPCWSTR processName, LPCWSTR baseModuleName) :
		m_baseAddress(baseAddress), m_hProcess(hProcess), m_pid(pid), m_hwnd(gameHwnd), m_processName(processName), m_baseModuleName(baseModuleName)
	{
	}

	explicit Process(DWORD_PTR&& baseAddress, HANDLE&& hProcess, DWORD&& pid, HWND&& gameHwnd, LPCWSTR&& processName, LPCWSTR&& baseModuleName) :
		m_baseAddress(std::move(baseAddress)), m_hProcess(std::move(hProcess)), m_pid(std::move(pid)), m_hwnd(std::move(gameHwnd)),
		m_processName(std::move(processName)), m_baseModuleName(std::move(baseModuleName))
	{
	}

	const DWORD_PTR baseAddress() const { return m_baseAddress; }
	const HANDLE hProcess() const { return m_hProcess; }
	const DWORD pid() const { return m_pid; }
	const HWND hwnd() const { return m_hwnd; }
	const LPCWSTR processName() const { return m_processName; }
	const LPCWSTR baseModuleName() const { return m_baseModuleName; }

	void setBaseAddress(DWORD_PTR baseAddress) {}
	void setHProcess(const HANDLE& hProcess) {}
	void setPid(const DWORD& pid) {}
	void setHwnd(const HWND& hwnd) {}
	void processName(const std::string& processName) {}
	void processName(const std::string& baseModuleName) {}


	virtual void log() = 0;
	virtual void setupHwnd() = 0;
	virtual void setupPid() = 0;
	virtual void setupHProcess() = 0;
	virtual void setupBaseAddress() = 0;

	virtual Process& operator=(const Process& other) = 0;
	virtual Process& operator=(Process&& other) noexcept = 0;

protected:
	DWORD_PTR m_baseAddress;
	HANDLE m_hProcess;
	DWORD m_pid;
	HWND m_hwnd;
	LPCWSTR m_processName;
	LPCWSTR m_baseModuleName; //should be casted into a TCHAR[]
};

