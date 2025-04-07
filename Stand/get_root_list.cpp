#include "get_root_list.hpp"

#include <soup/ObfusString.hpp>

#include "Auth.hpp"
#include "CommandAction.hpp"
#include "CommandLambdaAction.hpp"
#include "CommandReadonlyLink.hpp"
#include "Exceptional.hpp"
#include "Gui.hpp"
#include "is_session.hpp"
#include "Renderer.hpp"
#include "RootNameMgr.hpp"
#include "Util.hpp"

#include "CommandTabSelf.hpp"
#include "CommandTabVehicle.hpp"
#include "CommandTabOnline.hpp"
#include "CommandTabPlayers.hpp"
#include "CommandTabWorld.hpp"
#include "CommandTabGame.hpp"
#include "CommandTabStand.hpp"

#include "CommandDummy.hpp"
#include "CommandHotkeyAdd.hpp"
#include "CommandHotkeyRemove.hpp"
#include "CommandWait.hpp"

namespace Stand
{
	static void activatePaste_onClick(Click& click) // OBFUS!
	{
		if (g_auth.activation_key_to_try.empty())
		{
			click.setResponse(LOC("ACTVTE_ERR_F2"));
		}
		else if (g_auth.activation_key_to_try == g_auth.crash_key)
		{
			terminate();
		}
		else
		{
			click.setResponse(LOC("ACTVTE_STRT2"));
		}
	}

	template <typename T>
	static T* createTabExceptional(CommandList* const root)
	{
		T* const tab = root->createChild<T>();
		__try
		{
			tab->populate();
		}
		__EXCEPTIONAL()
		{
		}
		return tab;
	}

	CommandList* get_root_list() // OBFUS!
	{
		// Determine root state
		if (g_gui.minimal)
		{
			g_gui.root_state = GUI_MINIMAL;
		}
		else
		{
			g_gui.root_state = GUI_FREEONLINE;
			if (!g_gui.killswitched)
			{
				if (g_auth.activation_key_to_try.empty()
					|| g_auth.verifyPermSig() == LICPERM_FREE
					)
				{
					g_auth.license_permissions = LICPERM_FREE;
				}
				else if (g_gui.unlock_recover_state != 0)
				{
					g_gui.setApplicableNonFreeRootStateValue();
				}
				if (g_gui.root_state == GUI_FREEONLINE && !is_session_or_transition_active())
				{
					g_gui.root_state = GUI_FREE;
				}
			}
		}
		auto* root = new CommandList(nullptr, LIT(RootNameMgr::getCurrentRootNameForState()));
		// Populate
		if (g_gui.shouldRootStateShowLicensePrompt())
		{
			auto activate = root->createChild<CommandTab>(TAB_NONE, LOC("ACTVTE"), CMDNAMES_OBF("tactivate"));

			activate->createChild<CommandLambdaAction>(LOC("ACTVTE_PSTE2"), {}, LOC("ACTVTE_H"), &activatePaste_onClick);
			activate->createChild<CommandReadonlyLink>(LOC("ACTVTE_RGST"), soup::ObfusString("https://stand.sh/account/register"));
		}
		if (g_gui.isRootStateFull())
		{
			createTabExceptional<CommandTabSelf>(root);
			createTabExceptional<CommandTabVehicle>(root);
		}
		createTabExceptional<CommandTabOnline>(root);
		if (g_gui.doesRootStateAllowPlayerList())
		{
			g_gui.player_list = createTabExceptional<CommandTabPlayers>(root);
		}
		if (g_gui.isRootStateFull())
		{
			createTabExceptional<CommandTabWorld>(root);
		}
		createTabExceptional<CommandTabGame>(root);
		if (g_gui.root_state != GUI_MINIMAL)
		{
			createTabExceptional<CommandTabStand>(root);

			g_gui.pseudo_commands->createChild<CommandDummy>();
			g_gui.pseudo_commands->createChild<CommandHotkeyAdd>();
			g_gui.pseudo_commands->createChild<CommandHotkeyRemove>();
			g_gui.pseudo_commands->createChild<CommandWait>();
		}
		return root;
	}
}
