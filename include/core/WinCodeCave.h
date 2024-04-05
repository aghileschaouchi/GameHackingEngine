#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>

#include "../helper/Helper.h"

namespace ghe
{
	namespace codeCave
	{
		namespace
		{
			const DWORD NOP = 0x90;
			const DWORD CALL = 0xE8;
			const DWORD JMP = 0xE9;
		}

		MODULEINFO getModuleInfo(const std::string& module)
		{
			MODULEINFO _moduleInfo = { 0 };
			HMODULE _hModule = GetModuleHandle(helper::constConvert(module));
			if (_hModule != 0)
			{
				GetModuleInformation(GetCurrentProcess(), _hModule, &_moduleInfo, sizeof(MODULEINFO));
			}
			return _moduleInfo;
		}

		bool jumpCodeCave(BYTE* destAddress, const DWORD& jumpTo, const DWORD& length)
		{
			DWORD oldProtection;
			
			if (VirtualProtect(destAddress, length, PAGE_EXECUTE_READWRITE, &oldProtection) == NULL)
			{
				unsigned int codeError = GetLastError();
				printf("jumpCodeCave() exited with %ud as code error, check VirtualProtect call 1\n", codeError);
				return false;
			}
			DWORD reloadAddress = static_cast<DWORD>(jumpTo - reinterpret_cast<DWORD>(destAddress)) - 5;

			*destAddress = JMP;
			*(reinterpret_cast<DWORD*>(destAddress + 0x1)) = reloadAddress;
			for (DWORD i = 0x5; i < length; ++i)
			{
				*(destAddress + i) = NOP;
			}

			if (VirtualProtect(destAddress, length, oldProtection, NULL) == NULL)
			{
				unsigned int codeError = GetLastError();
				printf("jumpCodeCave() exited with %ud as code error, check VirtualProtect call 2\n", codeError);
				return false;
			}
			return true;
		}

		DWORD findPattern(const std::string& module, const std::string& pattern, const std::string& mask)
		{
			MODULEINFO _moduleInfo = getModuleInfo(module);
			DWORD size = static_cast<DWORD>(_moduleInfo.SizeOfImage);
			DWORD base = reinterpret_cast<DWORD>(_moduleInfo.lpBaseOfDll); //replace by LPVOID and handle it in the DLL

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

		//bool patchBytes(DWORD destAddress, LPVOID patch, DWORD numBytes)
		//{
		//	DWORD oldProtect = 0;
		//	DWORD srcAddress = reinterpret_cast<DWORD>(patch);

		//	if (VirtualProtect((void*)(destAddress), numBytes, PAGE_EXECUTE_READWRITE, &oldProtect) == 0)
		//	{
		//		unsigned int codeError = GetLastError();
		//		printf("patchBytes() exited with %ud as code error, check VirtualProtect call 1\n", codeError);
		//		return false;
		//	}

		//	__asm
		//	{
		//		nop
		//		nop
		//		nop

		//		mov esi, srcAddress
		//		mov edi, destAddress
		//		mov ecx, numBytes
		//		Start :
		//		cmp ecx, 0
		//			jz Exit

		//			mov al, [esi]
		//			mov[edi], al
		//			dec ecx
		//			inc esi
		//			inc edi
		//			jmp Start
		//			Exit :
		//		nop
		//			nop
		//			nop
		//	}

		//	if (VirtualProtect((void*)(destAddress), numBytes, oldProtect, &oldProtect) == 0)
		//	{
		//		unsigned int codeError = GetLastError();
		//		printf("patchBytes() exited with %ud as code error, check VirtualProtect call 2\n", codeError);
		//		return false;
		//	}
		//	return true;
		//}

		//bool callCodeCave(BYTE* destAddress, const DWORD& toCall, const DWORD& length)
		//{
		//	// Calculate the code cave for chat interception
		//	DWORD offset = (PtrToUlong(func) - destAddress) - 5;

		//	// Buffer of NOPs, static since we limit to 'UCHAR_MAX' NOPs
		//	BYTE nopPatch[0xFF] = { 0 };

		//	// Construct the patch to the function call
		//	BYTE patch[5] = { CALL, 0x00, 0x00, 0x00, 0x00 };
		//	memcpy(patch + 1, &offset, sizeof(DWORD));
		//	WriteBytesASM(destAddress, patch, 5);

		//	// We are done if we do not have NOPs
		//	if (nopCount == 0)
		//		return;

		//	// Fill it with nops
		//	memset(nopPatch, NOP, nopCount);

		//	// Make the patch now
		//	WriteBytesASM(destAddress + 5, nopPatch, nopCount);
		//	return false;
		//}
	}
}