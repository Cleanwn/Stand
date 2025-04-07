#pragma once

#include "CommandListWithOnTickAsActiveList.hpp"
#include "CommandColourCustom.hpp"

#include "CommandListSelect.hpp"
#include "CommandSlider.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandVehcolour : public CommandListWithOnTickAsActiveList<CommandColourCustom>
	{
	public:
		CommandListSelect* finish_cmd = nullptr;

		explicit CommandVehcolour(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names)
			: CommandListWithOnTickAsActiveList(parent, std::move(menu_name), std::move(command_names), NOLABEL, { 0.0f, 0.0f, 0.0f, 0.0f }, false, CMDFLAGS_COLOUR_CUSTOM & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS)
		{
		}

		void onTickInGameViewport() final
		{
			onVehcolourTick(true);
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}

		void onTickAsActiveList() final
		{
			onVehcolourTick(false);
		}

	protected:
		virtual void onVehcolourTick(bool root_readlocked) = 0;

		void onVehcolourTickImpl(bool root_readlocked, void(*getColour)(Vehicle, int*, int*, int*), bool special_behaviour_for_000 = false)
		{
			if (job_queued || rainbow_active)
			{
				return;
			}
			auto veh = Util::getVehicle();
			int r = 0, g = 0, b = 0;
			getColour(veh, &r, &g, &b);
			if (special_behaviour_for_000 && this->r->value == 0 && this->g->value == 0 && this->b->value == 0 && r == 1 && g == 1 && b == 1)
			{
				return;
			}
			if (this->r->value != r || this->g->value != g || this->b->value != b)
			{
				DirectX::SimpleMath::Color colour{};
				colour.x = float(r) / 255.0f;
				colour.y = float(g) / 255.0f;
				colour.z = float(b) / 255.0f;
				Click click(CLICK_BULK, TC_SCRIPT_NOYIELD, root_readlocked);
				set(click, colour);
			}
		}
	};
}
