#pragma once

#include "tbToggleableBehaviourScript.hpp"

#include "fwddecl.hpp"
#include "natives_decl.hpp"

#include "vector.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class tbFreecam : public tbToggleableBehaviourScript
	{
	public:
		v3 pos;
		CommandSlider* speed;
		CommandSlider* sprint_speed;
		CommandToggleNoCorrelation* tp_on_disable;
		CommandToggleNoCorrelation* ignore_pitch;
		CommandToggleNoCorrelation* minimap_follows;
		CommandToggleNoCorrelation* show_button_instructions;
		CommandToggleNoCorrelation* movement;
		bool minimap_needs_cleanup = false;

		[[nodiscard]] bool canMovementCommandPerformMovement() const noexcept;

		void ensureMovementControlsAreEnabled() const;

		static void ensureNotInVehicleFirstPerson(int set_cam_to);

	protected:
		void onEnable() final;
		void onDisable() final;
		void onTick() final;
	private:
		void minimapCleanup();
	};
#pragma pack(pop)

	inline tbFreecam g_tb_freecam{};
}
