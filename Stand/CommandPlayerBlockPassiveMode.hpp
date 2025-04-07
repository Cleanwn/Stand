#pragma once

#include "CommandPlayerToggle.hpp"

#include "tse.hpp"

namespace Stand
{
	class CommandPlayerBlockPassiveMode : public CommandPlayerToggle
	{
	public:
		explicit CommandPlayerBlockPassiveMode(CommandList* const parent)
			: CommandPlayerToggle(parent, LOC("NOPSSVE"), combineCommandNames(CMDNAMES_OBF("no", "block", "disable"), CMDNAMES_OBF("passivemode")))
		{
		}

		void onChange(Click& click) final
		{
			ensureScriptThread(click, [this]
			{
				for (const auto& p : PP_PTR->getPlayers())
				{
					uint64_t args[] = {
						Util::i32_to_u64_trickery(SE_SET_TERM_TARGET),
						g_player,
						(uint64_t)(BOOL)this->m_on
					};

					TSE(1 << p, args);
				}
			});
		}
	};
}