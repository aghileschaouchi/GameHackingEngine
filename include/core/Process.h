#pragma once

#include "Program.h"

//use printf for the moment, should be replaced by Boost.Log
#include <cstdio>

namespace ghe
{
	template<typename T, typename S, typename A>
	class Process : public Program<T, S, A>
	{
	public:
		using Program<T, S, A>::Program;
		//copy
		Process(std::unique_ptr<ghe::Address<A>>& baseAddress, T& pid, S& programName) = delete;
		Process(ghe::Address<A>& baseAddressContent, T& pid, S& programName) = delete;

		//move
		Process(std::unique_ptr<ghe::Address<A>>&& baseAddress, T&& pid, S&& programName) :
			Program<T, S, A>(std::move(baseAddress), std::forward<T>(pid), std::forward<S>(programName)) {}

		Process(ghe::Address<A>&& baseAddressContent, T&& pid, S&& programName) :
			Program<T, S, A>(std::move(baseAddressContent), std::forward<T>(pid), std::forward<S>(programName)) {}

	};
} //end namespace ghe

