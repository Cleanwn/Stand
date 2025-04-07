#pragma once

#include "CommandListWithOnTickAsActiveList.hpp"
#include "CommandStandardGameVehicleColour.hpp"

#include "AbstractModel.hpp"
#include "CommandCustomsShowNa.hpp"

namespace Stand
{
	class CommandInteriorColour : public CommandListWithOnTickAsActiveList<CommandStandardGameVehicleColour>
	{
	public:
		using CommandListWithOnTickAsActiveList<CommandStandardGameVehicleColour>::CommandListWithOnTickAsActiveList;

		void onTickAsActiveList() final
		{
			if (parent->as<CommandListConcealer>()->isVisible(this) != getShouldBeVisible())
			{
				g_gui.inputBack(TC_OTHER, false);
			}
		}

		void onTickInGameViewport() final
		{
			parent->as<CommandListConcealer>()->setVisible(this, getShouldBeVisible());
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}

	private:
		[[nodiscard]] bool getShouldBeVisible() const
		{
			return CommandCustomsShowNa::instance->m_on || g_player_veh.getModel().supportsInteriorColours();
		}
	};
}