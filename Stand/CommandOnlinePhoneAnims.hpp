#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandOnlinePhoneAnims : public CommandToggle
	{
	public:
		explicit CommandOnlinePhoneAnims(CommandList* const parent);

		void onEnable(Click& click) final;

	private:
		bool was_on = false;
		void togglePhoneAnims(bool on);
	};
}
