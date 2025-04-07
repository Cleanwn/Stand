#pragma once

#include "CommandListSelect.hpp"

#include "eJoinResponseCode.hpp"

namespace Stand
{
	class CommandBjMessage : public CommandListSelect
	{
	public:
		explicit CommandBjMessage(CommandList* const parent)
			: CommandListSelect(parent, LOC("MSG"), {}, LOC("GEN_H_HOST"), {
				{ RESPONSE_DENY_UNKNOWN, LOC("BJMSG_BUG"), {}, LIT(Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("HUD_MPINVITEBAIL"))) },
				{ RESPONSE_DENY_INCOMPATIBLE_ASSETS, LOC("BJMSG_ASS"), {}, LIT(Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("BAIL_COMPATASSE"))) },
				{ RESPONSE_DENY_WRONG_VERSION, LOC("BJMSG_TU"), {}, LIT(Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("BAIL_WRNGVER"))) },
				{ RESPONSE_DENY_AIM_PREFERENCE, LOC("BJMSG_TAR"), {}, LIT(Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("BAIL_AIMPREF"))) },
				{ RESPONSE_DENY_REPUTATION, LOC("BJMSG_BAR"), {}, LIT(Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("HUD_REPUTATION"))) },
				{ RESPONSE_DENY_CHEATER, LOC("BJMSG_C"), {}, LIT(Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("BAIL_CHEAT"))) },
				{ RESPONSE_DENY_POOL_NORMAL, LOC("BJMSG_BS"), {}, LIT(Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("BAIL_NORMBAD"))) },
				{ RESPONSE_DENY_DATA_HASH, LOC("BJMSG_DLC"), {}, LIT(Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("BAIL_DATA_HASH"))) },
				{ RESPONSE_DENY_DIFFERENT_BUILD, LOC("BJMSG_BT"), {}, LIT(Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("BAIL_DIFFBUILD"))) },
				{ RESPONSE_DENY_DIFFERENT_CONTENT_SETTING, LOC("BJMSG_CONT"), {}, LIT(Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("BAIL_DIFFCONT"))) },
				{ RESPONSE_DENY_POOL_BAD_SPORT, LOC("BJMSG_BSO"), {}, LIT(Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("BAIL_BADSPONLY"))) },
				{ RESPONSE_DENY_POOL_CHEATER, LOC("BJMSG_CO"), {}, LIT(Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("BAIL_CHEATONLY"))) },
				{ RESPONSE_DENY_PREMIUM, LOC("BJMSG_PR"), {}, LIT(Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("TRAN_JOINPRF"))) },
				{ RESPONSE_DENY_FAILED_TO_ESTABLISH, LOC("BJMSG_CON"), {}, LIT(Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("HUD_ESTAB"))) },
				{ RESPONSE_DENY_PRIVATE_ONLY, LOC("BJMSG_PRIV"), {}, LIT(Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("BAIL_PRIVONLY"))) },
				{ RESPONSE_DENY_NOT_FRIEND, LOC("BJMSG_F"), {}, LIT(Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("BAIL_FRIENDSONLY"))) },
				{ RESPONSE_DENY_BLACKLISTED, LOC("BJMSG_VK"), {}, LIT(Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("BAIL_BLACKLIST"))) },
				{ RESPONSE_DENY_BLOCKING, LOC("BJMSG_ID"), {}, LIT(Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("BAIL_BLOCKING"))) },
				{ RESPONSE_DENY_NOT_JOINABLE, LOC("BJMSG_POOF"), {}, LIT(Util::GET_LABEL_TEXT_no_replacements(ATSTRINGHASH("BAIL_SESSGONE"))) },
			}, RESPONSE_DENY_UNKNOWN)
		{
			static_assert(RESPONSE_DENY_UNKNOWN == 1); // needs to match default value of g_hooking.host_bj_message
		}

		void onChange(Click& click, long long prev_value) final
		{
			g_hooking.host_bj_message = (int32_t)value;
		}
	};
}
