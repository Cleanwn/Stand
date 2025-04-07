#pragma once

#include <unordered_set>

namespace Stand
{
	template <class E>
	struct evtEvent
	{
		using handler_t = void(*)(E&);

		static inline std::unordered_set<handler_t> handlers;

		static void trigger(E& e)
		{
			for (const auto& handler : handlers)
			{
				handler(e);
			}
		}

		static void trigger(E&& e)
		{
			return trigger(e);
		}

		static void registerHandler(handler_t handler)
		{
			handlers.insert(handler);
		}

		static void unregisterHandler(handler_t handler)
		{
			handlers.erase(handler);
		}
	};
}
