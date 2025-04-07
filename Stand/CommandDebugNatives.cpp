#include "CommandDebugNatives.hpp"

#include "AbstractEntity.hpp"
#include "drawDebugText.hpp"
#include "DrawUtil3d.hpp"
#include "natives.hpp"
#include "NativeTableHooks.hpp"
#include "Renderer.hpp"
#include "StringUtils.hpp"
#include "regular_event.hpp"

namespace Stand
{
	CommandDebugNatives::CommandDebugNatives(CommandList* const parent)
		: CommandToggle(parent, LOC("DBGNTVS"), CMDNAMES("debugnatives"), LOC("DBGNTVS_H"))
	{
	}

	void DRAW_DEBUG_TEXT_2D(rage::scrNativeCallContext& ctx) // (const char* text, float x, float y, float z, int red, int green, int blue, int alpha)
	{
		drawDebugText(std::string(1, '[').append(SCRIPT::GET_THIS_SCRIPT_NAME()).append("] ").append(ctx.getArg<const char*>(0)));
	}

	void DRAW_DEBUG_LINE(rage::scrNativeCallContext& ctx) // (float x1, float y1, float z1, float x2, float y2, float z2, int red, int green, int blue, int alpha)
	{
		if (ctx.getArg<float>(0) == ctx.getArg<float>(3) && ctx.getArg<float>(1) == ctx.getArg<float>(4) && ctx.getArg<float>(2) == ctx.getArg<float>(5))
		{
			drawDebugText(std::string(SCRIPT::GET_THIS_SCRIPT_NAME()).append(" is drawing a debug point"));
			GRAPHICS::DRAW_MARKER_SPHERE(ctx.getArg<float>(0), ctx.getArg<float>(1), ctx.getArg<float>(2), 0.1f, ctx.getArg<int>(6), ctx.getArg<int>(7), ctx.getArg<int>(8), ctx.getArg<float>(9));
		}
		else
		{
			//drawDebugText(std::string(SCRIPT::GET_THIS_SCRIPT_NAME()).append(" is drawing a debug line"));
			ctx.invokePassthrough(0x6B7256074AE34680);
		}
	}

	void DRAW_DEBUG_TEXT(rage::scrNativeCallContext& ctx) // (const char* text, float x, float y, float z, int red, int green, int blue, int alpha)
	{
		//drawDebugText(std::string(SCRIPT::GET_THIS_SCRIPT_NAME()).append(" is drawing 3D debug text"));
		g_renderer.draw3dTextThisTickH(v3(ctx.getArg<float>(1), ctx.getArg<float>(2), ctx.getArg<float>(3)), StringUtils::utf8_to_utf16(ctx.getArg<const char*>(0)), 1.0);
	}

	void DRAW_DEBUG_BOX(rage::scrNativeCallContext& ctx)
	{
		drawDebugText(std::string(SCRIPT::GET_THIS_SCRIPT_NAME()).append(" is drawing a debug box"));
		ctx.invokePassthrough(0xD3A9971CADAC7252);
	}

	void DRAW_DEBUG_SPHERE(rage::scrNativeCallContext& ctx)
	{
		drawDebugText(std::string(SCRIPT::GET_THIS_SCRIPT_NAME()).append(" is drawing a debug sphere"));
		DrawUtil3d::draw_sphere({ ctx.getArg<float>(0), ctx.getArg<float>(1), ctx.getArg<float>(2) }, ctx.getArg<float>(3), ctx.getArg<int>(4), ctx.getArg<int>(5), ctx.getArg<int>(6), ctx.getArg<int>(7));
	}

	static std::unordered_map<Ped, std::wstring> ped_debug_names;

	void SET_PED_NAME_DEBUG(rage::scrNativeCallContext& ctx)
	{
		if (ped_debug_names.find(ctx.getArg<Ped>(0)) == ped_debug_names.end())
		{
			ped_debug_names.emplace(ctx.getArg<Ped>(0), StringUtils::utf8_to_utf16(ctx.getArg<const char*>(1)));
			if (ped_debug_names.size() == 1)
			{
				reScriptTickEvent::registerHandlerInThreadContext([]() -> bool
				{
					auto i = ped_debug_names.begin();
					while (i != ped_debug_names.end())
					{
						auto ped = AbstractEntity::get(i->first);
						if (!ped.exists())
						{
							i = ped_debug_names.erase(i);
							continue;
						}
						g_renderer.draw3dTextThisTickH(ped.getPos(), i->second, 1.0);
						i++;
					}
					return !ped_debug_names.empty();
				}, "SET_PED_NAME_DEBUG");
			}
		}
	}

	static std::unordered_map<Vehicle, std::wstring> vehicle_debug_names;

