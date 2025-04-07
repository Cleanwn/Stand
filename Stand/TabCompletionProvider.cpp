#include "TabCompletionProvider.hpp"

#include <soup/base.hpp>

#include "StringUtils.hpp"

namespace Stand
{
	bool TabCompletionProvider::canHandleKeyUp() const noexcept
	{
		return message_length_when_key_was_first_down != -1;
	}

	void TabCompletionProvider::onKeyDown()
	{
		if (message_length_when_key_was_first_down == -1)
		{
			message_length_when_key_was_first_down = tbif.getLength();
		}
	}

	void TabCompletionProvider::onKeyUp()
	{
#ifdef STAND_DEBUG
		SOUP_ASSERT(canHandleKeyUp());
#endif
		if (tbif.getLength() > message_length_when_key_was_first_down)
		{
			tbif.remove(tbif.getLength() - message_length_when_key_was_first_down);
		}
		message_length_when_key_was_first_down = -1;
		std::wstring draft = tbif.getText();
		size_t last_word_off = draft.find_last_of(L' ');
		if (last_word_off == std::string::npos)
		{
			last_word_off = 0;
		}
		else
		{
			last_word_off++;
		}
		std::wstring last_word = draft.substr(last_word_off);
		if (draft != last_draft)
		{
			tab_completions = getCompletions(std::move(last_word));
			tab_complete_index = 0;
		}
		else if (tab_complete_index >= tab_completions.size())
		{
			tab_complete_index = 0;
		}
		if (!tab_completions.empty())
		{
			tbif.remove(last_word.length());
			tbif.add(tab_completions.at(tab_complete_index++));
			last_draft = tbif.getText();
		}
	}
}

#include "AbstractPlayer.hpp"

namespace Stand
{
	std::vector<std::wstring> PlayerNameTabCompletionProvider::getCompletions(std::wstring&& prefix)
	{
		StringUtils::to_lower(prefix);

		std::vector<std::wstring> tab_completions{};
		for (const auto& p : AbstractPlayer::listAll())
		{
			std::wstring name = p.getNameW();
			if (prefix.length() <= name.length())
			{
				StringUtils::to_lower(name);
				if (name.substr(0, prefix.length()) == prefix)
				{
					tab_completions.emplace_back(p.getNameW());
				}
			}
		}
		return tab_completions;
	}
}

#include "CommandboxGrid.hpp"
#include "CommandExtraInfo.hpp"
#include "Exceptional.hpp"
#include "Gui.hpp"

namespace Stand
{
	std::vector<std::wstring> CommandNameTabCompletionProvider::getCompletions(std::wstring&& prefix)
	{
		StringUtils::to_lower(prefix);
#if COMPACT_COMMAND_NAMES
		CommandName prefix_cn = StringUtils::utf16_to_utf8(prefix);
#else
		const CommandName& prefix_cn = prefix;
#endif

		std::vector<std::wstring> tab_completions{};
		if (!prefix.empty())
		{
			size_t offset = 0;
			g_commandbox_grid.cache_mtx.lock();
			auto commands{ g_commandbox_grid.cache_data };
			if (g_commandbox_grid.cache_input.length() > prefix.length()
				&& g_commandbox_grid.cache_input.substr(g_commandbox_grid.cache_input.length() - prefix.length()) == prefix
				)
			{
				// Handle CommandIssuable::collapse_command_names, e.g. for "t g", offset = 1.
				offset = (g_commandbox_grid.cache_input.length() - prefix.length());
			}
			g_commandbox_grid.cache_mtx.unlock();

			// Handle CommandExtraInfo::collapse, e.g. for "getgun pis", offset = 6.
			if (commands.size() == 1)
			{
				CommandExtraInfo info{};
				CommandName command_name;
				EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
				if (auto cmd = commands.at(0).getPointer())
				{
					command_name = cmd->command_names.at(0);
					std::wstring args = prefix;
					cmd->getExtraInfo(info, args);
				}
				EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
				if (info.collapse)
				{
					offset = command_name.size();
					command_name.append(prefix_cn);
					commands = g_gui.findCommandsWhereCommandNameStartsWithAsWeakrefs(command_name);
				}
			}

			for (const auto& wr : commands)
			{
				if (auto cmd = wr.getPointer())
				{
					for (const auto& name : cmd->command_names)
					{
						if (name.substr(offset, prefix_cn.length()) == prefix_cn)
						{
							tab_completions.emplace_back(cmdNameToUtf16(name.substr(offset)));
						}
					}
				}
			}
		}
		return tab_completions;
	}
}
