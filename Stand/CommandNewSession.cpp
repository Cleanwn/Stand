#include "CommandNewSession.hpp"

#include "CommandActionSuffixLambda.hpp"
#include "CommandDivider.hpp"
#include "CommandToggle.hpp"
#include "is_session.hpp"
#include "Script.hpp"
#include "TransitionHelper.hpp"

namespace Stand
{
	CommandNewSession::CommandNewSession(CommandList* const parent)
		: CommandListNameshare(parent, LOC("NWSESS"), { CMDNAME("go") })
	{
		select_character = createChild<CommandToggle>(LOC("NWSESS_CHAR"));
		createChild<CommandDivider>(LOC("NWSESS"));
		createChild<CommandActionSuffixLambda>(LOC("NWSESS_SP"), { CMDNAME("solopublic") }, NOLABEL, [this](Click& click)
		{
			if (click.isBasicEdition())
			{
				onChildClick(click, NEW_PUBLIC);
			}
		});
		createChild<CommandActionSuffixLambda>(LOC("NWSESS_IO"), { CMDNAME("inviteonly"), CMDNAME("invonly") }, NOLABEL, [this](Click& click)
		{
			onChildClick(click, INVITE_ONLY);
		});
		createChild<CommandActionSuffixLambda>(LOC("NWSESS_CC"), { CMDNAME("closedcrew") }, NOLABEL, [this](Click& click)
		{
			onChildClick(click, CLOSED_CREW);
		});
		createChild<CommandActionSuffixLambda>(LOC("NWSESS_C"), { CMDNAME("crew") }, NOLABEL, [this](Click& click)
		{
			onChildClick(click, CREW);
		});
		createChild<CommandActionSuffixLambda>(LOC("NWSESS_CF"), { CMDNAME("closedfriend") }, NOLABEL, [this](Click& click)
		{
			onChildClick(click, CLOSED_FRIEND);
		});
		createChild<CommandActionSuffixLambda>(LOC("NWSESS_S"), { CMDNAME("solo") }, NOLABEL, [this](Click& click)
		{
			onChildClick(click, SOLO);
		});
		createChild<CommandActionSuffixLambda>(LOC("NWSESS_FF"), { CMDNAME("joinafriend") }, NOLABEL, [this](Click& click)
		{
			onChildClick(click, JOIN_A_FRIEND);
		});
		createChild<CommandActionSuffixLambda>(LOC("NWSESS_FC"), { CMDNAME("joincrew") }, NOLABEL, [this](Click& click)
		{
			onChildClick(click, JOIN_CREW);
		});
		// As of b3351, attempting to matchmake for a public session seems to crash.
		/*createChild<CommandActionSuffixLambda>(LOC("NWSESS_FP"), { CMDNAME("public") }, NOLABEL, [this](Click& click)
		{
			onChildClick(click, FIND_PUBLIC);
		});
		createChild<CommandActionSuffixLambda>(LOC("NWSESS_NP"), { CMDNAME("newpublic") }, LOC("NWSESS_NP_H"), [this](Click& click)
		{
			onChildClick(click, STAND_NEW_PUBLIC);
		});*/
		createChild<CommandActionSuffixLambda>(LOC("NWSESS_CTOR"), { CMDNAME("creatormode") }, NOLABEL, [this](Click& click)
		{
			onChildClick(click, STAND_CREATOR_MODE);
		});
		/*createChild<CommandActionSuffixLambda>(LOC("NWSESS_SCTV"), { CMDNAME("sctv") }, NOLABEL, [this](Click& click)
		{
			onChildClick(click, STAND_SCTV);
		});*/
	}

	void CommandNewSession::onChildClick(Click& click, SessionType value)
	{
		ensureYieldableScriptThread(click, [this, value]() mutable
		{
			if (select_character->m_on)
			{
				if (value == STAND_NEW_PUBLIC)
				{
					value = FIND_PUBLIC;
				}
				TransitionHelper::doTransition(STAND_CHARACTER_SELECTION);
				Script::current()->yield(10000);
			}
			if (value == STAND_NEW_PUBLIC)
			{
				if (is_session_started())
				{
					TransitionHelper::doTransition(INVITE_ONLY);
					Script::current()->yield(2000);
				}
				TransitionHelper::doTransition(FIND_PUBLIC);
			}
			else
			{
				TransitionHelper::doTransition(value);
			}
		});
	}
}
