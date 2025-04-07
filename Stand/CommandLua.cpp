#include "CommandLua.hpp"

#include "FiberPool.hpp"
#include "Gui.hpp"

namespace Stand
{
	void CommandLuaData::queueJob(Command* cmd, ClickType click_type, std::function<void(lua_State*, bool)>&& func)
	{
		SOUP_IF_LIKELY (const auto L = script->L)
		{
			SOUP_IF_UNLIKELY (!script->running
				|| g_gui.current_bulk_state_operation == BSO_DEFAULT
				|| (g_gui.current_bulk_state_operation == BSO_LOAD && !script->isEnabledInActiveProfile())
				)
			{
				// Script is shutting down, have to invoke directly and now.
				func(L, true);
			}
			else
			{
				if (script->invoke_no_fiber)
				{
					// We have the signal that we can touch the Lua runtime in this thread context,
					// still spinning up a coroutine for the callback, tho.
					func(L, false);
				}
				else
				{
					// Fiber job is needed so lua scripts can trigger their own commands
					FiberPool::queueJob([cmd_wr{ cmd->getWeakRef() }, running_wr{ script->getRunningWeakref() }, func{ std::move(func) }]
					{
						if (cmd_wr.isValid())
						{
							auto script = running_wr.getPointer();
							SOUP_ASSERT(script, "Can't invoke Lua callback because script is no longer running");
							func(script->L, false);
						}
					});
				}
			}
		}
	}
}
