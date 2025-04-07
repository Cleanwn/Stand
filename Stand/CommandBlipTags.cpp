#include "CommandBlipTags.hpp"

#include "AbstractPlayer.hpp"
#include "CommandPlayer.hpp"
#include "natives.hpp"
#include "NativeTableHooks.hpp"
#include "scrNativeCallContext.hpp"

namespace Stand
{
	CommandBlipTags::CommandBlipTags(CommandList* const parent)
		: CommandToggle(parent, LOC("BLIPTAGS"), CMDNAMES_OBF("bliptags"))
	{
	}

	void CommandBlipTags::onChange(Click& click)
	{
		CommandPlayer::blip_names = m_on;

		for (const auto& p : AbstractPlayer::listAll())
		{
			p.updateNameOnBlip();
		}

		CommandToggle::onChange(click);
	}

	void CommandBlipTags::onEnable(Click& click)
	{
		// SET_BLIP_NAME_TO_PLAYER_NAME
		NativeTableHooks::createNativeEntrypointOverride(0x127DE7B20C60A6A3, [](rage::scrNativeCallContext& ctx)
		{
			auto name = AbstractPlayer(ctx.getArg<Player>(1)).getNameWithFlags();
			HUD::BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
			HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(name.c_str());
			HUD::END_TEXT_COMMAND_SET_BLIP_NAME(ctx.getArg<Blip>(0));
		});
	}

	void CommandBlipTags::onDisable(Click& click)
	{
		NativeTableHooks::removeNativeEntrypointOverride(0x127DE7B20C60A6A3);
	}
}
