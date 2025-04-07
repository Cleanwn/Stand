#pragma once

#include <CommandListPlayer.hpp>

#include "PlayerProviderAll.hpp"

namespace Stand
{
	class CommandAllPlayers : public CommandListPlayer
	{
	private:
		const PlayerProviderAll _player_provider;

	public:
		explicit CommandAllPlayers(CommandList* const parent);

		[[nodiscard]] std::string getPlayerName() const final;
		[[nodiscard]] CommandName getCommandNamesSuffix() const final;
	};
}
