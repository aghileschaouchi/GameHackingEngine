#pragma once

#include "Address.h"

namespace ghe
{
	template <typename P, typename A>
	class WinMemoryManager //this class is not useful for the moment, methods has been moved to WinGame
	{
	public:

		// SHOULD BE ISOLATED WITH A DESIGN PATTERN
		//inline ghe::Address getDynamicAddress(unsigned int pointerLevel, A* offsets, A staticAddress, A staticOffset) {
		//	return ghe::Address(true, FindTheAddr(m_hProcess, pointerLevel, offsets, m_baseAddress, staticAddress, staticOffset));
		//}
	};
}