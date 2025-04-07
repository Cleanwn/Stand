#include "DisableDripfeed.hpp"

#include <soup/macros.hpp>

#include "atStringHash.hpp"
#include "script_func.hpp"

namespace Stand
{
	void DisableDripfeed::onTick()
	{
		if (enabled)
		{
			patch.onTick(ATSTRINGHASH("appinternet"), FUNC_AI_CANPRCHS, ScriptFuncPatch::patchBytesReturnTrue_2, COUNT(ScriptFuncPatch::patchBytesReturnTrue_2));
		}
	}

	void DisableDripfeed::onDisable()
	{
		patch.onDisable(ATSTRINGHASH("appinternet"));
	}
}
