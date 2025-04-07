#include "CommandSendChatMessage.hpp"

#include "Chat.hpp"
#include "CommandLuaScript.hpp"
#include "CommandToggle.hpp"
#include "ExecCtx.hpp"
#include "Hooking.hpp"
#include "Script.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandSendChatMessage::CommandSendChatMessage(CommandList* parent, CommandToggleNoCorrelation* in_team_chat, CommandToggleNoCorrelation* add_to_local_history)
		: CommandAction(parent, LOC("SNDMSG"), { CMDNAME("say") }), in_team_chat(in_team_chat), add_to_local_history(add_to_local_history)
	{
	}

	std::wstring CommandSendChatMessage::getCommandSyntax() const
	{
		return std::move(LANG_GET_W("CMD").append(L": say ").append(LANG_GET_W("ARGTXT")));
	}

	void CommandSendChatMessage::onCommand(Click& click, std::wstring& args)
	{
		if (args.empty())
		{
			return onClick(click);
		}
		auto message = std::move(args);
		args.clear();
		const bool team_chat = in_team_chat->m_on;
		if (click.type == CLICK_SCRIPTED)
		{
			if (!ExecCtx::get().isScript())
			{
				return; // Lua script may be executing in OS thread.
			}
			if (auto script = CommandLuaScript::fromFiber(Script::current()))
			{
				if (!script->canSendChatMessage(Chat::getNumRecipientsExcludingSelf(team_chat)))
				{
					return;
				}
			}
		}
		ensureYieldableScriptThread(click, [this, message{ std::move(message) }, team_chat]() mutable
		{
			auto message_u8 = StringUtils::utf16_to_utf8(std::move(message));
			if (!g_hooking.bypass_profanity_filter
				&& !message_u8.empty()
				)
			{
				g_hooking.censorChatMessageImpl(message_u8);
			}
			Chat::sendMessage(std::move(message_u8), team_chat, add_to_local_history->m_on, true);
		});
	}
}
