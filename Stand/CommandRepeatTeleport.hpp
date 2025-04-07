#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandRepeatTeleport : public CommandAction
	{
	public:
		explicit CommandRepeatTeleport(CommandList* parent);

		void onClick(Click& click) final;
	};
}
