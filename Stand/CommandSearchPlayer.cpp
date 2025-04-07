#include "CommandSearchPlayer.hpp"

#include <soup/string.hpp>

#include "CommandDivider.hpp"
#include "CommandHistoricPlayer.hpp"
#include "CommandLink.hpp"
#include "PlayerHistory.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandSearchPlayer::CommandSearchPlayer(CommandList* parent)
		: CommandSearch(parent, LOC("SRCH"), { CMDNAME("findplayer") }, LOC("PYLFND_H2"))
	{
	}

	void CommandSearchPlayer::doSearch(std::string&& arg)
	{
		StringUtils::simplifyCommandName(arg);
		std::vector<Command*> note_results{};
		std::vector<Command*> name_results{};
		std::vector<Command*> rid_results{};
		for (auto i = parent->children.begin() + PlayerHistory::list_offset; i != parent->children.end(); i++)
		{
			std::string note{ ((CommandHistoricPlayer*)i->get())->player->note };
			std::string name{ ((CommandHistoricPlayer*)i->get())->player->name };
			StringUtils::simplifyCommandName(note);
			StringUtils::simplifyCommandName(name);
			if (note.find(arg) != std::string::npos)
			{
				note_results.emplace_back(i->get());
			}
			if (name.find(arg) != std::string::npos)
			{
				name_results.emplace_back(i->get());
			}
		}
		std::optional<uint64_t> rid = soup::string::toInt<uint64_t, soup::string::TI_FULL>(arg);
		if (!rid.has_value())
		{
			if (arg.substr(0, 2) == "0x")
			{
				arg.erase(0, 2);
			}
			rid = soup::string::hexToInt<uint64_t>(arg);
		}
		if (rid.has_value())
		{
			for (auto i = parent->children.begin() + PlayerHistory::list_offset; i != parent->children.end(); i++)
			{
				if (rid.value() == ((CommandHistoricPlayer*)i->get())->player->rid)
				{
					rid_results.emplace_back(i->get());
				}
			}
		}

		uint8_t result_types = 0;
		if (!note_results.empty())
		{
			++result_types;
		}
		if (!name_results.empty())
		{
			++result_types;
		}
		if (!rid_results.empty())
		{
			++result_types;
		}

		if (!note_results.empty())
		{
			if (result_types > 1)
			{
				createChild<CommandDivider>(LOC("PLYNOTE"));
			}
			for (const auto& result : note_results)
			{
				createChild<CommandLink>(result);
			}
		}

		if (!name_results.empty())
		{
			if (result_types > 1)
			{
				createChild<CommandDivider>(LOC("NME"));
			}
			for (const auto& result : name_results)
			{
				createChild<CommandLink>(result);
			}
		}

		if (!rid_results.empty())
		{
			if (result_types > 1)
			{
				createChild<CommandDivider>(LIT("Rockstar ID"));
			}
			for (const auto& result : rid_results)
			{
				createChild<CommandLink>(result);
			}
		}
	}
}
