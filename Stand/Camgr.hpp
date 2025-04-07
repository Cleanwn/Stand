#pragma once

#include <soup/fwd.hpp>

#include "natives_decl.hpp"
#include "vector.hpp"

namespace Stand
{
	class Camgr
	{
	private:
		inline static Cam custom_cam = 0;
		inline static uint8_t cinematic_cam_disabled = 0;
		inline static bool pos_from_gameplay_cam = true;
		inline static float distance_from_character_multiplier = 1.0f;
		inline static bool rot_from_gameplay_cam = true;
		inline static bool fov_from_gameplay_cam = true;
	public:
		inline static bool potato_mode = false;
	private:
		inline static v3 focus_pos;
		inline static bool dedup = false;
	public:
		inline static bool user_fov_allowed = true;
		inline static bool freecam_focus_allowed = true;

		static void ensure_custom_cam();
	public:
		static void cleanup();
	private:
		static bool needsCustomCam();
		static bool cleanup_if_applicable();
		static void set_pos(const v3& pos);
		static void set_rot(const v3& rot);
		static void set_rot(const soup::Quaternion& rot);
		static void set_fov(const float fov);
		static void copy_pos_from_gameplay_cam();
		static void copy_rot_from_gameplay_cam();
		static void copy_fov_from_gameplay_cam();
	public:
		static void on_tick();
		static void lock_pos(const v3& pos);
		static void unlock_pos();
		static void set_distance_from_character_multiplier(const float multiplier);
		static void lock_rot(const v3& rot);
		static void lock_rot(const soup::Quaternion& rot);
		static void unlock_rot();
		static void lock_fov(const float fov);
		static void unlock_fov();
		static void lock_focus(const v3& pos);
		static void unlock_focus();
		[[nodiscard]] static bool shouldClearFocus();
		static void clear_focus_if_applicable();
		static void stopAnyActiveCamCommand();
		static void startCamCommand();
		[[nodiscard]] static bool canCamCommandRun();
		static void endCamCommand();
		static void disableCinematicCam();
		static void enableCinematicCam();
	};
}
