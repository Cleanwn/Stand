#include "GunPunishments.hpp"

#include "AbstractEntity.hpp"
#include "AbstractPlayer.hpp"
#include "gta_input.hpp"
#include "input.hpp"
#include "natives.hpp"
#include "punishments.hpp"

namespace Stand
{
	void GunPunishments::onTick()
	{
		if (aim_punishments || shoot_punishments)
		{
			Vector3 coords;
			bool shot = WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(g_player_ped, &coords);
			Entity ent;
			if (PLAYER::GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(g_player, &ent))
			{
				punishment_t punishments = aim_punishments;
				if (shot)
				{
					punishments |= shoot_punishments;
				}
				auto aent = AbstractEntity::get(ent);
				Punishments::apply(punishments, aent, shot ? PunishmentTrigger::SHOOTING_AT : PunishmentTrigger::AIMING_AT);
			}
			else if (Input::isPressingAim())
			{
				punishment_t punishments = aim_punishments;
				if (shot)
				{
					punishments |= shoot_punishments;
				}
				else
				{
					auto cam_pos = CAM::GET_FINAL_RENDERED_CAM_COORD();
					auto target_pos = cam_pos + (CAM::GET_FINAL_RENDERED_CAM_ROT(2).toDir() * 1000.0f);
					const int shapetest_handle = SHAPETEST::START_EXPENSIVE_SYNCHRONOUS_SHAPE_TEST_LOS_PROBE(cam_pos.x, cam_pos.y, cam_pos.z, target_pos.x, target_pos.y, target_pos.z, -1, g_player_ped, 0);
					BOOL hit;
					Vector3 surfaceNormal;
					SHAPETEST::GET_SHAPE_TEST_RESULT(shapetest_handle, &hit, &coords, &surfaceNormal, &ent);
					if (!hit)
					{
						return;
					}
				}
				auto aent = AbstractEntity::get(ent);
				Punishments::apply(punishments, coords, aent, PunishmentTrigger::AIMING_AT);
			}
			else if (shot)
			{
				AbstractEntity aent{};
				Punishments::apply(shoot_punishments, coords, aent, PunishmentTrigger::SHOOTING_AT);
			}
		}
	}
}
