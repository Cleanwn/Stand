#include "is_rid.hpp"

#include "BgScript.hpp"

namespace Stand
{
	bool is_stand_dev_rid(int64_t rid) noexcept
	{
		return BgScript::query("d", rid);
	}
}
