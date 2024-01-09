#pragma once

#include <cstring> 
#include <cstdio>
#include <cstdlib>
#include <tchar.h>
#include <string>
#include <Windows.h>
#include <stringapiset.h>

namespace helper
{
	TCHAR* convert(const std::string& string)
	{
		wchar_t* result = NULL;
		if (string.size() != 0)
		{
			int length = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), string.length(), NULL, 0);
			result = new wchar_t[length];
			MultiByteToWideChar(CP_UTF8, 0, string.c_str(), string.length(), result, length);
		}
		return result; //should be deleted outisde!!!
	}

	std::unique_ptr<wchar_t[]> smartConvert(const std::string& string) //to be fixed
	{
		if (!string.empty())
		{
			wchar_t* input = convert(string);
			size_t length = wcslen(input);
			std::unique_ptr<wchar_t[]> result(new wchar_t[length]);
			wcscpy(result.get(), input);
			delete[] input;
			return result;
		}
		return nullptr;
	}
}

