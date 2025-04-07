#pragma once

#include "CommandAction.hpp"
#include "CommandOnPlayer.hpp"
#include "CommandListWithOnTickAsActiveList.hpp"

#include "Exceptional.hpp"
#include "Gui.hpp"
#include "PlayerProvider.hpp"

namespace Stand
{
	class CommandInlinePlayerListItem : public CommandAction
	{
	public:
		const PlayerProvider* const pp;

		explicit CommandInlinePlayerListItem(CommandList* const parent, const PlayerProvider* const pp, Label&& menu_name, Label&& help_text)
			: CommandAction(parent, std::move(menu_name), {}, std::move(help_text), CMDFLAGS_ACTION | CMDFLAG_FEATURELIST_SKIP), pp(pp)
		{
		}
	};

	template <class T> // T must extend CommandInlinePlayerListItem
	class CommandInlinePlayerList : public CommandOnPlayer<CommandListWithOnTickAsActiveList<CommandList>>
	{
	public:
		bool single_only;

		explicit CommandInlinePlayerList(CommandList* const parent, Label&& menu_name, bool single_only = false)
			: CommandOnPlayer(parent, std::move(menu_name)), single_only(single_only)
		{
			populate();
		}

	private:
		void populate()
		{
			if (g_gui.player_list != nullptr)
			{
				for (const auto& command : g_gui.player_list->children)
				{
					if (command && command->type == COMMAND_LIST_PLAYER)
					{
						auto* const pc = ((CommandListPlayer*)command.get());

						if (pc->isSingle() || !single_only)
						{
							createChild<T>(pc->pp, Label(pc->menu_name), Label(pc->help_text));
						}
					}
				}
			}
		}

	public:
		void onTickAsActiveList() final
		{
			bool identical_to_player_list = true;
			auto children_i = children.begin();
			for (auto& _command : g_gui.player_list->children)
			{
				if (_command->type != COMMAND_LIST_PLAYER)
				{
					continue;
				}
				if (children_i == children.end())
				{
					identical_to_player_list = false;
					break;
				}
				auto* child = (CommandPhysical*)children_i->get();
				auto* command = (CommandListPlayer*)_command.get();
				if (child->menu_name != command->menu_name || child->help_text != command->help_text)
				{
					identical_to_player_list = false;
					break;
				}
				children_i++;
			}
			if (!identical_to_player_list)
			{
				EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
				children.clear();
				populate();
				EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
				processChildrenUpdate();
			}
		}
	};

	template <typename T>
	using CommandPlayerListPlayers = CommandOnPlayer<CommandInlinePlayerList<T>>;
}
