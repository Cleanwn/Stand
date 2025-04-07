#include "DrawUtil3d.hpp"

#include <soup/Poly.hpp>
#include <soup/UvSphere.hpp>

#include "AbstractEntity.hpp"
#include "huddecl.hpp"
#include "natives.hpp"
#include "Renderer.hpp"

namespace Stand
{
	void DrawUtil3d::draw_line(float from_x, float from_y, float from_z, float to_x, float to_y, float to_z, int r, int g, int b, int a)
	{
		if (!g_renderer.stream_proof)
		{
			draw_line_native(from_x, from_y, from_z, to_x, to_y, to_z, r, g, b, a);
		}
		else
		{
			draw_line_directx(v3(from_x, from_y, from_z), v3(to_x, to_y, to_z), r, g, b, a);
		}
	}

	void DrawUtil3d::draw_line(const v3& from, const v3& to, int r, int g, int b, int a)
	{
		if (!g_renderer.stream_proof)
		{
			draw_line_native(from, to, r, g, b, a);
		}
		else
		{
			draw_line_directx(from, to, r, g, b, a);
		}
	}

	void DrawUtil3d::draw_line(const v3& from, const v3& to, const DirectX::SimpleMath::Color& colour)
	{
		if (g_renderer.stream_proof)
		{
			draw_line_directx(from, to, colour);
		}
		else
		{
			draw_line_native(from, to, colour);
		}
	}

	void DrawUtil3d::draw_line(const v3& from, const v3& to)
	{
		return draw_line(from, to, g_renderer.arColour);
	}

	void DrawUtil3d::draw_line_native(const v3& from, const v3& to, const DirectX::SimpleMath::Color& colour)
	{
		draw_line_native(from, to, (int)(colour.x * 255.0f), (int)(colour.y * 255.0f), (int)(colour.z * 255.0f), (int)(colour.w * 255.0f));
	}

	void DrawUtil3d::draw_line_native(const v3& from, const v3& to, int r, int g, int b, int a)
	{
		draw_line_native(from.x, from.y, from.z, to.x, to.y, to.z, r, g, b, a);
	}

	void DrawUtil3d::draw_line_native(float from_x, float from_y, float from_z, float to_x, float to_y, float to_z, int r, int g, int b, int a)
	{
		GRAPHICS::DRAW_LINE(from_x, from_y, from_z, to_x, to_y, to_z, r, g, b, a);
	}

	void DrawUtil3d::draw_line_directx(const v3& from, const v3& to, const DirectX::SimpleMath::Color& colour)
	{
		g_renderer.drawLineThisTickCP(from.getScreenPos(), to.getScreenPos(), colour);
	}

	void DrawUtil3d::draw_line_directx(const v3& from, const v3& to, int r, int g, int b, int a)
	{
		draw_line_directx(from, to, Renderer::convert_to_dx_colour(r, g, b, a));
	}

