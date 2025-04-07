#pragma once

#include "netMessage.hpp"

namespace rage
{
	struct netMessageText : public netMessage
	{
		char text[256];

		void setText(const char* text) noexcept
		{
			strncpy(this->text, text, 256);
			this->text[255] = 0;
		}
	};
	static_assert(sizeof(netMessageText) == 256);
}
