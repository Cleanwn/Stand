#pragma once

#include "CommandPlayerActionValue.hpp"

namespace Stand
{
	class CommandPlayerNotify : public CommandPlayerActionValue
	{
	private:
		const uint64_t notify_hash;

	public:
		explicit CommandPlayerNotify(CommandList* const parent, uint64_t notify_hash, Label&& menu_name, std::vector<CommandName>&& command_names);

	protected:
		void onCommandWithValue(Click& click, long long value) final;
	};
}
