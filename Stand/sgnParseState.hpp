#pragma once

#include <stack>

#include "script_program.hpp"

#include "sgnStackVar.hpp"

namespace ScriptGenie
{
	// THIS NAMESPACE IS (MORE OR LESS) A MIRROR OF https://github.com/calamity-inc/ScriptGenie

	struct ParseState
	{
		const rage::scrProgram* scr;
		uint32_t code_i = 0;
		std::stack<StackVar> stack;

		ParseState(const rage::scrProgram* scr)
			: scr(scr)
		{
		}

		[[nodiscard]] const uint8_t* getCodePtr() const noexcept;

		bool tick();
		void advance() noexcept;
	};
}
