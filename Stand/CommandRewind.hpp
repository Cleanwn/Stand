#pragma once

#include <deque>

#include "Camgr.hpp"
#include "CommandToggle.hpp"
#include "get_current_time_millis.hpp"
#include "gta_input.hpp"
#include "input.hpp"
#include "PositionStateWithCam.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandRewind : public CommandToggle
	{
	public:
		inline static std::vector<Hotkey> default_hotkeys{
			VK_OEM_102, // for ISO keyboards: the key to the right of LSHFIT
			VK_OEM_2    // for ANSI keyboards: the key to the left of RSHIFT
		};

		std::deque<PositionStateWithCam> ticks = {};
		bool playback_mode = false;
		time_t playback_since = 0;
		uint8_t playback_speed;

		explicit CommandRewind(CommandList* const parent)
			: CommandToggle(parent, LOC("RWND"), { CMDNAME("rewind") }, LOC("RWND_H2"), false, CMDFLAGS_TOGGLE | CMDFLAG_TUTORIAL_CTX_POINT, COMMANDPERM_USERONLY, default_hotkeys)
		{
		}

		[[nodiscard]] const std::vector<Hotkey>& getDefaultHotkeys() const noexcept final
		{
			return default_hotkeys;
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				if (m_on)
				{
					if (Input::isAnyKeyPressed(hotkeys, INPUT_SPECIAL_ABILITY))
					{
						if (ticks.empty())
						{
							ensurePlaybackStopped();
						}
						else
						{
							playback_mode = true;
							if (playback_since == 0)
							{
								playback_since = get_current_time_millis();
								playback_speed = 1;
							}
							else if (GET_MILLIS_SINCE(playback_since) > 10000)
							{
								playback_speed = 3;
							}
							else if (GET_MILLIS_SINCE(playback_since) > 5000)
							{
								playback_speed = 2;
							}
							ticks.begin()->restoreThisTick();
							if ((size_t)playback_speed > ticks.size())
							{
								playback_speed = (uint8_t)ticks.size();
							}
							for (uint8_t i = 0; i != playback_speed; ++i)
							{
								ticks.pop_front();
							}
						}
					}
					else
					{
						ensurePlaybackStopped();
						while (ticks.size() > 20000)
						{
							ticks.pop_back();
						}
						ticks.emplace_front(PositionStateWithCam::save());
					}
					return true;
				}
				ensurePlaybackStopped();
				return false;
			});
		}

	private:
		void ensurePlaybackStopped()
		{
			if (playback_mode)
			{
				playback_mode = false;
				playback_since = 0;
				Camgr::unlock_pos();
				Camgr::unlock_rot();
				CAM::SET_THIRD_PERSON_CAM_RELATIVE_PITCH_LIMITS_THIS_UPDATE(0.0f, 0.0f);
				CAM::SET_THIRD_PERSON_CAM_ORBIT_DISTANCE_LIMITS_THIS_UPDATE(0.0f, 0.0f);
				HUD::UNLOCK_MINIMAP_ANGLE();
			}
		}
	};
#pragma pack(pop)
}
