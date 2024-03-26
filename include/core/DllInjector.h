#pragma once

#include <cstdio>
#include <Windows.h>
#include <TlHelp32.h>

#include "Injector.h"

namespace ghe
{
	template <typename T, typename P, typename H>
	class DllInjector : public Injector
	{
	public:
		DllInjector(T* pidPtr, H* hProcessPtr, const std::string& dllPath, const std::string& gameName) : m_pidPtr(pidPtr), m_hProcessPtr(hProcessPtr),
			m_dllPath(dllPath), m_gameName(gameName), m_injectionLocation(nullptr) {}
		DllInjector(T* pidPtr, H* hProcessPtr, std::string&& dllPath, std::string&& gameName) = delete;
		~DllInjector() override
		{
			if (m_injectionLocation != nullptr)
			{
				VirtualFreeEx(*m_hProcessPtr, m_injectionLocation, MAX_PATH, MEM_FREE);
			}
			m_injectionLocation = nullptr;
			m_pidPtr = nullptr;
			m_hProcessPtr = nullptr;
		}

		DllInjector& operator=(const DllInjector& other) = delete;
		DllInjector& operator=(DllInjector&& other) = delete;

		inline bool copyDllPathToGame()
		{
			m_injectionLocation = VirtualAllocEx(*m_hProcessPtr, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			if (m_injectionLocation != NULL)
			{
				if (WriteProcessMemory(*m_hProcessPtr, m_injectionLocation, m_dllPath.c_str(), m_dllPath.length() + 1, 0) == 0)
				{
					unsigned int codeError = GetLastError();
					printf("copyDllPathToGame() exited with %ud as code error, check WriteProcessMemory call\n", codeError);
					return false;
				}
				return true;
			}
			unsigned int codeError = GetLastError();
			printf("copyDllPathToGame() exited with %ud as code error, check VirtualAllocEx call\n", codeError);
			return false;
		}

		inline P createThread()
		{
			P _thread = CreateRemoteThread(*m_hProcessPtr, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryA),
				m_injectionLocation, 0, nullptr);
			if (_thread == nullptr)
			{
				CloseHandle(_thread);
				unsigned int codeError = GetLastError();
				printf("createThread() exited with %ud as code error, check CreateRemoteThread call\n", codeError);
				return 0;
			}
			return _thread;
		}

		inline bool checkTimeOut(P& thread)
		{
			if (WaitForSingleObject(thread, m_timeOut) == WAIT_TIMEOUT)
			{
				CloseHandle(thread);
				unsigned int codeError = GetLastError();
				printf("checkTimeOut() exited with %ud as code error, check WaitForSingleObject call (time out)\n", codeError);
				return true;
			}
			CloseHandle(thread);
			return false;
		}

		bool inject() override
		{
			if (*m_hProcessPtr && *m_hProcessPtr != INVALID_HANDLE_VALUE)
			{
				copyDllPathToGame();
				P _thread = createThread();
				return !checkTimeOut(_thread);
			}
		}

		void log() override
		{
			static const char* pidMsg = "injecting into process: "; //%ld
			static const char* hProcessMsg = "hProecess: "; //%p
			static const char* dllPathMsg = "dll path: "; //%s
			static const char* gameNameMsg = "game name: ";
			printf("%s %ld\n %s %p\n %s %s\n %s %s\n", pidMsg, *m_pidPtr, hProcessMsg, *m_hProcessPtr, dllPathMsg, m_dllPath.c_str(),
				gameNameMsg, m_gameName.c_str());
		}

	private:
		static const UINT8 m_timeOut = 1000;
		void* m_injectionLocation;
		T* m_pidPtr;
		H* m_hProcessPtr;
		std::string m_dllPath;
		std::string m_gameName;
	};
}
