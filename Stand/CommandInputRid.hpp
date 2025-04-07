#pragma once

#include "CommandInput.hpp"

namespace Stand
{
	class CommandInputRid : public CommandInput
	{
	protected:
		using CommandInput::CommandInput;

	public:
		std::wstring getCommandSyntax() const final;

		void onCommand(Click& click, std::wstring& args) final;

		[[nodiscard]] int64_t getRid() const noexcept;

	protected:
		virtual void onCommand(Click& click, int64_t rid) = 0;
	};
}
