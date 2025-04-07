#pragma once

#include "datBitBuffer.hpp"
#include "gta_net_object_mgr.hpp"
#include "pointers.hpp"

struct CNetworkPlaySoundEvent // Real name: CPlaySoundEvent
{
	bool m_bUseEntity = false;
	uint16_t m_EntityID = 0;
	Stand::v3 m_Position = { 0.0f, 0.0f, 0.0f };
	bool hasSetNameHash = false;
	hash_t m_setNameHash = 0; // a.k.a, audioRef
	hash_t m_soundNameHash = 0; // a.k.a, audioName
	uint8_t m_SoundID = 0;

	CNetworkPlaySoundEvent(rage::datBitBuffer* buffer)
	{
		m_bUseEntity = buffer->readBool();

		if (m_bUseEntity)
		{
			m_EntityID = buffer->readU16(13);
		}
		else
		{
			m_Position = buffer->readPosition(19);
		}

		hasSetNameHash = buffer->readBool();

		if (hasSetNameHash)
		{
			m_setNameHash = buffer->readU32();
		}

		m_soundNameHash = buffer->readU32();
		m_SoundID = buffer->readU8();
	}

	[[nodiscard]] bool isSpammySound() const noexcept
	{
		return (m_Position == Stand::v3(1111.0121f, 229.81728f, -49.134766f) && m_setNameHash == ATSTRINGHASH("dlc_vw_casino_lucky_wheel_sounds"))
			|| (m_soundNameHash == ATSTRINGHASH("trigger"))
			|| (m_soundNameHash == ATSTRINGHASH("fire")) // Firing from Avenger
			|| (m_soundNameHash == 2125493300) // Unk, passenger firing from Buzzard apparently
			;
	}

	[[nodiscard]] bool isCloseFreeroamSound() const noexcept
	{
		return m_setNameHash == ATSTRINGHASH("dlc_vw_table_games_sounds")
			|| (m_setNameHash == 0 && m_soundNameHash == ATSTRINGHASH("DLC_XM_Explosions_Orbital_Cannon"))
			|| (m_setNameHash == ATSTRINGHASH("dlc_sm_generic_mission_sounds") && m_soundNameHash == ATSTRINGHASH("air_drop_parachute"))
			|| (m_setNameHash == ATSTRINGHASH("dlc_xm_iaa_player_facility_sounds") && m_soundNameHash == ATSTRINGHASH("scanner_alarm_os"))
			|| (m_setNameHash == ATSTRINGHASH("alarms_soundset") && m_soundNameHash == ATSTRINGHASH("klaxon_01")) // (Us: false, Pos: (3426.9036, 3761.9792, 33.9989), Dict: -1657326825, Sound: 1550920767, ID: 6), Humane Labs Doors in some org mission
			|| (m_setNameHash == ATSTRINGHASH("DLC_Exec_Air_Drop_Sounds") && m_soundNameHash == ATSTRINGHASH("drop_package")) // (Us: false, Pos: (-240.99702, 381.48193, 112.96326), Dict: 1922682698, Sound: -644886390, ID: 0)
			|| (m_setNameHash == ATSTRINGHASH("DLC_Exec_Land_Multiple_Sounds") && m_soundNameHash == ATSTRINGHASH("drop_package")) // (Us: false, Pos: (-240.99702, 381.48193, 112.96326), Dict: 1922682698, Sound: -644886390, ID: 0)
			|| (m_setNameHash == ATSTRINGHASH("dlc_h3_lovemachine_sounds") && m_soundNameHash == ATSTRINGHASH("calculate_outcome")) // (Bool: false, Pos: (2735.3425, -371.35687, -50.00232), Dict: -1445562881, Sound: 1053381780, ID: 5)
			;
	}

	[[nodiscard]] bool attachedToPlayer() const
	{
		if (auto n = (*Stand::pointers::network_object_mgr)->find_object_by_id(m_EntityID, true))
		{
			if (auto ent = n->GetEntity())
			{
				return Stand::AbstractEntity::get((CPhysical*)ent).isAPlayer() && m_Position.isNull();
			}
		}

		return false;
	}

	[[nodiscard]] bool isBlacklistedSound() const noexcept
	{
		return m_setNameHash == ATSTRINGHASH("hud_frontend_default_soundset") && m_soundNameHash == ATSTRINGHASH("mp_award");
	}

	[[nodiscard]] const std::string toString() const
	{
		return fmt::format(fmt::runtime(soup::ObfusString(" (Bool: {}, Pos: ({}, {}, {}), Dict: {}, Sound: {}, ID: {})").str()), 
			Stand::g_player_ped.isNetObjId(m_EntityID) || Stand::g_player_veh.isNetObjId(m_EntityID),
			m_Position.x,
			m_Position.y,
			m_Position.z,
			m_setNameHash,
			m_soundNameHash,
			m_SoundID
		);
	}
};