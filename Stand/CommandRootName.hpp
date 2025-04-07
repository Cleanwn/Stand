#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
	class CommandRootName : public CommandListSelect
	{
	public:
		inline static CommandRootName* instance;

		explicit CommandRootName(CommandList* parent);

		void update();

		void onChange(Click& click, long long prev_value) final;

		std::string getState() const final;
		std::string getDefaultState() const final;
		void setState(Click& click, const std::string& state) final;
	};
}
