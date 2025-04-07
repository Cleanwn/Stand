#pragma once

#include "CommandList.hpp"

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandDisableInputs : public CommandList
	{
	public:
		explicit CommandDisableInputs(CommandList* const parent);
	};

	class CommandDisableInput : public CommandToggle
	{
	public:
		const int input;

		explicit CommandDisableInput(CommandList* const parent, int input);

		void onEnable(Click& click) final;
	};

	class CommandDisableInputInvert : public CommandToggle
	{
	public:
		inline static CommandDisableInputInvert* instance;

		explicit CommandDisableInputInvert(CommandList* const parent);

		void onEnable(Click& click) final;
	};
}
