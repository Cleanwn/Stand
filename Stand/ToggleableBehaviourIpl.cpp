#include "ToggleableBehaviourIpl.hpp"

#include "FiberPool.hpp"
#include "force_door_open.hpp"
#include "is_session.hpp"
#include "natives.hpp"
#include "regular_event.hpp"
#include "ScriptGlobal.hpp"

namespace Stand
{
	void IplData::enable() const
	{
	}

	void IplData::tick() const
	{
		for (const auto& ipl : request)
		{
			if (!STREAMING::IS_IPL_ACTIVE(ipl))
			{
				STREAMING::REQUEST_IPL(ipl);
			}
		}
		for (const auto& ipl : remove)
		{
			if (STREAMING::IS_IPL_ACTIVE(ipl))
			{
				STREAMING::REMOVE_IPL(ipl);
			}
		}
		for (const auto& ipl : remove_enable_only)
		{
			if (STREAMING::IS_IPL_ACTIVE(ipl))
			{
				STREAMING::REMOVE_IPL(ipl);
			}
		}
		for (const auto& door : doors)
		{
			force_door_open(door.door_hash, door.model_hash, door.x, door.y, door.z);
		}
	}

	void IplData::disable() const
	{
		FiberPool::queueJob([remove{ this->remove }, request{ this->request }] // REMOVE_IPL needs to yield (or something like that)
		{
			for (const auto& ipl : remove)
			{
				STREAMING::REQUEST_IPL(ipl);
			}
			for (const auto& ipl : request)
			{
				STREAMING::REMOVE_IPL(ipl);
			}
		});
	}

	void ToggleableBehaviourIpl::onEnable()
	{
		FiberPool::queueJob([this]
		{
			data.enable();
			reScriptTickEvent::registerHandlerInThreadContext([this]
			{
				if (isEnabled())
				{
					data.tick();
					return true;
				}
				data.disable();
				return false;
			}, "toggleable behaviour");
		});
	}
}
