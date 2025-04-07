#include "Camgr.hpp"

#include <soup/Quaternion.hpp>

#include "AbstractEntity.hpp"
#include "fwPool.hpp"
#include "gta_cam.hpp"
#include "gta_input.hpp"
#include "Hooking.hpp"
#include "hud_component.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "NativeTableHooks.hpp"
#include "pointers.hpp"
#include "Script.hpp"
#include "script_thread.hpp"
#include "SpectateMgr.hpp"
#include "Util.hpp"
#include "weapons.hpp"

namespace Stand
{
	void Camgr::ensure_custom_cam()
	{
		if (custom_cam == 0)
		{
			GtaThread::fromHash(ATSTRINGHASH("main_persistent"))->spoofAsOpt([]
			{
				const Vector3 pos = CAM::GET_GAMEPLAY_CAM_COORD();
				const Vector3 rot = CAM::GET_GAMEPLAY_CAM_ROT(2);
				custom_cam = CAM::CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPTED_CAMERA", pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, CAM::GET_GAMEPLAY_CAM_FOV(), false, 2);
				CAM::SET_CAM_ACTIVE(custom_cam, true);
				CAM::SET_CAM_AFFECTS_AIMING(custom_cam, true);
				CAM::RENDER_SCRIPT_CAMS(true, false, 0, false, false, false);
				disableCinematicCam();
			});
		}
	}

	void Camgr::cleanup()
	{
		if (custom_cam != 0)
		{
			GtaThread::fromHash(ATSTRINGHASH("main_persistent"))->spoofAsOpt([]
			{
				CAM::RENDER_SCRIPT_CAMS(false, false, 0, false, false, false);
				CAM::SET_CAM_ACTIVE(custom_cam, false);
				enableCinematicCam();
				CAM::DESTROY_CAM(custom_cam, false);
				custom_cam = 0;
			});
		}
	}

	bool Camgr::needsCustomCam()
	{
		if (!pos_from_gameplay_cam || !rot_from_gameplay_cam)
		{
			return true;
		}
		if (distance_from_character_multiplier != 1.0f)
		{
			if (g_player_ped.isAiming())
			{
				const auto weapon = Weapon::find(WEAPON::GET_SELECTED_PED_WEAPON(g_player_ped));
				return weapon == nullptr || weapon->category != Weapon::SNIPER;
			}
			else
			{
				return true;
			}
		}
		return false;
	}

	bool Camgr::cleanup_if_applicable()
	{
		if (!needsCustomCam())
		{
			cleanup();
			return false;
		}
		return custom_cam != 0;
	}

	void Camgr::set_pos(const v3& pos)
	{
		const v3 cam_pos = pos - g_player_ent.getPos();
		CAM::ATTACH_CAM_TO_ENTITY(custom_cam, g_player_ent, cam_pos.x, cam_pos.y, cam_pos.z, false);
	}

	void Camgr::set_rot(const v3& rot)
	{
		CAM::SET_CAM_ROT(custom_cam, rot.x, rot.y, rot.z, 2);
	}

	void Camgr::set_rot(const soup::Quaternion& rot)
	{
		if (auto cam = (*pointers::cam_pool)->GetAt(custom_cam))
		{
			rot.toMatrix(cam->frame.view);
			cam->update_flags |= 0x80;
		}
	}

	void Camgr::set_fov(const float fov)
	{
		CAM::SET_CAM_FOV(custom_cam, fov);
	}

	void Camgr::copy_pos_from_gameplay_cam()
	{
		const auto gameplay_cam_pos = v3(CAM::GET_GAMEPLAY_CAM_COORD());
		if (distance_from_character_multiplier == 1.0f)
		{
			set_pos(gameplay_cam_pos);
		}
		else
		{
			const v3 player_pos = g_player_ped.getVisualPos();
			set_pos(player_pos + (player_pos.lookAt(gameplay_cam_pos).toDir() * gameplay_cam_pos.distance(player_pos) * distance_from_character_multiplier));
		}
	}

	void Camgr::copy_rot_from_gameplay_cam()
	{
		set_rot(CAM::GET_GAMEPLAY_CAM_ROT(2));
	}

	void Camgr::copy_fov_from_gameplay_cam()
	{
		set_fov(CAM::GET_GAMEPLAY_CAM_FOV());
	}

