#pragma once

#include "CommandReadonlyValueCopy.hpp"

#include "CommandSessionCode.hpp"

namespace Stand
{
	class CommandListSessionCode : public CommandReadonlyValueCopy
	{
	public:
		explicit CommandListSessionCode(CommandList* parent)
			: CommandReadonlyValueCopy(parent, LOC("SESSCODE"), LANG_GET_W("GENWAIT"))
		{
		}

		void onClick(Click& click) final
		{
			if (CommandSessionCode::hasSessionCode(parent->as<CommandListSession>()->getGsinfo()))
			{
				CommandReadonlyValueCopy::onClick(click);
			}
			else
			{
				click.uwotm8();
			}
		}

		void onTickInGameViewport() final
		{
			setValue(CommandSessionCode::getSessionCodeForUser(
				parent->as<CommandListSession>()->getSessionType(),
				parent->as<CommandListSession>()->getGsinfo()
			));
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}
	};
}
