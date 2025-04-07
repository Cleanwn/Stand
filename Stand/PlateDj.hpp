#pragma once

#include "tbToggleableBehaviourScript.hpp"

#include "AbstractEntity.hpp"

namespace Stand
{
	class PlateDj : public tbToggleableBehaviourScript
	{
	public:
		enum Track : uint8_t
		{
			SPEEDOMETER,
			TPS,

			NUM_TRACKS,
		};

		bool track_enabled[NUM_TRACKS] = { false };

		void enableTrack(uint8_t track) noexcept;
		void disableTrack(uint8_t track) noexcept;

	protected:
		void onEnable() final;
		void onDisable() final;
		void onTick() final;

	private:
		AbstractEntity last_modified_veh;
		uint8_t current_track;
		time_t last_switch;

		void switchToNextTrack() noexcept;
		void switchToTrack(uint8_t track) noexcept;
	};
	inline PlateDj g_plate_dj{};
}
