#pragma once

#include "CommandSaveCommandBoxPreset.hpp"

#include "CommandListSelect.hpp"
#include "CommandSlider.hpp"
#include "CommandToggle.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class CommandLockWeaponsSavePreset : public CommandSaveCommandBoxPreset
	{
	public:
		using CommandSaveCommandBoxPreset::CommandSaveCommandBoxPreset;

		bool performSave(Click& click, std::ofstream& out) const final
		{
			this->parent->parent->recursiveForEachChild([&out, this](const std::unique_ptr<Command>& c)
			{
				auto physical = c->getPhysical();

				if (!physical || physical->command_names.empty())
				{
					return true;
				}

				const auto cmdname = cmdNameToUtf8(physical->command_names.at(0));

				if (c->isToggle())
				{
					out << fmt::format("{} {}\n", cmdname, c->as<CommandToggle>()->m_on ? "on" : "off");
				}
				else if (c->isSlider())
				{
					out << fmt::format("{} {}\n", cmdname, std::to_string(c->as<CommandSlider>()->value));
				}
				else if (c->isListNameshare())
				{
					out << fmt::format("{} {}\n", cmdname, cmdNameToUtf8(c->as<CommandListSelect>()->getCurrentValueCommand()->command_suffixes.at(0)));
				}

				return true;
			});

			return true;
		}
	};
}