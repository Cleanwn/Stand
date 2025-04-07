#pragma once

#include "fwddecl.hpp"

#include "tbToggleableBehaviourScript.hpp"

namespace Stand
{
	class tbCustomFov : public tbToggleableBehaviourScript
	{
	public:
		CommandSlider* tp_in_vehicle;
		CommandSlider* tp_on_foot;
		CommandSlider* fp_in_vehicle;
		CommandSlider* fp_on_foot;
		CommandSlider* aiming;
		CommandSlider* sniping;

	private:
		bool had_custom_fov = false;

	protected:
		void onTick() final;
		void onDisable() final;
	};

	inline tbCustomFov g_tb_custom_fov{};
}
