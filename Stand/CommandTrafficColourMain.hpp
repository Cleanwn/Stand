#pragma once

#include "CommandToggle.hpp"

#include <soup/macros.hpp>

#include "AbstractEntity.hpp"
#include "Colour.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandTrafficColourMain : public CommandToggle
	{
	public:
		Colour colour{255, 0, 255};

		explicit CommandTrafficColourMain(CommandList* const parent)
			: CommandToggle(parent, LOC("TRFCLR"), CMDNAMES("colourtraffic", "colortraffic"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				HANDLER_CHECK(m_on)
				AbstractEntity::getAllVehicles([this](AbstractEntity&& ent)
				{
					if (!ent.isAPlayerInAnySeat()
						&& !isAlreadyColoured(ent)
						&& ent.getPos().distance(g_player_ped.getPos()) < 100.0f // We may be aware of other player's vehicles even when they are way too far away.
						&& !DECORATOR::DECOR_EXIST_ON(ent, "Player_Vehicle") // If we are trying to gift a vehicle, we don't want to take away control.
						)
					{
						if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(ent))
						{
							VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(ent, colour.r, colour.g, colour.b);
							VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(ent, colour.r, colour.g, colour.b);
						}
						else
						{
							NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(ent);
						}
					}
					CONSUMER_CONTINUE
				});
				HANDLER_END
			});
		}

	private:
		[[nodiscard]] bool isAlreadyColoured(AbstractEntity& ent) const
		{
			int primary_r = colour.r;
			int primary_g = colour.g;
			int primary_b = colour.b;

			int secondary_r = colour.r;
			int secondary_g = colour.g;
			int secondary_b = colour.b;

			VEHICLE::GET_VEHICLE_CUSTOM_PRIMARY_COLOUR(ent, &primary_r, &primary_g, &primary_b);
			VEHICLE::GET_VEHICLE_CUSTOM_SECONDARY_COLOUR(ent, &secondary_r, &secondary_g, &secondary_b);

			return (primary_r == colour.r && primary_g == colour.g && primary_b == colour.b) && (secondary_r == colour.r && secondary_g == colour.g && secondary_b == colour.b);
		}
	};
}
