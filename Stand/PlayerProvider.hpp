#pragma once

#include <functional>
#include <vector>

#include <soup/macros.hpp>

#include "fwddecl.hpp"

#include "AbstractPlayer.hpp"
#include "PlayerExcludes.hpp"
#include "StringUtils.hpp"

#define PC_PTR this->resolveParent(COMMAND_LIST_PLAYER)->as<CommandListPlayer>()
#define PC_PTR_SINGLE this->resolveParent(COMMAND_LIST_PLAYER)->as<CommandPlayer>()
#define PP_PTR PC_PTR->pp
#define PP_PTR_SINGLE ((PlayerProviderSingle*)PC_PTR->pp)
#define DEF_PC const CommandListPlayer* const pc = PC_PTR;
#define DEF_PP const PlayerProvider* const pp = pc->pp;
#define DEF_P2 DEF_PC; DEF_PP;

namespace Stand
{
	class PlayerProvider
	{
	public:
		inline static PlayerExcludes excludes{};

		const bool single;

		explicit PlayerProvider(bool single)
			: single(single)
		{
		}

		virtual ~PlayerProvider() = default;

		[[nodiscard]] virtual std::string getCommandName() const = 0;

		[[nodiscard]] virtual std::vector<AbstractPlayer> getPlayers(const bool include_self) const = 0;
		[[nodiscard]] std::vector<AbstractPlayer> getPlayers() const;

		[[nodiscard]] AbstractPlayer getPlayer() const;

		[[nodiscard]] bool isUser() const;

		void toggleFiberBatch(CommandPhysical* const cmd, std::function<void(const std::vector<AbstractPlayer>&)>&& callback) const;
		void toggleFiberBatch(const std::string& english_command_path_from_player_command, std::function<void(const std::vector<AbstractPlayer>&)>&& callback) const;
		void toggleBatch(const std::string& english_command_path_from_player_command, std::function<void(const std::vector<AbstractPlayer>&)>&& callback) const;
	};
}
