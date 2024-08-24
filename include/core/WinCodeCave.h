#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>

#include "../helper/Helper.h"

namespace ghe
{
	namespace winCodeCave
	{
		namespace ASM
		{
			const DWORD NOP = 0x90;
			const DWORD CALL = 0xE8;
			const DWORD JMP = 0xE9;
		}

		MODULEINFO getModuleInfo(const std::string& module)
		{
			MODULEINFO _moduleInfo = { 0 };
			HMODULE _hModule = GetModuleHandleA(module.c_str());
			if (_hModule != 0)
			{
				GetModuleInformation(GetCurrentProcess(), _hModule, &_moduleInfo, sizeof(MODULEINFO));
			}
			return _moduleInfo;
		}

		//move to PatternScanning file
		DWORD findPattern(const std::string& module, const std::string& pattern, const std::string& mask)
		{
			MODULEINFO _moduleInfo = getModuleInfo(module);
			DWORD size = static_cast<DWORD>(_moduleInfo.SizeOfImage);
			DWORD base = reinterpret_cast<DWORD>(_moduleInfo.lpBaseOfDll);

			for (DWORD i = 0; i < size - mask.length(); ++i)
			{
				bool found = true;
				for (DWORD j = 0; j < mask.length(); ++j)
				{
					found &= mask.at(j) == '?' || pattern.at(j) == *reinterpret_cast<char*>(base + i + j);
				}
				if (found)
				{
					return base + i;
				}
			}
			return 0;
		}

		bool writeIntoMemory1(uintptr_t destAddress, const char* patch, size_t numBytes)
		{
			unsigned long oldProtection;

			if (VirtualProtect(reinterpret_cast<LPVOID>(destAddress), static_cast<size_t>(numBytes), PAGE_EXECUTE_READWRITE, &oldProtection) == NULL)
			{
				printf("writeIntoMemory1() exited with %ud as code error, check VirtualProtect call 1\n", GetLastError());
				return false;
			}

			if (memcpy(reinterpret_cast<void*>(destAddress), reinterpret_cast<const void*>(patch), numBytes) != reinterpret_cast<void*>(destAddress))
			{
				printf("writeIntoMemory1() failed, check memcpy\n");
				return false;
			}

			if (VirtualProtect(reinterpret_cast<LPVOID>(destAddress), static_cast<size_t>(numBytes), oldProtection, NULL) == NULL)
			{
				printf("writeIntoMemory1() exited with %ud as code error, check VirtualProtect call 2\n", GetLastError());
				return false;
			}
			return true;
		}

		//bool WriteIntoMemory2(uintptr_t destAddress, LPVOID patch, size_t numBytes)
		//{
		//	unsigned long  oldProtection = 0;
		//	// Store the source address
		//	DWORD srcAddress = PtrToUlong(patch);

		//	if (VirtualProtect(reinterpret_cast<LPVOID>(destAddress), static_cast<size_t>(numBytes), PAGE_EXECUTE_READWRITE, &oldProtection) == NULL)
		//	{
		//		printf("WriteIntoMemory2() exited with %ud as code error, check VirtualProtect call 1\n", GetLastError());
		//		return false;
		//	}

		//	//SHOULD FIX THIS PART! use "__asm" for each line?
		//	//__asm
		//	//{
		//	//	nop
		//	//	nop
		//	//	nop

		//	//	mov esi, srcAddress		// Save the address
		//	//	mov edi, destAddress	// Save the destination address
		//	//	mov ecx, numBytes		// Save the size of the patch
		//	//	Start :
		//	//	cmp ecx, 0				// Are we done yet?
		//	//		jz Exit					// If so, go to end of function

		//	//		mov al, [esi]			// Move the byte at the patch into AL
		//	//		mov[edi], al			// Move AL into the destination byte
		//	//		dec ecx					// 1 less byte to patch
		//	//		inc esi					// Next source byte
		//	//		inc edi					// Next destination byte
		//	//		jmp Start				// Repeat the process
		//	//		Exit :
		//	//	nop
		//	//		nop
		//	//		nop
		//	//}

		//	if (VirtualProtect(reinterpret_cast<LPVOID>(destAddress), static_cast<size_t>(numBytes), oldProtection, NULL) == NULL)
		//	{
		//		printf("writeIntoMemory1() exited with %ud as code error, check VirtualProtect call 2\n", GetLastError());
		//		return false;
		//	}
		//	return true;
		//}

		//by default its a jump code cave
		template<unsigned int INSTRUCTION = ASM::JMP>
		bool codeCave1(uintptr_t destAddress, const DWORD& funcAddress, const DWORD& length)
		{
			unsigned long oldProtection;
			if (VirtualProtect(reinterpret_cast<LPVOID>(destAddress), static_cast<size_t>(length), PAGE_EXECUTE_READWRITE, &oldProtection) == NULL)
			{
				printf("jumpCodeCave() exited with %ud as code error, check VirtualProtect call 1\n", GetLastError());
				return false;
			}

			DWORD offset = static_cast<DWORD>(funcAddress - static_cast<DWORD>(destAddress)) - 5;

			if constexpr (INSTRUCTION == ASM::JMP)
			{
				*(reinterpret_cast<DWORD*>(destAddress)) = ASM::JMP;
			}
			else if constexpr (INSTRUCTION == ASM::CALL)
			{
				*(reinterpret_cast<DWORD*>(destAddress)) = ASM::CALL;
			}

			*(reinterpret_cast<DWORD*>(destAddress + 0x1)) = offset;

			for (DWORD i = 0x5; i < length; ++i)
			{
				*(reinterpret_cast<DWORD*>(destAddress)+i) = ASM::NOP;
			}

			if (VirtualProtect(reinterpret_cast<LPVOID>(destAddress), static_cast<size_t>(length), static_cast<DWORD>(oldProtection), NULL) == NULL)
			{
				printf("jumpCodeCave() exited with %ud as code error, check VirtualProtect call 2\n", GetLastError());
				return false;
			}
			return true;
		}

		//template<unsigned int INSTRUCTION = ASM::CALL>
		//bool codeCave2(uintptr_t destAddress, VOID(*func)(VOID), BYTE nopCount)
		//{
		//	//calculate the code cave for chat interception : offset = (PtrToUlong(func) - destAddress) - 5
		//	DWORD offset = (reinterpret_cast<DWORD>(func) - static_cast<DWORD>(destAddress)) - 5;

		//	//buffer of NOPs, static since we limit to 'UCHAR_MAX' NOPs
		//	BYTE nopPatch[0xFF] = { 0 };

		//	BYTE patch[5] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
		//	if constexpr (INSTRUCTION == ASM::CALL)
		//	{
		//		patch[0] = ASM::CALL;
		//	}

		//	if constexpr (INSTRUCTION == ASM::JMP)
		//	{
		//		patch[0] = ASM::JMP;
		//	}

		//	memcpy(patch + 1, &offset, sizeof(DWORD));
		//	if (!WriteIntoMemory2(destAddress, static_cast<size_t>(patch), 5))
		//	{
		//		return false;
		//	}

		//	if (nopCount == 0)
		//		return true;

		//	memset(nopPatch, ASM::NOP, nopCount);

		//	return WriteIntoMemory2(destAddress + 5, reinterpret_cast<LPVOID>(nopPatch), static_cast<size_t>(nopCount));
		//}
	}
}