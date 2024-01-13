#pragma once

#include <tchar.h>
#include <string>
#include <Windows.h>
#include <stringapiset.h>

namespace helper
{
	std::wstring s2ws(const std::string& string, bool isUTF8 = true)
	{
		int wslength;
		int slength = string.length() + 1;
		wslength = MultiByteToWideChar(isUTF8 ? CP_UTF8 : CP_ACP, 0, string.c_str(), slength, 0, 0);

		std::wstring result;
		result.resize(wslength);
		MultiByteToWideChar(isUTF8 ? CP_UTF8 : CP_ACP, 0, string.c_str(), slength, const_cast<wchar_t*>(result.c_str()), wslength);
		return result;
	}

	LPCWSTR constConvert(const std::string& string)
	{
		LPCWSTR result = L"";
		if (string.size() != 0)
		{
			std::wstring wrapper = s2ws(string);
			result = wrapper.c_str();
		}
		return result;
	}

	TCHAR* convert(const std::string& string, bool isUTF8 = true)
	{
		wchar_t* result = NULL;
		if (string.size() != 0)
		{
			int length = MultiByteToWideChar((isUTF8) ? CP_UTF8 : CP_ACP, 0, string.c_str(), string.length(), NULL, 0);
			result = new wchar_t[length];
			MultiByteToWideChar(CP_UTF8, 0, string.c_str(), string.length(), result, length);
		}
		return result; //should be deleted outisde!!!
	}

	namespace functors
	{
		//struct stringToTcharP
		//{

		//	stringToTcharP()
		//};
	}
}


