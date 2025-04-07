#include "FiberPool.hpp"

#include <soup/AtomicDeque.hpp>

#include "main.hpp" // g_hmodule
#include "Script.hpp"
#include "ScriptMgr.hpp"

namespace Stand
{
	// Not using AtomicStack because there might be some expectation that multiple FiberPool::queueJob will have the jobs spinned up in that exact order.
	static soup::AtomicDeque<std::function<void()>> q;

	void FiberPool::queueJob(std::function<void()>&& func)
	{
		q.emplace_front(std::move(func));
	}

	void FiberPool::onTick()
	{
		auto i = q.size();
		while (i--)
		{
			g_script_mgr.addScript(g_hmodule, std::make_unique<Script>([]
			{
				auto job = q.pop_back();
				(*job)();
			}));
		}
	}
}
