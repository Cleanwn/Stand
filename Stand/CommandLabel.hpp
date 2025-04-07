#pragma once

#include "CommandList.hpp"

#include <fmt/core.h>

#include "CommandEditLabel.hpp"
#include "CommandEditLabels.hpp"
#include "CommandLambdaAction.hpp"
#include "Gui.hpp"
#include "Hooking.hpp"
#include "StringUtils.hpp"
#include "UnicodePrivateUse.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandLabel : public CommandList
	{
	public:
		std::string key;
		uint32_t label_hash;

		explicit CommandLabel(CommandList* const parent, const std::string& key, std::string&& text)
			: CommandList(parent, NOLABEL, {}, NOLABEL, CMDFLAGS_LIST | CMDFLAG_FEATURELIST_SKIP), key(key), label_hash(CommandEditLabels::keyToHash(key))
		{
			StringUtils::to_upper(this->key);
			set_menu_name(text);
			g_hooking.label_replacements[label_hash] = std::move(text);
			this->createChild<CommandEditLabel>();
			{
				CommandName cmdname = CMDNAME("reset");
				cmdname.append(utf8ToCmdName(key));
				StringUtils::simplifyCommandName(cmdname);
				this->createChild<CommandLambdaAction>(LIT(LANG_FMT("RESET", g_hooking.getLabelTextSafe(label_hash))), { std::move(cmdname) }, NOLABEL, [this](Click&)
				{
					g_hooking.label_replacements[label_hash] = g_hooking.getLabelTextSafe(label_hash);
					this->on_edit();
				});
			}
			{
				CommandName cmdname = CMDNAME("remove");
				cmdname.append(utf8ToCmdName(key));
				StringUtils::simplifyCommandName(cmdname);
				this->createChild<CommandLambdaAction>(LOC("REMOVE"), { std::move(cmdname) }, NOLABEL, [this](Click&)
				{
					this->parent->as<CommandListUserkv>()->removeItem(this->key);
				});
			}
		}

		void set_menu_name(const std::string& text)
		{
			setMenuName(LIT(std::move(std::string(key).append(": ").append(StringUtils::utf16_to_utf8(UnicodePrivateUse::fromGta(StringUtils::utf8_to_utf16(text)))))));
		}

		void on_edit()
		{
			set_menu_name(g_hooking.label_replacements.at(label_hash));
			backToParent(TC_OTHER);
			g_gui.on_command_state_change();
		}

		void backToParent(ThreadContext thread_context)
		{
			goBackIfActive(thread_context);
			parent->updateWebState();
		}
	};
}