	void Camgr::on_tick()
	{
		GtaThread::fromHash(ATSTRINGHASH("main_persistent"))->spoofAsOpt([]
		{
			if (cinematic_cam_disabled != 0)
			{
				PAD::DISABLE_CONTROL_ACTION(2, INPUT_VEH_CIN_CAM, true);
				//CAM::SET_CINEMATIC_BUTTON_ACTIVE(false);
				CAM::SET_CINEMATIC_MODE_ACTIVE(false);
			}
			if (needsCustomCam())
			{
				ensure_custom_cam();
				if (pos_from_gameplay_cam)
				{
					copy_pos_from_gameplay_cam();
				}
				if (rot_from_gameplay_cam)
				{
					copy_rot_from_gameplay_cam();
				}
				if (fov_from_gameplay_cam)
				{
					copy_fov_from_gameplay_cam();
				}

				if (g_player_ped.isAiming())
				{
					HUD::SHOW_HUD_COMPONENT_THIS_FRAME(HUD_RETICLE);
				}
			}
			else
			{
				cleanup();
			}
			v3 effective_focus_pos = focus_pos;
			if (potato_mode)
			{
				effective_focus_pos = { 0.0f, 0.0f, 10000.0f };
			}
			if (!effective_focus_pos.isNull())
			{
				STREAMING::SET_GAME_PAUSES_FOR_STREAMING(FALSE);
				STREAMING::SET_FOCUS_POS_AND_VEL(effective_focus_pos.x, effective_focus_pos.y, effective_focus_pos.z, 0.0f, 0.0f, 0.0f);
				SpectateMgr::onTickWithAlteredFocus();
			}
		});
	}

	void Camgr::lock_pos(const v3& pos)
	{
		pos_from_gameplay_cam = false;
		ensure_custom_cam();
		set_pos(pos);
	}

	void Camgr::unlock_pos()
	{
		pos_from_gameplay_cam = true;
		if (cleanup_if_applicable())
		{
			copy_pos_from_gameplay_cam();
		}
	}

	void Camgr::set_distance_from_character_multiplier(const float multiplier)
	{
		if ((distance_from_character_multiplier = multiplier) == 1.0f)
		{
			cleanup_if_applicable();
		}
		else
		{
			ensure_custom_cam();
		}
	}

	void Camgr::lock_rot(const v3& rot)
	{
		rot_from_gameplay_cam = false;
		ensure_custom_cam();
		set_rot(rot);
	}

	void Camgr::lock_rot(const soup::Quaternion& rot)
	{
		rot_from_gameplay_cam = false;
		ensure_custom_cam();
		set_rot(rot);
	}

	void Camgr::unlock_rot()
	{
		rot_from_gameplay_cam = true;
		if (cleanup_if_applicable())
		{
			copy_rot_from_gameplay_cam();
		}
	}

	static void IS_GAMEPLAY_CAM_RENDERING(rage::scrNativeCallContext& ctx)
	{
		ctx.setReturnValue<BOOL>(true);
	}

	void Camgr::lock_fov(const float fov)
	{
		if (fov_from_gameplay_cam)
		{
			fov_from_gameplay_cam = false;
			NativeTableHooks::createNativeEntrypointOverride(0x39B5D1B10383F0C8, &IS_GAMEPLAY_CAM_RENDERING);
		}
		g_hooking.fov_override = fov;
		set_fov(fov);
	}

	void Camgr::unlock_fov()
	{
		if (!fov_from_gameplay_cam)
		{
			fov_from_gameplay_cam = true;
			NativeTableHooks::removeNativeEntrypointOverride(0x39B5D1B10383F0C8);
		}
		g_hooking.fov_override = FLT_MAX;
	}

	void Camgr::lock_focus(const v3& pos)
	{
		focus_pos = pos;
	}

	void Camgr::unlock_focus()
	{
		focus_pos = v3();
		clear_focus_if_applicable();
	}

	bool Camgr::shouldClearFocus()
	{
		return !potato_mode && focus_pos.isNull();
	}

	void Camgr::clear_focus_if_applicable()
	{
		if (shouldClearFocus())
		{
			STREAMING::CLEAR_FOCUS();
		}
	}

	void Camgr::stopAnyActiveCamCommand()
	{
		if (dedup)
		{
			dedup = false;
			Script::current()->yield();
		}
	}

	void Camgr::startCamCommand()
	{
		stopAnyActiveCamCommand();
		dedup = true;
	}

	bool Camgr::canCamCommandRun()
	{
		return dedup;
	}

	void Camgr::endCamCommand()
	{
		dedup = false;
	}

	void Camgr::disableCinematicCam()
	{
		cinematic_cam_disabled++;
	}

	void Camgr::enableCinematicCam()
	{
		cinematic_cam_disabled--;
	}
}
