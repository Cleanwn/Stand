#pragma once

#include "CommandPlayerToggle.hpp"

#include "CommandSlider.hpp"

namespace Stand
{
	class CommandPlayerLoopBounty : public CommandPlayerToggle
	{
	public:
		CommandToggle* self = nullptr;
		CommandSlider* value = nullptr;

		explicit CommandPlayerLoopBounty(CommandList* const parent, CommandSlider* value)
			: CommandPlayerToggle(parent, LOC("LOOPBNTY"), CMDNAMES_OBF("loopbounty")), value(value)
		{
		}
		
		void onEnable(Click& click) final
		{
			PP_PTR->toggleBatch(getPathEnFromParent(COMMAND_LIST_PLAYER), [this](const std::vector<AbstractPlayer>& players)
			{
				auto value = (int)this->value->value;

				if (self && self->m_on && !g_player.hasBounty())
				{
					g_player.giveBounty(value);
				}

				for (const auto& p : players)
				{
					if (!p.hasBounty())
					{
						p.giveBounty(value);
					}
				}
			});
		}
	};
}