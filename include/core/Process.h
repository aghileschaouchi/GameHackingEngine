#pragma once

#include "Program.h"

namespace ghe
{
	template<typename T, typename A>
	class Process : public Program<T, A>
	{
	public:
		using Program<T, A>::Program;
		//copy
		Process(std::unique_ptr<ghe::Address<A>>& baseAddress, T& pid, std::string& programName) = delete;
		Process(ghe::Address<A>& baseAddressContent, T& pid, std::string& programName) = delete;

		//move
		Process(std::unique_ptr<ghe::Address<A>>&& baseAddress, T&& pid, std::string&& programName) :
			Program<T, A>(std::move(baseAddress), std::forward<T>(pid), std::move(programName)) {}

		Process(ghe::Address<A>&& baseAddressContent, T&& pid, std::string&& programName) :
			Program<T, A>(std::move(baseAddressContent), std::forward<T>(pid), std::move(programName)) {}

	};
} //end namespace ghe

