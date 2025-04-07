#pragma once

#include <string>
#include <fmt/xchar.h>

#include "lang.hpp"
#include "Renderer.hpp"
#include "StringCastable.hpp"

namespace Stand
{
	extern bool g_force_debug_text;

	extern void onTick_drawDebugText();

	extern void drawDebugText(const std::wstring& str);
	extern void drawDebugText(std::wstring&& str);
	extern void drawDebugText(const StringCastable& str);

	extern void drawInfoText(std::string&& value, const std::string& name);

	template <typename T>
	static void drawInfoNumber(T value, const std::string& name)
	{
		switch (g_renderer.info_alignment & 0b11)
		{
		case 0b00: // Left
			drawDebugText(std::move(std::string(name).append(": ").append(fmt::to_string(value))));
			break;

		default:
			std::string str{};
			if constexpr (std::is_same_v<T, float>)
			{
				str = fmt::to_string((int32_t)value);
			}
			else
			{
				str = fmt::to_string(value);
			}
			str.push_back(' ');
			str.append(name);
			drawDebugText(std::move(str));
			break;
		}
	}

	template <typename T>
	static void drawInfoNumber(T value, const wchar_t* name)
	{
		switch (g_renderer.info_alignment & 0b11)
		{
		case 0b00: // Left
			drawDebugText(std::move(std::wstring(name).append(L": ").append(fmt::to_wstring(value))));
			break;

		default:
			std::wstring str{};
			if constexpr (std::is_same_v<T, float>)
			{
				str = fmt::to_wstring((int32_t)value);
			}
			else
			{
				str = fmt::to_wstring(value);
			}
			str.push_back(L' ');
			str.append(name);
			drawDebugText(std::move(str));
			break;
		}
	}

	template <typename T>
	static void drawInfoNumber(T value, uint32_t singular_name, uint32_t plural_name)
	{
		switch (g_renderer.info_alignment & 0b11)
		{
		case 0b00: // Left
			drawDebugText(std::move(std::string(Lang::get(plural_name)).append(": ").append(fmt::to_string(value))));
			break;

		default:
			auto str = fmt::to_string(value);
			str.push_back(' ');
			str.append(Lang::get(value == 1 ? singular_name : plural_name));
			drawDebugText(std::move(str));
			break;
		}
	}
}
