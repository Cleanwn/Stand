#pragma once

#include <functional>
#include <unordered_set>
#include <utility>
#include <vector>

#include <fmt/core.h>

#include "Exceptional.hpp"
#include "FileLogger.hpp"
#include "Spinlock.hpp"

namespace Stand
{
	template <class E>
	struct reRegularEvent
	{
		static inline Spinlock mtx;
		static inline std::unordered_set<void(*)()> stateless_handlers;
		static inline std::vector<std::pair<std::function<bool()>, std::string>> handlers;

		static void trigger()
		{
			EXCEPTIONAL_LOCK(mtx)
			for (auto i = stateless_handlers.begin(); i != stateless_handlers.end(); )
			{
				__try
				{
					(*i)();
					++i;
					continue;
				}
				__EXCEPTIONAL()
				{
					g_logger.log("Unregistering scheduled handler due to exception");
				}
				i = stateless_handlers.erase(i);
			}
			for (auto i = handlers.begin(); i != handlers.end(); )
			{
				__try
				{
					if ((i->first)())
					{
						i++;
						continue;
					}
				}
				__EXCEPTIONAL()
				{
					g_logger.log(fmt::format("Unregistering scheduled handler for {} due to exception", std::move(i->second)));
				}
				i = handlers.erase(i);
			}
			EXCEPTIONAL_UNLOCK(mtx)
		}

		static void registerHandler(void handler())
		{
			EXCEPTIONAL_LOCK(mtx)
			stateless_handlers.insert(handler);
			EXCEPTIONAL_UNLOCK(mtx)
		}

		static void registerHandler(std::function<bool()>&& handler, std::string&& name)
		{
			EXCEPTIONAL_LOCK(E::mtx)
			E::handlers.emplace_back(std::move(handler), std::move(name));
			EXCEPTIONAL_UNLOCK(E::mtx)
		}

		static void registerHandlerInThreadContext(std::function<bool()>&& handler, std::string&& name)
		{
			if (handler())
			{
				EXCEPTIONAL_LOCK(E::mtx)
				E::handlers.emplace_back(std::move(handler), std::move(name));
				EXCEPTIONAL_UNLOCK(E::mtx)
			}
		}

		static void unregisterHandler(void handler())
		{
			EXCEPTIONAL_LOCK(mtx)
			stateless_handlers.erase(handler);
			EXCEPTIONAL_UNLOCK(mtx)
		}

		[[nodiscard]] static size_t getNumHandlers()
		{
			return stateless_handlers.size() + handlers.size();
		}
	};

	struct reScriptTickEvent : public reRegularEvent<reScriptTickEvent> { };

	struct rePostTickEvent : public reRegularEvent<rePostTickEvent> { };

	struct rePresentEvent : public reRegularEvent<rePresentEvent> { };
}
