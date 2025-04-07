#include "PlateDj.hpp"

#include "AbstractEntity.hpp"
#include "get_current_time_millis.hpp"
#include "Metrics.hpp"
#include "natives.hpp"
#include "Util.hpp"

namespace Stand
{
	void PlateDj::enableTrack(uint8_t track) noexcept
	{
		enable();
		track_enabled[track] = true;
		switchToTrack(track);
	}

	void PlateDj::disableTrack(uint8_t track) noexcept
	{
		disable();
		track_enabled[track] = false;
		if (current_track == track)
		{
			switchToNextTrack();
		}
	}

	void PlateDj::onEnable()
	{
		tbToggleableBehaviourScript::onEnable();
		switchToNextTrack();
	}

	void PlateDj::onDisable()
	{
		tbToggleableBehaviourScript::onDisable();
		if (last_modified_veh.isValid())
		{
			last_modified_veh.setPlateBasedOnSeed();
			last_modified_veh.reset();
		}
	}

	void PlateDj::onTick()
	{
		if (last_modified_veh.isValid()
			&& last_modified_veh != g_player_veh
			)
		{
			last_modified_veh.setPlateBasedOnSeed();
			last_modified_veh.reset();
		}

		if (g_player_veh.isValid()
			&& g_player_veh.isOwnerOfVehicleAndDriver()
			)
		{
			if (GET_MILLIS_SINCE(last_switch) > 3000)
			{
				switchToNextTrack();
			}
			std::string text;
			switch (current_track)
			{
			case SPEEDOMETER:
				text = Util::mps_to_unit_of_choice(ENTITY::GET_ENTITY_SPEED(g_player_veh));
				break;

			case TPS:
				text = fmt::to_string(Metrics::ticks_per_second);
				text.append(" TPS");
				break;
			}
			g_player_veh.setPlateText(text);
			last_modified_veh = g_player_veh;
		}
	}

	void PlateDj::switchToNextTrack() noexcept
	{
		for (uint8_t its = 0; its != NUM_TRACKS; ++its)
		{
			current_track += 1;
			current_track %= NUM_TRACKS;
			if (track_enabled[current_track])
			{
				break;
			}
		}
		last_switch = get_current_time_millis();
	}

	void PlateDj::switchToTrack(uint8_t track) noexcept
	{
		current_track = track;
		last_switch = get_current_time_millis();
	}
}
