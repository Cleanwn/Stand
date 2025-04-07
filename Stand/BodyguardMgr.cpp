#include "BodyguardMgr.hpp"

#include "AbstractPlayer.hpp"
#include "AbstractModel.hpp"
#include "BlipUtil.hpp"
#include "CommandBodyguardMember.hpp"
#include "CommandDivider.hpp"
#include "CommandSlider.hpp"
#include "CommandToggle.hpp"
#include "CVehicleModelInfo.hpp"
#include "eCombatAbility.hpp"
#include "eCombatAttributes.hpp"
#include "eCombatRange.hpp"
#include "ePedConfigFlags.hpp"
#include "Exceptional.hpp"
#include "gta_vehicle_layout.hpp"
#include "Gui.hpp"
#include "is_session.hpp"
#include "natives.hpp"
#include "PedType.hpp"
#include "Script.hpp"
#include "Util.hpp"

namespace Stand
{
	static std::vector<AbstractEntity> bodyguards{};

	[[nodiscard]] static bool isDiscarded(AbstractEntity& ped)
	{
		const bool dead = ped.isDead();

		if (!ped.exists())
		{
			return true;
		}

		if (dead
			|| g_gui.isUnloadPending()
			|| is_session_transition_active(true)
			)
		{
			auto blip = HUD::GET_BLIP_FROM_ENTITY(ped);
			auto handle = ped.getHandle();

			BlipUtil::remove(blip);
			PED::REMOVE_PED_FROM_GROUP(ped);

			if (dead)
			{
				ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&handle);
			}
			else
			{
				ped.removeFromPlaneOfExistence(); // We're unloading or in a transition, despawn them.
			}

			return true;
		}

