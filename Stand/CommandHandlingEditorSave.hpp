#pragma once

#include "CommandSaveCommandBoxPreset.hpp"

#include "AbstractEntity.hpp"
#include "CommandSliderFloat.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class CommandHandlingEditorSave : public CommandSaveCommandBoxPreset
	{
	public:
		using CommandSaveCommandBoxPreset::CommandSaveCommandBoxPreset;

		bool preCheck(Click& click) override
		{
			if (!g_player_veh.isValid())
			{
				click.setResponse(LOC("NOVEHFND"));
				return false;
			}

			return true;
		}

		bool performSave(Click& click, std::ofstream& out) const final
		{
			this->parent->parent->recursiveForEachChild([&out](const std::unique_ptr<Command>& c)
			{
				if (c->isSlider())
				{
					const auto cmd = c->as<CommandSlider>();

					cmd->onTickInGameViewport();

					if (!cmd->isNotApplicable())
					{
						const auto cmdname = cmdNameToUtf8(cmd->command_names.at(0));

						if (c->isT<CommandSliderFloat>())
						{
							const auto fcmd = cmd->as<CommandSliderFloat>();
							out << fmt::format("{} {}\n", cmdname, StringUtils::utf16_to_utf8(fcmd->formatNumber(fcmd->value, true)));
						}
						else
						{
							out << fmt::format("{} {}\n", cmdname, cmd->value);
						}
					}
				}

				return true;
			});

			return true;
		}
	};
}