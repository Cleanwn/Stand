#include "Script.hpp"

#include "Exceptional.hpp"
#include "ExecCtx.hpp"
#include "get_current_time_millis.hpp"
#include "Hooking.hpp"
#include "ScriptMgr.hpp"
#include "Util.hpp"

#if REPORT_YIELD_WITH_LOCK
#include "Gui.hpp"
#endif

namespace Stand
{
	Script::Script(script_func_t func)
		: func(func)
	{
		fiber = CreateFiber(0, [](void* param)
		{
			static_cast<Script*>(param)->fiberFunc();
		}, this);
	}

	Script::~Script()
	{
		if (fiber != nullptr)
		{
			DeleteFiber(fiber);
		}
	}

	Script* Script::current()
	{
		return static_cast<Script*>(GetFiberData());
	}

	HANDLE Script::getModule() const
	{
		return g_script_mgr.getScriptModule(this);
	}

	bool Script::isCurrent() const noexcept
	{
		return GetCurrentFiber() == fiber;
	}

	void Script::stop()
	{
		if (fiber != nullptr)
		{
			DeleteFiber(fiber);
			fiber = nullptr;
		}
	}

	bool Script::tick()
	{
		if (fiber == nullptr)
		{
			return false;
		}
		ExecCtx::get().tc = TC_SCRIPT_YIELDABLE;
		SwitchToFiber(fiber);
		return func != nullptr;
	}

	void Script::nestedTick()
	{
		SOUP_ASSERT(ExecCtx::get().isScript());
		SOUP_ASSERT(!isCurrent());
		const auto prev_ret_fiber = ret_fiber;
		ret_fiber = GetCurrentFiber();
#if REPORT_YIELD_WITH_LOCK
		nested = true;
#endif
		tick();
#if REPORT_YIELD_WITH_LOCK
		nested = false;
#endif
		ret_fiber = prev_ret_fiber;
	}

	void Script::yield()
	{
#if REPORT_YIELD_IN_NOYIELD
		if (ExecCtx::get().tc != TC_SCRIPT_YIELDABLE)
		{
			Exceptional::report("Yielding not allowed in this context");
		}
#endif
#if REPORT_YIELD_WITH_LOCK
		if (!nested)
		{
			if (g_gui.root_mtx.isWriteLockedByThisThread())
			{
				Exceptional::report("Yielding while having root write lock");
			}
			else if (g_gui.root_mtx.isReadLockedByThisThread())
			{
				Exceptional::report("Yielding while having root read lock");
			}
		}
#endif
		SwitchToFiber(ret_fiber);
	}

	void Script::yield(time_t minSleepMs)
	{
		const auto deadline = get_current_time_millis() + minSleepMs;
		do
		{
			yield();
		} while (!IS_DEADLINE_REACHED(deadline));
	}

	void Script::fiberFunc()
	{
		__try
		{
			func();
		}
		__EXCEPTIONAL()
		{
		}
		func = nullptr;
#if REPORT_YIELD_IN_NOYIELD
		ExecCtx::get().tc = TC_SCRIPT_YIELDABLE;
#endif
		do
		{
			yield();
		} while (true);
	}
}
