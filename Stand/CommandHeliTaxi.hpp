#pragma once

#include "CommandTextslider.hpp"

namespace Stand
{
	class CommandHeliTaxi : public CommandTextslider
	{
	public:
		explicit CommandHeliTaxi(CommandList* const parent)
			: CommandTextslider(parent, LOC("AM_HELI_TAXI"), { CMDNAME("helipickup"), CMDNAME("helitaxi"), CMDNAME("requesthelipickup") }, NOLABEL, {
				{ -1, LOC("AM_HELI_TAXI_0") },
				{ 1, LOC("AM_HELI_TAXI_1") },
			})
		{
		}

		void onClick(Click& click) final
		{
			ensureScriptThread(click, [value{ (int)this->value }]()
			{
				ScriptGlobal(GLOBAL_AM_HELI_TAXI).set<BOOL>(TRUE);
				ScriptGlobal(MPGlobalsAmbience_iRequestedHeliByPA).set<int>(value); // -1 = Maverick, > -1 = Supervolito
			});
		}
	};
}
