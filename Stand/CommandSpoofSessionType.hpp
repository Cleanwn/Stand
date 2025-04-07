#pragma once

#include "CommandListSelect.hpp"

#include "gsType.hpp"
#include "SessionSpoofing.hpp"
#include "Tunables.hpp"

namespace Stand
{
	class CommandSpoofSessionType : public CommandListSelect
	{
	public:
		inline static CommandSpoofSessionType* instance;

		explicit CommandSpoofSessionType(CommandList* parent)
			: CommandListSelect(parent, LOC("SESSTYP"), {}, LOC("SESSTYP_H"), {
				{-1, LOC("NSPFD")},
				{GS_SOLO, LOC("SHTONLSTS_ST_S")},
				{GS_INVITE, LOC("SHTONLSTS_ST_I")},
				{GS_CLOSED_FRIEND, LOC("SHTONLSTS_ST_CF")},
				{GS_CLOSED_CREW, LOC("SHTONLSTS_ST_CC")},
				{GS_CREW, LOC("SHTONLSTS_ST_C2")},
				{GS_PUBLIC, LOC("SHTONLSTS_ST_P")},
				{69, LOC("NVLD")},
			}, -1, CMDFLAGS_LIST_SELECT | CMDFLAG_FEATURELIST_ULTIMATE)
		{
			instance = this;
		}

		void onChange(Click& click, long long prev_value) final
		{
			if (this->value == -1)
			{
				SessionSpoofing::onDisable();
			}
			else
			{
				if (g_tunables.getBool(TUNAHASH("DISABLE_SESSION_SPOOFING")))
				{
					click.setResponse(LOC("CMDDISA"));
					return onChangeImplUnavailable(click, prev_value);
				}
				if (!click.isUltimateEdition())
				{
					return onChangeImplUnavailable(click, prev_value);
				}
			}
		}
	};
}
