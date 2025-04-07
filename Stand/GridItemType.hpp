#pragma once

#include <cstdint>

namespace Stand
{
	enum GridItemType : uint8_t
	{
		GRIDITEM_INDIFFERENT,
		GRIDITEM_PRIMARYTEXT,
		GRIDITEM_LIST,
		GRIDITEM_SCROLLBAR,
		GRIDITEM_TABS,
	};
}
