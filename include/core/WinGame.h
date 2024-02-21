#pragma once

#include "WinMemoryManager.h"
#include "WinProcess.h"
#include "../helper/Helper.h"

namespace ghe
{
	template <typename T, typename P, typename H, typename A, typename PB>
	class WinGame : public WinProcess<T, P, H, A>
	{
	public:
		//copy
		WinGame(std::unique_ptr<ghe::Address<A>>& baseAddress, T& pid, std::string& programName, P& hProcess, H& hwnd, std::string& baseModuleName) = delete;
		WinGame(ghe::Address<A>& baseAddressContent, T& pid, std::string& programName, P& hProcess, H& hwnd, std::string& baseModuleName) = delete;

		//move
		WinGame(std::unique_ptr<ghe::Address<A>>&& baseAddress, T&& pid, std::string&& programName, P&& hProcess, H&& hwnd, std::string&& baseModuleName) :
			WinProcess<T, P, H, A>(std::move(baseAddress), std::forward<T>(pid), std::move(programName),
				std::forward<P>(hProcess), std::forward<H>(hwnd)), m_baseModuleName(std::move(baseModuleName)) { //assuming that V would be A DWORD in every windows game
			setupHwnd();
			setupPid();
			setupHProcess();
			setupBaseAddress();
			m_memoryManager.setHProcess(&this->m_hProcess);
			log();
		}

		WinGame(ghe::Address<A>&& baseAddressContent, T&& pid, std::string&& programName, P&& hProcess, H&& hwnd, std::string&& baseModuleName) :
			WinProcess<T, P, H, A>(std::move(baseAddressContent), std::forward<T>(pid), std::move(programName),
				std::forward<P>(hProcess), std::forward<H>(hwnd)), m_baseModuleName(std::move(baseModuleName)) {
			setupHwnd();
			setupPid();
			setupHProcess();
			setupBaseAddress();
			m_memoryManager.setHProcess(&this->m_hProcess);
			log();
		}

		virtual ~WinGame() override
		{
			if (this->m_hProcess != NULL)
			{
				CloseHandle(this->m_hProcess);
			}
		}

		inline void setupBaseAddress()
		{
			
			DWORD_PTR _baseModuleAddress = m_memoryManager.moduleBaseAddress(this->m_pid, m_baseModuleName);
			this->m_baseAddress.get()->setAddress(static_cast<DWORD_PTR>(_baseModuleAddress));
			this->m_baseAddress.get()->setStatic(); //access to unique_ptr's Address value to be checked
		}

		inline void setupHwnd()
		{
			this->m_hwnd = FindWindow(NULL, helper::constConvert(this->m_programName));
			if (this->m_hwnd == NULL)
			{
				unsigned int codeError = GetLastError();
				printf("setupHwnd() exited with %ud as code error, check FindWindow call\n", codeError);
			}
		}

		inline void setupPid()
		{
			if (GetWindowThreadProcessId(this->m_hwnd, &this->m_pid) == 0)
			{
				unsigned int codeError = GetLastError();
				printf("setupPid() exited with %ud as code error, check GetWindowThreadProcessId call\n", codeError);
			}
		}

		inline void setupHProcess()
		{
			this->m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->m_pid);
			if (this->m_hProcess == NULL)
			{
				unsigned int codeError = GetLastError();
				printf("setupHProcess() exited with %ud as code error, check OpenProcess call\n", codeError);
			}
		}

		WinGame& operator=(const WinGame& other) = delete;

		WinGame& operator=(WinGame&& other) noexcept
		{
			if (this != &other)
			{
				this->m_baseAddress.reset();
				this->m_baseAddress = std::move(other.m_baseAddress);
				this->m_hProcess = std::move(other.m_hProcess);
				this->m_pid = std::move(other.m_pid);
				this->m_hwnd = std::move(other.m_hwnd);
				this->m_processName = std::move(other.m_processName);
				this->m_baseModuleName = std::move(other.m_baseModuleName);
				this->m_memoryManager = std::move(other.m_memoryManager);
			}
			return *this;
		}

		void log() override
		{
			static const char* processNameMessage = "process name: "; //%s
			static const char* baseModuleNameMessage = "base module name: "; //%s
			static const char* pidMessage = "pid:"; //%ld
			static const char* hwndMessage = "hwnd:"; //%p
			static const char* hProcessMessage = "hProcess:"; //%p
			static const char* baseAddressMessage = "base address:"; //%s
			printf("%s %s\n %s %s\n %s %ld\n %s %p\n %s %p\n %s ", processNameMessage, this->m_programName.c_str(), baseModuleNameMessage, this->m_baseModuleName.c_str(), pidMessage, this->m_pid,
				hwndMessage, this->m_hwnd, hProcessMessage, this->m_hProcess, baseAddressMessage);
			this->m_baseAddress->log();
		}

		ghe::WinMemoryManager<T, P, H, A, PB>& memoryManager() { return this->m_memoryManager; }

	private:
		std::string m_baseModuleName;
		ghe::WinMemoryManager<T, P, H, A, PB> m_memoryManager;
	};
}

