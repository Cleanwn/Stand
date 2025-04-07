#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandToggleEditionbound : public CommandToggle
	{
	public:
		using CommandToggle::CommandToggle;

		void onChange(Click& click) final;

		virtual bool doPermissionCheck(Click& click) const noexcept = 0;

		virtual void onChangeInner(Click& click);
		virtual void onEnableInner(Click& click);
		virtual void onDisableInner(Click& click);
	};
}
