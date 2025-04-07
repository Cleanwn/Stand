#pragma once

#include "dx_common.hpp"
#include "fwddecl.hpp"

#include "vector.hpp"
#include "Vector2Plus.hpp"

namespace Stand
{
	struct DrawUtil3d
	{
		inline static bool exclusive_ar = false;

		static void draw_line(float from_x, float from_y, float from_z, float to_x, float to_y, float to_z, int r, int g, int b, int a);
		static void draw_line(const v3& from, const v3& to, int r, int g, int b, int a);
		static void draw_line(const v3& from, const v3& to, const DirectX::SimpleMath::Color& colour);
		static void draw_line(const v3& from, const v3& to);
		static void draw_line_native(const v3& from, const v3& to, const DirectX::SimpleMath::Color& colour);
		static void draw_line_native(const v3& from, const v3& to, int r, int g, int b, int a);
		static void draw_line_native(float from_x, float from_y, float from_z, float to_x, float to_y, float to_z, int r, int g, int b, int a);
		static void draw_line_directx(const v3& from, const v3& to, const DirectX::SimpleMath::Color& colour);
		static void draw_line_directx(const v3& from, const v3& to, int r, int g, int b, int a);

		static void draw_ar_beacon(const Vector2Plus& pos);
		static void draw_ar_marker(const v3& pos, const DirectX::SimpleMath::Color& colour);
		static void draw_ar_marker(const v3& pos);
		static void draw_line_and_ar_beacon(const v3& line_start, const v3& highlight_pos);
		static void draw_line_and_ar_beacon(const v3& highlight_pos);
		static void draw_line_if_offscreen(const v3& highlight_pos);
		static void draw_focus_line(const v3& highlight_pos, const DirectX::SimpleMath::Color& colour);
		static void draw_focus_line(const v3& highlight_pos);
		static void draw_portal(const v3& pos, const float heading);

		//static void draw_ground_line(const v3& pos);
		static void draw_poly(const v3& pos1, const v3& pos2, const v3& pos3, int r, int g, int b, int a = 255);
		static void draw_camera(const v3& pos, const v3& rot);
		static void draw_circle(const Circle& c, const v3& pos);
		static void draw_sphere(const v3& pos, float radius, int r, int g, int b, int a = 255, int quality = 40);
	};
}
