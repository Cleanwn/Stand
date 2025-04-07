#pragma once

#include <soup/Quaternion.hpp>

#include "AbstractEntity.hpp"
#include "dedsec_hacks.hpp"
#include "PedBoneId.hpp"
#include "PlayerExcludes.hpp"
#include "TargetingData.hpp"
#include "vector.hpp"

#define DEDSEC_TUTORIAL_STATE_DONE 0
#define DEDSEC_TUTORIAL_STATE_TARGET 1
#define DEDSEC_TUTORIAL_STATE_OPTION 2

namespace Stand
{
	class AllEntitiesEveryTick
	{
	public:
		static inline bool dedsec_mode = false;
		static inline uint8_t dedsec_tutorial_state = DEDSEC_TUTORIAL_STATE_DONE;
	private:
		static inline AbstractEntity dedsec_selected_target;
		static inline v3 dedsec_selected_target_start_rot;
		static inline const DedsecHack* dedsec_last_hack = {};
		static inline time_t dedsec_deselect_delay_start = 0;
	public:
		static inline int dedsec_deselect_delay = 0;
		static inline bool dedsec_passive_reticle = true;
		static inline bool dedsec_passive_line = true;
		static inline bool dedsec_passive_box = false;
		static inline bool dedsec_active_reticle = true;
		static inline bool dedsec_active_line = true;
		static inline bool dedsec_active_box = true;
		static inline int dedsec_r = 255;
		static inline int dedsec_g = 0;
		static inline int dedsec_b = 255;
		static inline int dedsec_a = 255;
		static inline bool dedsec_enabled_reticle = false;
		static inline int phone_input_delay = 200;
		static inline time_t phone_input_start = 0;

		// aimbot config
		static inline bool aimbot = false;
		static inline TargetingData aimbot_targeting{};
		static inline bool aimbot_target_box = true;
		static inline int aimbot_lockout = 150;
		static inline int aimbot_smoothing = 10;
		static inline bool aimbot_los_check = true;
		static inline bool aimbot_magic_bullets = false;
		static inline PedBoneId aimbot_ped_bone = PedBoneId::SKEL_Head;
		// aimbot state
		static inline AbstractEntity aimbot_target;
		static inline bool aimbot_captured_rot = false;
		static inline v3 aimbot_rot;
		static inline time_t aimbot_last_attack = 0;
		static inline time_t aimbot_last_move = 0;
		static inline float aimbot_lerping = 1.0f;
		static inline soup::Quaternion aimbot_lerp_from;
		static inline soup::Quaternion aimbot_lerp_to;

		static inline punishment_t npc_existence_punishments = 0;
		static inline float npc_punishable_proximity = 10.0f;
		static inline punishment_t npc_proximity_punishments = 0;
		static inline punishment_t npc_hostility_punishments = 0;
		static inline bool npc_hostility_include_everyone = false;
		static inline bool npc_hostility_include_everyone_in_missions = false;
		static inline bool npc_hostility_include_friends = false;
		static inline bool npc_hostility_include_passengers = false;
		static inline bool npc_hostility_include_crew = false;
		static inline bool npc_hostility_include_org = false;
		static inline bool npc_needs_to_aim_at_user = true;
		static inline punishment_t npc_aim_punishments = 0;
		static inline bool player_needs_to_aim_at_user = true;
		static inline PlayerExcludes player_aim_excludes{ true };
		static inline punishment_t player_aim_punishments = 0;

		static void onTick();
		static void onTickEraseObjectsWithNoModelInfo();

		static void aimbotEnsureCamIsFree();
		static void aimbotDisable();
	};
}
