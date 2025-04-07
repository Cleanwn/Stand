#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandSvmBpOptim : public CommandToggle
	{
	private:
		void* og_op_0;
		void* og_op_2;

	public:
		explicit CommandSvmBpOptim(CommandList* parent);

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;
	};
}
