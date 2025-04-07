#pragma once

#include "CommandListConcealer.hpp"

namespace Stand
{
	class CommandListPlayer : public CommandListConcealer
	{
	public:
		const PlayerProvider* const pp;

		explicit CommandListPlayer(CommandList* const parent, const PlayerProvider* pp, Label&& menu_name, std::vector<CommandName>&& command_names = {}, commandflags_t flags = CMDFLAGS_LIST_CONCEALER);
	protected:
		__declspec(noinline) void populate(compactplayer_t p);
	public:
		[[nodiscard]] commandflags_t getSublistFlags() const;

		void registerCommand(CommandPhysical* const cmd) const;
		void registerCommandNames(CommandPhysical* const cmd) const;

		[[nodiscard]] bool isSingle() const noexcept;
		[[nodiscard]] virtual std::string getPlayerName() const = 0;
		[[nodiscard]] virtual CommandName getCommandNamesSuffix() const = 0;

		void ensureOnlineAndWarnSelfharm(Command* command_for_warning, Click& click, std::function<void(ThreadContext)>&& callback) const;
	};
}
