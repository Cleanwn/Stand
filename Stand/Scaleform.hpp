#pragma once

#include "natives.hpp"

namespace Stand
{
	class Scaleformcall
	{
	public:
		int scaleform;
		const char* method_name;

		static void addArg(const char* val)
		{
			GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(val);
		}

		static void addArg(int val)
		{
			GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(val);
		}

		static void addArg(float val)
		{
			GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_FLOAT(val);
		}

		static void addArg(bool val)
		{
			GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_BOOL(val);
		}

		template <typename...Args>
		void operator()(const Args&...args) const
		{
			GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(scaleform, method_name);
			(addArg(args), ...);
			GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
		}
	};

	class Scaleform
	{
	public:
		const char* name;
		int inst = 0;

		Scaleform(const char* name)
			: name(name)
		{
		}

		[[nodiscard]] operator int() const noexcept
		{
			return inst;
		}

		[[nodiscard]] bool ensureLoaded();

		[[nodiscard]] constexpr bool isLoaded() const noexcept
		{
			return inst != 0 && GRAPHICS::HAS_SCALEFORM_MOVIE_LOADED(inst);
		}

		void unload()
		{
			if (isLoaded())
			{
				GRAPHICS::SET_SCALEFORM_MOVIE_AS_NO_LONGER_NEEDED(&inst);
			}
		}

		void forget() noexcept
		{
			inst = 0;
		}

		[[nodiscard]] Scaleformcall call(const char* method_name) const noexcept
		{
			return Scaleformcall{
				inst,
				method_name
			};
		}

		[[nodiscard]] Scaleformcall operator [](const char* method_name) const noexcept
		{
			return Scaleformcall{
				inst,
				method_name
			};
		}

		void drawFullscreen(int alpha = 255) const
		{
			GRAPHICS::DRAW_SCALEFORM_MOVIE_FULLSCREEN(inst, 255, 255, 255, alpha, 0);
		}

		void draw3d(const v3& pos, const v3& rot, const float scale) const
		{
			const auto v3scale = v3{ 1.0f, 1.0f, 1.0f } * scale;
			GRAPHICS::DRAW_SCALEFORM_MOVIE_3D_SOLID(inst, pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, 1.0f, 1.0f, 1.0f, v3scale.x, v3scale.y / GRAPHICS::GET_ASPECT_RATIO(FALSE), v3scale.z, 2);
		}
	};

	class ScaleformTextcanvas : public Scaleform
	{
	public:
		bool inited = false;

		ScaleformTextcanvas()
			: Scaleform("textcanvas")
		{
		}

		[[nodiscard]] bool ensureLoaded()
		{
			if (Scaleform::ensureLoaded())
			{
				if (!inited)
				{
					inited = true;
					call("BEGIN_SETUP_TEMPLATE")(2);
				}
				return true;
			}
			return false;
		}

		void unload()
		{
			inited = false;
			Scaleform::unload();
		}

		void setText(const char* text)
		{
			call("UPDATE_PROPERTY")(0, 0, text);
		}

		void setColour(int hud_colour)
		{
			call("UPDATE_PROPERTY")(0, 2, hud_colour);
		}
	};
}
