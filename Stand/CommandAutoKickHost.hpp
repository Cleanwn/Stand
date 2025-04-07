#pragma once

#include "CommandToggleUltimate.hpp"

#include "PlayerExcludes.hpp"

namespace Stand
{
	class CommandAutoKickHost : public CommandToggleUltimate
	{
	public:
		inline static CommandAutoKickHost* instance;

		PlayerExcludes excludes{ true, true, true };

		explicit CommandAutoKickHost(CommandList* parent);
		~CommandAutoKickHost();

		void onEnableInner(Click& click) final;
		void onDisableInner(Click& click) final;

		[[nodiscard]] bool canRemove(AbstractPlayer p) const;
	};
}
