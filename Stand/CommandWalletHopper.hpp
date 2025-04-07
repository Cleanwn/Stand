#pragma once

#include "CommandSlider.hpp"

#include "get_current_time_millis.hpp"
#include "Script.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandWalletHopper : public CommandSlider
	{
	public:
		explicit CommandWalletHopper(CommandList* const parent)
			: CommandSlider(parent, LOC("AUTODEPO"), { CMDNAME("wallethopper"), CMDNAME("autodepositwallet") }, NOLABEL, 0, 1000000, 0, 50)
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			if (value != 0)
			{
				const int value = this->value;
				registerScriptTickEventHandler(click, [this, value]()
				{
					static long long last_transaction = 0;
					const int char_slot = Util::get_char_slot_no_notify();
					if (char_slot != -1)
					{
						const int wallet = MONEY::NETWORK_GET_VC_WALLET_BALANCE(char_slot);
						if (wallet >= value)
						{
							const time_t time = get_current_time_millis();
							if (last_transaction + 3000 < time)
							{
								if (NETSHOPPING::NET_GAMESERVER_TRANSFER_WALLET_TO_BANK(char_slot, wallet))
								{
									Util::toast(LANG_FMT("WLTHPPR_T", wallet));
								}
								last_transaction = time;
							}
						}
					}
					return value == this->value;
				});
			}
		}
	};
}
