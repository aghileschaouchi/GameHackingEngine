#pragma once

#include <iostream>
#include <tchar.h>
#include <string>
#include <Windows.h>
#include <stringapiset.h>
#include <sstream>

namespace helper
{
	template<typename A>
	void debugMessageBox(const std::string& messsage, A address)
	{
		std::stringstream stream;
		stream << std::hex << address;
		std::string _message(messsage + ": 0x" + stream.str());
		int msgboxID = MessageBoxA(
			NULL,
			reinterpret_cast<LPCSTR>(_message.c_str()),
			reinterpret_cast <LPCSTR>(L""),
			MB_ICONWARNING
		);
	}

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
}


