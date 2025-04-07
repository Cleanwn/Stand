#include "CommandListHistoricPlayers.hpp"

#include <map>

#include "CommandHistoricPlayer.hpp"
#include "CommandLink.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandListHistoricPlayers::CommandListHistoricPlayers(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const commandflags_t flags)
		: CommandList(parent, std::move(menu_name), std::move(command_names), std::move(help_text), flags)
	{
	}

	void CommandListHistoricPlayers::populate()
	{
		const auto players = collectPlayers();
		std::map<std::string, CommandList*> note_map{};
		for (const auto& cmd : players)
		{
			auto simple_note = cmd->player->note;
			StringUtils::simplify(simple_note);
			if (simple_note.empty())
			{
				continue;
			}
			auto entry = note_map.find(simple_note);
			if (entry == note_map.end())
			{
				note_map.emplace(std::move(simple_note), nullptr);
			}
			else if (entry->second == nullptr)
			{
				entry->second = this->createChild<CommandList>(LIT(cmd->player->note), {}, NOLABEL, CMDFLAGS_LIST | CMDFLAG_FEATURELIST_SKIP);
			}
		}
		for (const auto& cmd : players)
		{
			auto simple_note = cmd->player->note;
			StringUtils::simplify(simple_note);
			auto entry = note_map.find(simple_note);
			if (entry != note_map.end() && entry->second != nullptr)
			{
				entry->second->createChild<CommandLink>(cmd);
			}
			else
			{
				this->createChild<CommandLink>(cmd);
			}
		}
	}

	void CommandListHistoricPlayers::onActiveListUpdate()
	{
		if (isThisOrSublistCurrentMenuListInTab())
		{
			if (!was_active)
			{
				was_active = true;
				resetChildren();
				populate();
				processChildrenUpdate();
			}
		}
		else
		{
			was_active = false;
		}
	}
}
