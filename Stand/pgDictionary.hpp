#pragma once

#include "pgBase.hpp"

namespace rage
{
	struct pgDictionaryBase : public pgBase
	{
	};

	template <typename T>
	struct pgDictionary : public pgDictionaryBase
	{
	};
}
