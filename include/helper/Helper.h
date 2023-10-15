#pragma once

#include <cstring> 
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <tchar.h>
#include <string>
#include <Windows.h>

namespace helper
{
	TCHAR* convertLPCWSTRToTcharPtr(LPCWSTR input)
	{
		TCHAR* output = NULL;
		if (input != NULL && input[0] != _T('0'))
		{
			const int outputLength = WideCharToMultiByte(CP_UTF8, 0, input, -1, NULL, 0, NULL, NULL);
			output = new TCHAR[outputLength + 1];
			//...

		}
		return output;
	}

	TCHAR* convertStringToTCHARPtr(const std::string& string)
	{
		TCHAR* result = NULL;
		if (string.size() != 0)
		{
			result = new TCHAR[string.size() + 1];
			result[string.size()] = '\0';
			std::copy(string.begin(), string.end(), result);
		}
		return result;
	}

	std::string convertTCHARPtrToString(TCHAR* input)
	{
		std::string result = "";

		if (input != NULL && input[0] != _T('\0'))
		{
			char* charBuffer = NULL;
			int bufferLength = lstrlenW(input);

			charBuffer = (char*)calloc(bufferLength + 1, sizeof(char));

			for (int i = 0; i < bufferLength; ++i)
			{
				char* singleCharacter = (char*)calloc(2, sizeof(char));
				singleCharacter[0] = static_cast<char>(input[i]);
				singleCharacter[1] = '\0';
				strcat_s(charBuffer, sizeof(charBuffer), singleCharacter);
				free(singleCharacter);
			}
			strcat_s(charBuffer, sizeof(charBuffer), "\0");
			std::string returnValue(charBuffer);
			free(charBuffer);
		}
		return result;
	}

	std::string convertLPCWSTRToString(LPCWSTR input)
	{
		if (input != NULL && input[0] != _T('0'))
		{
			int outputLength = WideCharToMultiByte(CP_UTF8, 0, input, -1, NULL, 0, NULL, NULL);
			std::string output(outputLength, 0);
			WideCharToMultiByte(CP_UTF8, 0, input, -1, &output[0], outputLength, NULL, NULL);
			return output;
		}
		std::string emptyString = "";
		return emptyString;
	}
}

