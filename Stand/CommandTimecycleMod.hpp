#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
	class CommandTimecycleMod : public CommandListSelect
	{
	public:
		static inline bool shader_overriden = false;

		explicit CommandTimecycleMod(CommandList* const parent);

		void onChange(Click& click, long long prev_value) final;
	};
}
