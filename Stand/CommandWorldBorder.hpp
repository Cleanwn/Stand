#pragma once

#include "CommandSlider.hpp"

#include "AbstractEntity.hpp"
#include "DrawUtil3d.hpp"

namespace Stand
{
	class CommandWorldBorder : public CommandSlider
	{
	private:
		CommandColour* const colour;
		float step_size = 1.0f;

	public:
		explicit CommandWorldBorder(CommandList* const parent, CommandColour* colour)
			: CommandSlider(parent, LOC("WRLDBDR"), CMDNAMES("worldborder"), LOC("WRLDBDR_H"), 0, 12000, 0, 30), colour(colour)
		{
		}

	private:
		inline static float world_x_min = -4000.0f;
		inline static float world_x_max = 4500.0f;
		inline static float world_y_min = -4000.0f;
		inline static float world_y_max = 8000.0f;
		static constexpr float world_z_min = -200.0f;
		static constexpr float world_z_max = 2700.0f;

		void processBorderX(const v3& ped_pos, const float x)
		{
			float dist = abs(ped_pos.x - x);
			if (dist <= float(value))
			{
				const auto a = colour->a->value - int(dist / float(value) * 255.0f);
				for (float y = world_y_min; y < world_y_max; y += step_size)
				{
					DrawUtil3d::draw_line_native(x, y, world_z_min, x, y, world_z_max, colour->r->value, colour->g->value, colour->b->value, a);
				}
				for (float z = world_z_min; z < world_z_max; z += step_size)
				{
					DrawUtil3d::draw_line_native(x, world_y_min, z, x, world_y_max, z, colour->r->value, colour->g->value, colour->b->value, a);
				}
			}
		}

		void processBorderY(const v3& ped_pos, const float y)
		{
			float dist = abs(ped_pos.y - y);
			if (dist <= float(value))
			{
				const auto a = colour->a->value - int(dist / float(value) * 255.0f);
				for (float x = world_x_min; x < world_x_max; x += step_size)
				{
					DrawUtil3d::draw_line_native(x, y, world_z_min, x, y, world_z_max, colour->r->value, colour->g->value, colour->b->value, a);
				}
				for (float z = world_z_min; z < world_z_max; z += step_size)
				{
					DrawUtil3d::draw_line_native(world_x_min, y, z, world_x_max, y, z, colour->r->value, colour->g->value, colour->b->value, a);
				}
			}
		}

		void processBorderZ(const v3& ped_pos, const float z)
		{
			float dist = abs(ped_pos.z - z);
			if (dist <= float(value))
			{
				const auto a = colour->a->value - int(dist / float(value) * 255.0f);
				for (float x = world_x_min; x < world_x_max; x += step_size)
				{
					DrawUtil3d::draw_line_native(x, world_y_min, z, x, world_y_max, z, colour->r->value, colour->g->value, colour->b->value, a);
				}
				for (float y = world_y_min; y < world_y_max; y += step_size)
				{
					DrawUtil3d::draw_line_native(world_x_min, y, z, world_x_max, y, z, colour->r->value, colour->g->value, colour->b->value, a);
				}
			}
		}

	public:
		void onChange(Click& click, int prev_value) final
		{
			if (this->value != 0)
			{
				const auto value = this->value;
				step_size = (float)value;
				if (step_size <= 6.0f)
				{
					step_size = 6.0f;
				}
				step_size = step_size / 30.0f * 1.0f;
				registerScriptTickEventHandler(click, [this, value]
				{
					if (this->value != value)
					{
						return false;
					}
					world_x_min = float(*pointers::world_x_min);
					world_x_max = float(*pointers::world_x_max);
					world_y_min = float(*pointers::world_y_min);
					world_y_max = float(*pointers::world_y_max);
					const v3 ped_pos = g_cam_ped.getPos();
					processBorderX(ped_pos, world_x_min);
					processBorderX(ped_pos, world_x_max);
					processBorderY(ped_pos, world_y_min);
					processBorderY(ped_pos, world_y_max);
					processBorderZ(ped_pos, world_z_min);
					processBorderZ(ped_pos, world_z_max);
					return true;
				});
			}
		}
	};
}
