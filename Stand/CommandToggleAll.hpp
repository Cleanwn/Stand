#pragma once

#include "CommandToggle.hpp"

#include "CommandList.hpp"

namespace Stand
{
	class CommandToggleAll : public CommandToggle
	{
	public:
		explicit CommandToggleAll(CommandList* parent, std::vector<CommandName>&& command_names = {})
			: CommandToggle(parent, LOC("ALL"), std::move(command_names), NOLABEL, false, CMDFLAGS_TOGGLE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS)
		{
		}

	private:
		void forEachManagedToggle(std::function<void(CommandToggleNoCorrelation*)>&& func) const
		{
			parent->forEachChild([this, func{ std::move(func) }](Command* cmd)
			{
				if (cmd->isT<CommandToggleNoCorrelation>() && cmd != this)
				{
					func(cmd->as<CommandToggleNoCorrelation>());
				}
			});
		}

	public:
		void onCommand(Click& click, std::wstring& args) final
		{
			forEachManagedToggle([](CommandToggleNoCorrelation* cmd)
			{
				if (!cmd->isInViewport())
				{
					cmd->onTickInGameViewport();
				}
			});
			if (!isInViewport())
			{
				onTickInGameViewport();
			}
			return CommandToggle::onCommand(click, args);
		}

		void onChange(Click& click) final
		{
			if (!click.isBulk())
			{
				Click click_ = click.deriveAuto();
				const bool on = this->m_on;
				forEachManagedToggle([&click_, on](CommandToggleNoCorrelation* cmd)
				{
					cmd->setStateBool(click_, on);
				});
			}
		}

		void onTickInGameViewport() final
		{
			size_t ttl = 0;
			size_t on = 0;
			forEachManagedToggle([&](CommandToggleNoCorrelation* cmd)
			{
				++ttl;
				if (cmd->m_on)
				{
					++on;
				}
			});
			setOnIndication((ttl == on));
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}
	};
}
