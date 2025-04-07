#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
	class CommandHeader final : public CommandListSelect
	{
	public:
		explicit CommandHeader(CommandList* const parent);

		void onChange(Click& click, long long prev_value) final;

		void setState(Click& click, const std::string& state) final;

		bool applyValue() const;
	};
}
