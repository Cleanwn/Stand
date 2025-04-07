#pragma once

#include "CommandPlayerListPlayers.hpp"

namespace Stand
{
	class CommandPlayerListPlayersShortcutItem : public CommandInlinePlayerListItem
	{
	public:
		using CommandInlinePlayerListItem::CommandInlinePlayerListItem;

		std::wstring getCommandSyntax() const final;

		void onClick(Click& click) final;
	};

	class CommandPlayerListPlayersShortcut : public CommandPlayerListPlayers<CommandPlayerListPlayersShortcutItem>
	{
	public:
		using Base = CommandPlayerListPlayers<CommandPlayerListPlayersShortcutItem>;

		using Base::Base;

		void onClick(Click& click) final;

		[[nodiscard]] virtual Label getInvalidityReason(const PlayerProvider* blamed_on) const;

		[[nodiscard]] virtual std::string getChild(AbstractPlayer& blamed_on, const std::string& target) const = 0;
	};
}