		return false;
	}

	void BodyguardMgr::associate(AbstractEntity& ped, Hash weapon, uint8_t group)
	{
		if (ped.getModel().isAnimal())
		{
			weapon = ATSTRINGHASH("weapon_animal");
			PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);
		}

		ped.giveWeapons({ weapon, (Hash)BodyguardMgr::secondary_weapon->value }, true);
		WEAPON::SET_CURRENT_PED_WEAPON(ped, weapon, TRUE);

		PED::SET_PED_HIGHLY_PERCEPTIVE(ped, true);
		PED::SET_PED_SEEING_RANGE(ped, 400.0f);
		PED::SET_PED_COMBAT_ABILITY(ped, eCombatAbility::CA_Professional);
		PED::SET_PED_COMBAT_RANGE(ped, eCombatRange::CR_VeryFar);
		PED::SET_PED_FIRING_PATTERN(ped, ATSTRINGHASH("FIRING_PATTERN_FULL_AUTO"));

		ped.setConfigFlag(CPED_CONFIG_FLAG_DisableExplosionReactions, true);
		ped.setConfigFlag(CPED_CONFIG_FLAG_NeverLeavesGroup, true);
		ped.setConfigFlag(CPED_CONFIG_FLAG_BlockWeaponSwitching, true);
		ped.setConfigFlag(CPED_CONFIG_FLAG_DisableEvasiveDives, true);
		ped.setCombatAttr(CA_ALWAYS_FIGHT, true);
		ped.setCombatAttr(CA_USE_VEHICLE, true);
		ped.setCombatAttr(CA_USE_COVER, false);
		ped.setCombatAttr(CA_CAN_FIGHT_ARMED_PEDS_WHEN_NOT_ARMED, true);
		ped.setCombatAttr(CA_WILL_DRAG_INJURED_PEDS_TO_SAFETY, true);
		ped.setCombatAttr(CA_DISABLE_ENTRY_REACTIONS, true);

		PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(ped, 1);
		PED::SET_RAGDOLL_BLOCKING_FLAGS(ped, 1);
		PED::SET_RAGDOLL_BLOCKING_FLAGS(ped, 4);
		PED::SET_PED_AS_GROUP_MEMBER(ped, group);
		PED::SET_PED_NEVER_LEAVES_GROUP(ped, true);
		PED::SET_GROUP_SEPARATION_RANGE(group, 9999.0f);
		PED::SET_GROUP_FORMATION(BodyguardMgr::getGroup(), (int)BodyguardMgr::formation->value);
		PED::SET_PED_ACCURACY(ped, 70);

		NETWORK::SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(NETWORK::PED_TO_NET(ped), true);
		NETWORK::SET_NETWORK_ID_ALWAYS_EXISTS_FOR_PLAYER(NETWORK::PED_TO_NET(ped), g_player, true);
		NETWORK::SET_NETWORK_ID_CAN_MIGRATE(NETWORK::PED_TO_NET(ped), false);
		ENTITY::SET_ENTITY_AS_MISSION_ENTITY(ped, false, true);
		ENTITY::SET_ENTITY_LOAD_COLLISION_FLAG(ped, true, 1);
		ped.setCanMigrate(false);

		auto blip = HUD::ADD_BLIP_FOR_ENTITY(ped);
		HUD::SET_BLIP_AS_FRIENDLY(blip, true);
		HUD::SET_BLIP_COLOUR(blip, 27);
		HUD::SET_BLIP_SCALE(blip, 0.5f);

		bodyguards.emplace_back(ped);
		EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
		if (!divider)
		{
			divider = BodyguardMgr::list->createChild<CommandDivider>(LOC("BDYGUARD"));
		}
		BodyguardMgr::list->createChild<CommandBodyguardMember>(ped);
		EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
	}

	int BodyguardMgr::getGroup()
	{
		return PLAYER::GET_PLAYER_GROUP(g_player);
	}

	Hash BodyguardMgr::getWeapon(AbstractEntity& bodyguard, bool primary)
	{
		for (const auto& c : BodyguardMgr::list->children)
		{
			if (const auto cmd = c->as<CommandBodyguardMember>(); cmd && cmd->entity == bodyguard)
			{
				return primary ? (Hash)cmd->primary->value : (Hash)cmd->secondary->value;
			}
		}

		return primary ? (Hash)primary_weapon->value : (Hash)secondary_weapon->value;
	}

	bool BodyguardMgr::isWeaponValid(Hash weapon, AbstractEntity& bodyguard, AbstractEntity& veh)
	{
		SOUP_IF_LIKELY (auto w = Weapon::find(weapon))
		{
			SOUP_IF_LIKELY (auto info = veh.getModel().getVehicleModelInfo())
			{
				SOUP_IF_LIKELY (auto seat = bodyguard.getSeat(veh); seat != -3)
				{
					return info->m_pVehicleLayoutInfo && info->m_pVehicleLayoutInfo->doesSeatAllowWeapon(*w, seat + 1);
				}
			}
		}

		return true;
	}

	Hash BodyguardMgr::getBestWeapon(Hash primary, Hash secondary, AbstractEntity& bodyguard)
	{
		auto veh = bodyguard.getVehicle();

		if (veh.isValid())
		{
			if (isWeaponValid(primary, bodyguard, veh))
			{
				return primary;
			}
			else
			{
				return isWeaponValid(secondary, bodyguard, veh) ? secondary : ATSTRINGHASH("WEAPON_UNARMED");
			}
		}
		else
		{
			return primary;
		}
	}

	void BodyguardMgr::push()
	{
		const auto weapon = (Hash)BodyguardMgr::primary_weapon->value;

		for (int i = 1; i <= BodyguardMgr::count->value; ++i)
		{
			if (bodyguards.size() == 7)
			{
				break;
			}
			
			auto ped = BodyguardMgr::model->createPed(g_player_ped.getPos(), PED_TYPE_ANIMAL, g_player_ped);
			BodyguardMgr::associate(ped, weapon, getGroup());
		}
	}

	void BodyguardMgr::clear()
	{
		bodyguards.clear();

		removeDivider();

		// Remove CommandBodyguardMember instances
		EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
		for (auto i = list->children.begin(); i != list->children.end(); )
		{
			if ((*i)->type == COMMAND_LIST_CUSTOM_SPECIAL_MEANING)
			{
				i = list->children.erase(i);
			}
			else
			{
				++i;
			}
		}
		list->processChildrenUpdate();
		EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
	}

	void BodyguardMgr::removeDivider()
	{
		if (divider)
		{
			divider->beGone();
			divider = nullptr;
		}
	}

	void BodyguardMgr::postDeleteMember()
	{
		bool any_bodyguards_remain = false;
		for (auto& cmd : list->children)
		{
			if (cmd->type == COMMAND_LIST_CUSTOM_SPECIAL_MEANING)
			{
				any_bodyguards_remain = true;
				break;
			}
		}
		if (!any_bodyguards_remain)
		{
			removeDivider();
		}
	}

	bool BodyguardMgr::isPedBodyguard(AbstractEntity& ped)
	{
		for (auto& e : bodyguards)
		{
			if (e == ped)
			{
				return true;
			}
		}

		return false;
	}

	void BodyguardMgr::summon(const bool extra_distance)
	{
		auto pos = g_player.getPos();

		if (extra_distance)
		{
			pos.x += 10.0f;
			pos.y += 10.0f;
		}

		for (auto& ped : bodyguards)
		{
			ped.requestControl(ATSTRINGHASH("call"), [pos](AbstractEntity& ent)
			{
				ent.setPos(pos);

				if (g_player_veh.isValid())
				{
					PED::SET_PED_INTO_VEHICLE(ent, g_player_veh, -2);
				}
			});
		}
	}

	void BodyguardMgr::onTick()
	{
		for (auto i = bodyguards.begin(); i != bodyguards.end(); )
		{
			auto& ped = *i;

			if (isDiscarded(ped))
			{
				i = bodyguards.erase(i);
				continue;
			}

			if (!NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(ped))
			{
				NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(ped);
			}

			if (BodyguardMgr::teleport_when_far->m_on)
			{
				auto player_pos = g_player.getPos();

				if (ped.getPos().distance(player_pos) > 20.0f)
				{
					BodyguardMgr::summon(true);
				}
			}

			if (BodyguardMgr::immortal->m_on != ped.isInvulnerable())
			{
				if (BodyguardMgr::immortal->m_on)
				{
					ped.godmodeEnable();
				}
				else
				{
					ped.godmodeDisable();
				}
			}

			const auto guardhash = (Hash)BodyguardMgr::relationship->value;

			const bool ignore_players = BodyguardMgr::ignore_players->m_on;
			const bool ignore_friends = BodyguardMgr::ignore_friends->m_on;
			const bool ignore_crew_members = BodyguardMgr::ignore_crew_members->m_on;
			const bool ignore_organisation_members = BodyguardMgr::ignore_crew_members->m_on;

			for (const auto& player : AbstractPlayer::listAll())
			{
				if (auto player_ped = player.getPed(); player_ped.isValid())
				{
					uint8_t desired_hash = 3;
					const auto playerhash = PED::GET_PED_RELATIONSHIP_GROUP_HASH(player_ped);

					if (ignore_players
						|| player.satisfiesAnyOf(ignore_friends, ignore_crew_members, false, ignore_organisation_members)
						)
					{
						desired_hash = 0;
					}

					if (desired_hash != PED::GET_RELATIONSHIP_BETWEEN_GROUPS(guardhash, playerhash))
					{
						if (desired_hash == 0)
						{
							PED::SET_RELATIONSHIP_BETWEEN_GROUPS(desired_hash, guardhash, playerhash);
						}
						else
						{
							PED::CLEAR_RELATIONSHIP_BETWEEN_GROUPS(3, guardhash, playerhash);
						}
					}
				}
			}

			if (BodyguardMgr::ignore_authorities->m_on)
			{
				PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, guardhash, ATSTRINGHASH("COP"));
			}
			else
			{
				PED::CLEAR_RELATIONSHIP_BETWEEN_GROUPS(4, guardhash, ATSTRINGHASH("COP"));
			}

			if (!ped.getModel().isAnimal())
			{
				Hash not_relevant;
				if (!WEAPON::GET_CURRENT_PED_VEHICLE_WEAPON(ped, &not_relevant))
				{
					if (const auto weap = getBestWeapon(getWeapon(ped, true), getWeapon(ped, false), ped); weap != ped.getSelectedWeapon())
					{
						WEAPON::SET_CURRENT_PED_WEAPON(ped, weap, TRUE);
					}
				}
			}

			if (!PED::IS_PED_GROUP_MEMBER(ped, getGroup()))
			{
				PED::SET_PED_AS_GROUP_MEMBER(ped, getGroup());
				PED::SET_PED_NEVER_LEAVES_GROUP(ped, true);
			}

			PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, guardhash);
			
			++i;
		}
	}
}