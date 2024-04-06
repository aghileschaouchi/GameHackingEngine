#pragma once

#include <cstdio>
#include <memory>

#include <fileapi.h>
#include <Windows.h>
#include <WinBase.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <Shlwapi.h>

#include "Injector.h"

#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Shlwapi.lib")

namespace ghe
{
	template <typename T, typename P, typename H>
	class DllInjector : public Injector
	{
	public:
		DllInjector(std::weak_ptr<H> hProcessPtr, const std::string& dllPath, const std::string& gameName) : m_hProcessPtr(hProcessPtr),
			m_dllPath(dllPath), m_gameName(gameName), m_injectionLocation(nullptr) {}

		DllInjector(std::weak_ptr<H> hProcessPtr, std::string&& dllPath, std::string&& gameName) = delete;

		~DllInjector() override
		{
			if (m_injectionLocation != nullptr)
			{
				if (auto&& _lockedHProcessPtr = m_hProcessPtr.lock())
				{
					VirtualFreeEx(*_lockedHProcessPtr, m_injectionLocation, static_cast<size_t>(m_maxInjectionPath), MEM_FREE);
				}
				m_injectionLocation = nullptr;
			}
		}

		DllInjector& operator=(const DllInjector& other) = delete;
		DllInjector& operator=(DllInjector&& other) = delete;

		inline bool isDllPathInvalid()
		{
			DWORD result = GetFileAttributesA(m_dllPath.c_str());
			return (result == INVALID_FILE_ATTRIBUTES || (result & FILE_ATTRIBUTE_DIRECTORY));
		}

		inline bool copyDllPathToGame()
		{
			if (auto&& _lockedHProcessPtr = m_hProcessPtr.lock())
			{
				m_injectionLocation = VirtualAllocEx(*_lockedHProcessPtr, 0, static_cast<size_t>(m_maxInjectionPath), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

				if (m_injectionLocation != NULL)
				{
					if (WriteProcessMemory(*_lockedHProcessPtr, m_injectionLocation, m_dllPath.c_str(), static_cast<size_t>(m_maxInjectionPath / 2), nullptr) == false)
					{
						printf("copyDllPathToGame() exited with %ud as code error, check WriteProcessMemory call\n", GetLastError());
						return false;
					}
					return true;
				}
				printf("copyDllPathToGame() exited with %ud as code error, check VirtualAllocEx call\n", GetLastError());
				return false;
			}
			printf("copyDllPathToGame() exited because there is an issue with its hProcess, check its lifetime!\n");
			return false;
		}

		inline P createThread()
		{
			if (auto&& _lockedHProcessPtr = m_hProcessPtr.lock())
			{
				FARPROC _loadLibraryAddress = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
				P _thread = CreateRemoteThread(*_lockedHProcessPtr, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(_loadLibraryAddress),
					m_injectionLocation, 0, nullptr);
				if (_thread == nullptr)
				{
					CloseHandle(_thread);
					printf("createThread() exited with %ud as code error, check CreateRemoteThread call\n", GetLastError());
					return nullptr;
				}
				return _thread;
			}
			printf("createThread() exited because there is an issue with its hProcess, check its lifetime!\n");
			return nullptr;
		}

		inline bool reachedTimeOut(P& thread)
		{
			if (WaitForSingleObject(thread, m_timeOut) == WAIT_TIMEOUT)
			{
				CloseHandle(thread);
				printf("checkTimeOut() exited with %ud as code error, check WaitForSingleObject call (time out)\n", GetLastError());
				return true;
			}
			CloseHandle(thread);
			return false;
		}

		bool inject() override
		{
			if (isDllPathInvalid())
			{
				printf("inject() failed because the dll's path is invalid!\n");
				return false;
			}
			//debug
			printf("injecting %s... the dll path is correct!\n", m_dllPath.c_str());

			if (auto&& _lockedHProcessPtr = m_hProcessPtr.lock())
			{
				if (*_lockedHProcessPtr != INVALID_HANDLE_VALUE)
				{
					if (!copyDllPathToGame())
					{
						printf("inject() exited because copyDllPathToGame() failed!\n");
						return false;
					}

					P _thread = createThread();
					if (_thread == nullptr)
					{
						printf("inject() exited because createThread() failed!\n");
						return false;
					}

					return !reachedTimeOut(_thread);
				}
				printf("inject() exited because there is an issue with its hProcess, check its validity!\n");
				return false;
			}
			printf("inject() exited because there is an issue with its hProcess, check its lifetime!\n");
			return false;
		}

		void log() override
		{
			static const char* hProcessMsg = "injecting into process hProecess: "; //%p
			static const char* dllPathMsg = "dll path: "; //%s
			static const char* gameNameMsg = "game name: ";
			if (auto&& _lockedHProcessPtr = m_hProcessPtr.lock())
			{
				printf("%s %p\n %s %s\n %s %s\n", hProcessMsg, *_lockedHProcessPtr, dllPathMsg, m_dllPath.c_str(),
					gameNameMsg, m_gameName.c_str());
				return;
			}
			printf("log() exited because there is an issue with its hProcess, check its lifetime!\n");
		}

	private:
		static const UINT16 m_timeOut = 1000;
		static const UINT16 m_maxInjectionPath = 520;
		void* m_injectionLocation;
		std::weak_ptr<H> m_hProcessPtr;
		std::string m_dllPath;
		std::string m_gameName;
	};
}
