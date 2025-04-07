#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandDisableInsurancePay : public CommandToggle
	{
	public:
		explicit CommandDisableInsurancePay(CommandList* const parent)
			: CommandToggle(parent, LOC("NINSURPAY"), combineCommandNames(CMDNAMES("no", "block", "disable"), CMDNAME("insurancepay")), LOC("NINSURPAY_H"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				ScriptGlobal(GLOBAL_DISABLE_PV_INSURANCE_PAY).set<BOOL>(m_on);
				return m_on;
			});
		}
	};
}
