#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandQuickCtx final : public CommandList
	{
	public:
		inline static CommandQuickCtx* instance = nullptr;

		CommandPhysical* save_state;
		CommandPhysical* load_state;
		CommandPhysical* apply_default_state;
		CommandPhysical* apply_default_state_to_children;
		CommandPhysical* min;
		CommandPhysical* max;

		explicit CommandQuickCtx(CommandList* const parent);
		~CommandQuickCtx() final;
	};
}