	void DrawUtil3d::draw_ar_beacon(const Vector2Plus& pos)
	{
		if (!g_renderer.stream_proof)
		{
			const DirectX::SimpleMath::Color colour = g_renderer.arColour;
			if (!pos.z_is_guessed)
			{
				draw_ar_marker(pos, colour);
			}
			const auto dist_to_cam = CAM::GET_FINAL_RENDERED_CAM_COORD().distanceTopdown(pos);
			float num_lines = 360.0f / (dist_to_cam / 8.0f); // At distance of 8 metres, we need about 360 lines.
			if (num_lines < 36.0f)
			{
				num_lines = 36.0f;
			}
			else if (num_lines > 10'000.0f)
			{
				num_lines = 10'000.0f;
			}
			const float space = 360.0f / num_lines;
			for (float heading = 0.0f; heading < 360.0f; heading += space)
			{
				auto px = pos + (v3(0.0f, 0.0f, heading).toDirNoZ() * 0.2f);
				DrawUtil3d::draw_line(px.x, px.y, -200.0f, px.x, px.y, 10000.0f, (int)(colour.x * 255.0f), (int)(colour.y * 255.0f), (int)(colour.z * 255.0f), (int)(colour.w * 255.0f));
			}
		}
		else
		{
			DrawUtil3d::draw_focus_line(pos);
		}
	}

	void DrawUtil3d::draw_ar_marker(const v3& pos, const DirectX::SimpleMath::Color& colour)
	{
		if (GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED("darts"))
		{
			const rage::Vector2 screen_pos = pos.getScreenPos();
			if (screen_pos.isInBounds())
			{
				auto size = g_renderer.sizeH2C(32.0f, 32.0f);
				GRAPHICS::DRAW_SPRITE("darts", "dart_reticules_zoomed", screen_pos.x, screen_pos.y, size.x / g_renderer.client_size.x, size.y / g_renderer.client_size.y, 0.0f, (int)(colour.x * 255.0f), (int)(colour.y * 255.0f), (int)(colour.z * 255.0f), (int)(colour.w * 255.0f), true, 0);
			}
		}
		else
		{
			GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT("darts", true);
		}
	}

	void DrawUtil3d::draw_ar_marker(const v3& pos)
	{
		draw_ar_marker(pos, g_renderer.arColour);
	}

	void DrawUtil3d::draw_line_and_ar_beacon(const v3& line_start, const v3& highlight_pos)
	{
		if (!highlight_pos.getScreenPos().isInBounds())
		{
			draw_line(line_start, highlight_pos);
		}
		return draw_ar_beacon(highlight_pos);
	}

	void DrawUtil3d::draw_line_and_ar_beacon(const v3& highlight_pos)
	{
		if (!g_renderer.stream_proof)
		{
			draw_line_if_offscreen(highlight_pos);
			draw_ar_beacon(highlight_pos);
		}
		else
		{
			draw_line_directx(g_player_ped.getPos(), highlight_pos, g_renderer.arColour);
		}
	}

	void DrawUtil3d::draw_line_if_offscreen(const v3& highlight_pos)
	{
		if (!highlight_pos.getScreenPos().isInBounds())
		{
			draw_focus_line(highlight_pos);
		}
	}

	void DrawUtil3d::draw_focus_line(const v3& highlight_pos, const DirectX::SimpleMath::Color& colour)
	{
		draw_line(g_player_ped.getPos(), highlight_pos, colour);
	}

	void DrawUtil3d::draw_focus_line(const v3& highlight_pos)
	{
		draw_focus_line(highlight_pos, g_renderer.arColour);
	}

	void DrawUtil3d::draw_portal(const v3& pos, const float heading)
	{
		for (int i = 0; i < 360; i++)
		{
			v3 line_from = (pos + (v3(float(i), 0.f, heading).toDir() * 10.0f));
			v3 line_to = (pos + (v3(float(i + 1), 0.f, heading).toDir() * 10.0f));
			draw_line(line_from.x, line_from.y, line_from.z, line_to.x, line_to.y, line_to.z, (int)(g_renderer.arColour.x * 255.0f), (int)(g_renderer.arColour.y * 255.0f), (int)(g_renderer.arColour.z * 255.0f), (int)(g_renderer.arColour.w * 255.0f));
		}
	}

	/*static constexpr auto dict = "deadline";
	static constexpr auto texture = "deadline_trail_01";

	void DrawUtil3d::draw_ground_line(const v3& pos)
	{
		if (!GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED(dict))
		{
			GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT(dict, false);
		}
		else
		{
			GRAPHICS::_DRAW_SPRITE_POLY(
				pos.x + 0.5f, pos.y - 0.5f, get_ground_z(pos.x + 0.5f, pos.y - 0.5f, pos.z) + 0.01f,
				pos.x + 0.5f, pos.y + 0.5f, get_ground_z(pos.x + 0.5f, pos.y + 0.5f, pos.z) + 0.01f,
				pos.x - 0.5f, pos.y - 0.5f, get_ground_z(pos.x - 0.5f, pos.y - 0.5f, pos.z) + 0.01f,
				255, 0, 255, 255,
				dict,
				texture,
				0, 1, 0,
				1, 1, 0,
				0, 0, 0
			);
			GRAPHICS::_DRAW_SPRITE_POLY(
				pos.x - 0.5f, pos.y - 0.5f, get_ground_z(pos.x - 0.5f, pos.y - 0.5f, pos.z) + 0.01f,
				pos.x + 0.5f, pos.y + 0.5f, get_ground_z(pos.x + 0.5f, pos.y + 0.5f, pos.z) + 0.01f,
				pos.x - 0.5f, pos.y + 0.5f, get_ground_z(pos.x - 0.5f, pos.y + 0.5f, pos.z) + 0.01f,
				255, 0, 255, 255,
				dict,
				texture,
				0, 0, 0,
				1, 1, 0,
				1, 0, 0
			);
		}
	}*/

	void DrawUtil3d::draw_poly(const v3& pos1, const v3& pos2, const v3& pos3, int r, int g, int b, int a)
	{
		/*GRAPHICS::_DRAW_SPRITE_POLY(
			pos1.x, pos1.y, pos1.z,
			pos2.x, pos2.y, pos2.z,
			pos3.x, pos3.y, pos3.z,
			r, g, b, a,
			nullptr,
			nullptr,
			0, 0, 0,
			0, 0, 0,
			0, 0, 0
		);*/
		GRAPHICS::DRAW_POLY(
			pos1.x, pos1.y, pos1.z,
			pos2.x, pos2.y, pos2.z,
			pos3.x, pos3.y, pos3.z,
			r, g, b, a
		);
	}

	void DrawUtil3d::draw_camera(const v3& pos, const v3& rot)
	{
		constexpr float length = 0.5f;

		auto top_left = pos + (v3(rot.x + 7.0f, 0.0f, rot.z + 15.0f).toDir() * length);
		auto top_right = pos + (v3(rot.x + 7.0f, 0.0f, rot.z - 15.0f).toDir() * length);
		auto bottom_left = pos + (v3(rot.x - 7.0f, 0.0f, rot.z + 15.0f).toDir() * length);
		auto bottom_right = pos + (v3(rot.x - 7.0f, 0.0f, rot.z - 15.0f).toDir() * length);

		draw_line(pos, top_left, g_renderer.arColour);
		draw_line(pos, top_right, g_renderer.arColour);
		draw_line(pos, bottom_left, g_renderer.arColour);
		draw_line(pos, bottom_right, g_renderer.arColour);

		draw_line(top_left, top_right, g_renderer.arColour);
		draw_line(bottom_left, bottom_right, g_renderer.arColour);
		draw_line(top_left, bottom_left, g_renderer.arColour);
		draw_line(top_right, bottom_right, g_renderer.arColour);
	}

	void DrawUtil3d::draw_circle(const Circle& c, const v3& pos)
	{
		bool first_side = true;
		v3 last_circle_pos;
		for (const auto& side : c.points)
		{
			auto circle_pos = pos;
			circle_pos.x += side.x;
			circle_pos.y += side.y;
			if (!first_side)
			{
				draw_poly(pos, last_circle_pos, circle_pos, 255, 0, 255); // top
				//draw_poly(pos, circle_pos, last_circle_pos); // bottom
			}
			first_side = false;
			last_circle_pos = circle_pos;
		}
	}

	void DrawUtil3d::draw_sphere(const v3& pos, float radius, int r, int g, int b, int a, int quality)
	{
		for (const auto& p : soup::UvSphere{ pos, radius }.toPolys(quality))
		{
			draw_poly(p.a, p.b, p.c, r, g, b, a);
		}
	}
}
