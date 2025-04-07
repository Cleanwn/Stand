#pragma once

#include "CommandReadonlyValueCopy.hpp"

namespace Stand
{
	class CommandSessionCodeView : public CommandReadonlyValueCopy
	{
	public:
		explicit CommandSessionCodeView(CommandList* parent)
			: CommandReadonlyValueCopy(parent, LOC("SESSCODE"), LANG_GET_W("GENWAIT"))
		{
		}

		void onClick(Click& click) final
		{
			if (parent->as<CommandSessionCode>()->hasSessionCode())
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
			setValue(parent->as<CommandSessionCode>()->getSessionCodeForUser());
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}
	};
}
