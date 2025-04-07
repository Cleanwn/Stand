#pragma once

#include "CommandToggle.hpp"

#include "DrawUtil3d.hpp"

namespace Stand
{
	class CommandWaypointPortal : public CommandToggle
	{
	private:
		Vector3 anchorPos{};
		float anchorHeading{};

	public:
		explicit CommandWaypointPortal(CommandList* const parent)
			: CommandToggle(parent, LOC("WPORTAL"), { CMDNAME("wpportals"), CMDNAME("wportals"), CMDNAME("waypointportals") })
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				if (!m_on)
				{
					return false;
				}
				if (!g_gui.waypoint.isNull())
				{
					// Ensure Anchor
					{
						v3 offsetPos = g_player_ent.getPos();
						if (anchorPos.distance(offsetPos) > 60.0f)
						{
							offsetPos += (ENTITY::GET_ENTITY_FORWARD_VECTOR(g_player_ent) * 50.0f);
							PATHFIND::GET_CLOSEST_VEHICLE_NODE_WITH_HEADING(offsetPos.x, offsetPos.y, offsetPos.z, &anchorPos, &anchorHeading, 1, 1.0f, 0);
							anchorPos.z += 5.0f;
							anchorHeading += 90.0f;
						}
					}
					// Draw Anchor
					DrawUtil3d::draw_portal(anchorPos, anchorHeading);
					// Teleport If In Portal
					if (g_player_ent.getPos().distance(anchorPos) < 10.0f)
					{
						auto playerPosRelativeToAnchor = (anchorPos - g_player_ent.getPos()).abs();
						auto scaler = v3(0.0f, 0.f, anchorHeading - 90.0f).toDirNoZ().abs();
						if ((playerPosRelativeToAnchor * scaler).sum() < 4.5f)
						{
							float speed = ENTITY::GET_ENTITY_SPEED(g_player_ent);
							TpUtil::teleportWithRedirects(g_gui.waypoint, [speed]()
							{
								if (g_player_ent.isVehicle())
								{
									VEHICLE::SET_VEHICLE_FORWARD_SPEED(g_player_ent, speed);
								}
							});
						}
					}
				}
				return true;
			});
		}
	};
}
