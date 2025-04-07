#pragma once

#include <fmt/core.h>

#include "CommandListPlayer.hpp"
#include "lang.hpp"
#include "PlayerProvider.hpp"

namespace Stand
{
	template <class T>
	class CommandOnPlayer : public T
	{
	protected:
		using T::T;

	public:
		[[nodiscard]] const CommandListPlayer* getPlayerCommand() const
		{
			return PC_PTR;
		}

		[[nodiscard]] const PlayerProvider* getPlayerProvider() const
		{
			return getPlayerCommand()->pp;
		}

		[[nodiscard]] compactplayer_t getPlayer() const
		{
			return getPlayerProvider()->getPlayer();
		}

		[[nodiscard]] std::string getPlayerName() const
		{
			return getPlayerCommand()->getPlayerName();
		}

		Label getActivationName() const override
		{
			return getActivationNameImplOnPlayer(this->menu_name.getLocalisedUtf8());
		}

	protected:
		Label getActivationNameImplOnPlayer(const std::string& localised_utf8) const
		{
			return LIT(LANG_FMT("PLY_A", fmt::arg("command", localised_utf8), fmt::arg("player", getPlayerName())));
		}

		Label getActivationNameImplCombineWithParent(const char* joiner) const
		{
			return getActivationNameImplOnPlayer(std::string(reinterpret_cast<const T*>(this)->parent->menu_name.getLocalisedUtf8()).append(joiner).append(this->menu_name.getLocalisedUtf8()));
		}
	};
}
