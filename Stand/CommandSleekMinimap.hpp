#pragma once

#include "CommandToggle.hpp"

#include "evtForceQuitting.hpp"
#include "FiberPool.hpp"
#include "natives.hpp"
#include "Script.hpp"

namespace Stand
{
	class CommandSleekMinimap : public CommandToggle
	{
	private:
		inline static CommandSleekMinimap* instance = nullptr;
		int scaleform = 0;
		bool loaded = false;

	public:
		explicit CommandSleekMinimap(CommandList* const parent)
			: CommandToggle(parent, LOC("SLEEK"), { CMDNAME("sleekminimap") })
		{
			instance = this;
		}

		~CommandSleekMinimap()
		{
			if (instance == this)
			{
				instance = nullptr;
			}
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				HANDLER_CHECK(m_on)
				if (scaleform == 0)
				{
					scaleform = GRAPHICS::REQUEST_SCALEFORM_MOVIE("minimap");
				}
				if (!loaded)
				{
					if (GRAPHICS::HAS_SCALEFORM_MOVIE_FILENAME_LOADED("minimap"))
					{
						loaded = true;
						HUD::SET_BIGMAP_ACTIVE(true, true);
						FiberPool::queueJob([]
						{
							HUD::SET_BIGMAP_ACTIVE(false, true);
						});
					}
				}
				if (loaded)
				{
					GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(scaleform, "SETUP_HEALTH_ARMOUR");
					GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(3);
					GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
				}
				HANDLER_END
			});
			evtForceQuitting::registerHandler(&onForceQuit);
		}

		void onDisable(Click& click) final
		{
			ensureYieldableScriptThread(click, [this]
			{
				HUD::SET_BIGMAP_ACTIVE(true, true);
				Script::current()->yield();
				HUD::SET_BIGMAP_ACTIVE(false, true);
			});
			evtForceQuitting::unregisterHandler(&onForceQuit);
		}
		
	private:
		static void onForceQuit(evtForceQuitting&)
		{
			instance->scaleform = 0;
			instance->loaded = false;
		}
	};
}
