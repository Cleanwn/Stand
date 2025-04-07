#pragma once

#include "CommandPhysical.hpp"

namespace Stand
{
	class CommandDivider : public CommandPhysical
	{
	public:
		inline static bool selectable = false;

		explicit CommandDivider(CommandList* const parent, Label&& menu_name);

		void preDetach() final;

		void onFocus(ThreadContext thread_context, Direction momentum) override;
	};
}
