#include "CommandThousandsSep.hpp"

#include <soup/Bytepatch.hpp>

#include "eSCRIPT_HUD_COMPONENT.hpp"
#include "natives.hpp"
#include "NativeTableHooks.hpp"
#include "pointers.hpp"

namespace Stand
{
	CommandThousandsSep::CommandThousandsSep(CommandList* const parent)
		: CommandToggle(parent, LOC("HUDTS"), CMDNAMES_OBF("thousands"), LOC("HUDTS_H"))
	{
	}

	static soup::Bytepatch patch;
	static bool override_int = false;

	void CommandThousandsSep::onEnable(Click& click)
	{
		enablePatch();

		// BEGIN_SCALEFORM_SCRIPT_HUD_MOVIE_METHOD
		NativeTableHooks::createNativeEntrypointOverride(0x98C494FD5BDFBFD5, [](rage::scrNativeCallContext& ctx)
		{
			if (ctx.getArg<int>(0) == HUD_CHIP || ctx.getArg<int>(0) == HUD_CHIP_CHANGE)
			{
				override_int = true;
			}
			return NativeTableHooks::og(0x98C494FD5BDFBFD5)(ctx);
		});

		// SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT
		NativeTableHooks::createNativeEntrypointOverride(0xC3D0841A0CC546A6, [](rage::scrNativeCallContext& ctx)
		{
			if (override_int)
			{
				override_int = false;
				char buf[64];
				pointers::CTextConversion_FormatIntForHumans(ctx.getArg<int64_t>(0), buf, sizeof(buf), "%s", true);
				GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(buf);
			}
			else
			{
				NativeTableHooks::og(0xC3D0841A0CC546A6)(ctx);
			}
		});

		// NETWORK_GET_STRING_WALLET_BALANCE
		NativeTableHooks::createNativeEntrypointOverride(0xF9B10B529DCFB33B, [](rage::scrNativeCallContext& ctx)
		{
			disablePatch();
			NativeTableHooks::og(0xF9B10B529DCFB33B)(ctx);
			enablePatch();
		});

		// NETWORK_GET_STRING_BANK_BALANCE
		NativeTableHooks::createNativeEntrypointOverride(0xA6FA3979BED01B81, [](rage::scrNativeCallContext& ctx)
		{
			disablePatch();
			NativeTableHooks::og(0xA6FA3979BED01B81)(ctx);
			enablePatch();
		});

		// NETWORK_GET_STRING_BANK_WALLET_BALANCE
		NativeTableHooks::createNativeEntrypointOverride(0x700AF71AE615E6DD, [](rage::scrNativeCallContext& ctx)
		{
			disablePatch();
			NativeTableHooks::og(0x700AF71AE615E6DD)(ctx);
			enablePatch();
		});
	}

	void CommandThousandsSep::onDisable(Click& click)
	{
		disablePatch();

		NativeTableHooks::removeNativeEntrypointOverride(0x98C494FD5BDFBFD5);
		NativeTableHooks::removeNativeEntrypointOverride(0xC3D0841A0CC546A6);
		NativeTableHooks::removeNativeEntrypointOverride(0xF9B10B529DCFB33B);
		NativeTableHooks::removeNativeEntrypointOverride(0xA6FA3979BED01B81);
		NativeTableHooks::removeNativeEntrypointOverride(0x700AF71AE615E6DD);
	}

	void CommandThousandsSep::enablePatch()
	{
		if (pointers::FormatInt64ToCash_mov_bAddCurrencyDeliminator)
		{
			uint8_t bytes[] = {
				0xc6, 0x44, 0x24, 0x20, 0x01 // mov byte ptr [rsp+20h], r9b -> mov byte ptr [rsp+20h], 1
			};
			patch.initPatch(pointers::FormatInt64ToCash_mov_bAddCurrencyDeliminator, bytes, COUNT(bytes));
		}
	}

	void CommandThousandsSep::disablePatch()
	{
		patch.restore();
	}
}