	void SET_VEHICLE_NAME_DEBUG(rage::scrNativeCallContext& ctx)
	{
		if (vehicle_debug_names.find(ctx.getArg<Vehicle>(0)) == vehicle_debug_names.end())
		{
			vehicle_debug_names.emplace(ctx.getArg<Vehicle>(0), StringUtils::utf8_to_utf16(ctx.getArg<const char*>(1)));
			if (vehicle_debug_names.size() == 1)
			{
				reScriptTickEvent::registerHandlerInThreadContext([]() -> bool
				{
					auto i = vehicle_debug_names.begin();
					while (i != vehicle_debug_names.end())
					{
						auto veh = AbstractEntity::get(i->first);
						if (!veh.exists())
						{
							i = vehicle_debug_names.erase(i);
							continue;
						}
						g_renderer.draw3dTextThisTickH(veh.getPos(), i->second, 1.0);
						i++;
					}
					return !vehicle_debug_names.empty();
				}, "SET_VEHICLE_NAME_DEBUG");
			}
		}
	}

	static std::unordered_map<Cam, std::wstring> cam_debug_names;

	void SET_CAM_DEBUG_NAME(rage::scrNativeCallContext& ctx)
	{
		if (cam_debug_names.find(ctx.getArg<Cam>(0)) == cam_debug_names.end())
		{
			cam_debug_names.emplace(ctx.getArg<Cam>(0), StringUtils::utf8_to_utf16(ctx.getArg<const char*>(1)));
			if (cam_debug_names.size() == 1)
			{
				reScriptTickEvent::registerHandlerInThreadContext([]() -> bool
				{
					auto i = cam_debug_names.begin();
					while (i != cam_debug_names.end())
					{
						if (!CAM::DOES_CAM_EXIST(i->first))
						{
							i = cam_debug_names.erase(i);
						}
						g_renderer.draw3dTextThisTickH(CAM::GET_CAM_COORD(i->first), i->second, 1.0);
						i++;
					}
					return !cam_debug_names.empty();
				}, "SET_CAM_DEBUG_NAME");
			}
		}
	}

	void CommandDebugNatives::onEnable(Click& click)
	{
		ensureScriptThread(click, []
		{
			NativeTableHooks::createNativeEntrypointOverride(0xA3BB2E9555C05A8F, DRAW_DEBUG_TEXT_2D);
			NativeTableHooks::createNativeEntrypointOverride(0x7FDFADE676AA3CB0, DRAW_DEBUG_LINE);
			NativeTableHooks::createNativeEntrypointOverride(0x3903E216620488E8, DRAW_DEBUG_TEXT);
			NativeTableHooks::createNativeEntrypointOverride(0x083A2CA4F2E573BD, DRAW_DEBUG_BOX);
			NativeTableHooks::createNativeEntrypointOverride(0xAAD68E1AB39DA632, DRAW_DEBUG_SPHERE);
			NativeTableHooks::createNativeEntrypointOverride(0x98EFA132A4117BE1, SET_PED_NAME_DEBUG);
			NativeTableHooks::createNativeEntrypointOverride(0xBFDF984E2C22B94F, SET_VEHICLE_NAME_DEBUG);
			NativeTableHooks::createNativeEntrypointOverride(0x1B93E0107865DD40, SET_CAM_DEBUG_NAME);
			// Not implemented:
			// - _LOG_DEBUG_INFO: seems not interesting at all
			// - DRAW_DEBUG_CROSS: seems to only be used for crosshairs
			// - DRAW_DEBUG_LINE_WITH_TWO_COLOURS: seems to only be called if a local is true which it won't ever be
			// - SET_DEBUG_LINES_AND_SPHERES_DRAWING_ACTIVE
		});
	}

	void CommandDebugNatives::onDisable(Click& click)
	{
		ensureScriptThread(click, []
		{
			NativeTableHooks::removeNativeEntrypointOverride(0xA3BB2E9555C05A8F); // DRAW_DEBUG_TEXT_2D
			NativeTableHooks::removeNativeEntrypointOverride(0x7FDFADE676AA3CB0); // DRAW_DEBUG_LINE
			NativeTableHooks::removeNativeEntrypointOverride(0x3903E216620488E8); // DRAW_DEBUG_TEXT
			NativeTableHooks::removeNativeEntrypointOverride(0x083A2CA4F2E573BD); // DRAW_DEBUG_BOX
			NativeTableHooks::removeNativeEntrypointOverride(0xAAD68E1AB39DA632); // DRAW_DEBUG_SPHERE
			NativeTableHooks::removeNativeEntrypointOverride(0x98EFA132A4117BE1); // SET_PED_NAME_DEBUG
			NativeTableHooks::removeNativeEntrypointOverride(0xBFDF984E2C22B94F); // SET_VEHICLE_NAME_DEBUG
			NativeTableHooks::removeNativeEntrypointOverride(0x1B93E0107865DD40); // SET_CAM_DEBUG_NAME
		});
	}
}
