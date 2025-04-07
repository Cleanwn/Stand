#include "Click.hpp"

#include "AbstractPlayer.hpp"
#include "Auth.hpp"
#include "ChatCommandsCommon.hpp"
#include "CommandListSelect.hpp"
#include "ExecCtx.hpp"
#include "FiberPool.hpp"
#include "get_current_time_millis.hpp"
#include "Gui.hpp"
#include "is_session.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"
#include "Worker.hpp"

//#include "Exceptional.hpp"

namespace Stand
{
	CommandList* getRootListPtr()
	{
		return g_gui.root_list.get();
	}

	/*static void validateTc(ThreadContext thread_context)
	{
		if (thread_context != ExecCtx::get().tc
			&& thread_context != TC_OTHER
			//&& !(thread_context == TC_SCRIPT_NOYIELD && ExecCtx::get().tc == TC_SCRIPT_YIELDABLE)
			)
		{
			Exceptional::report(fmt::format("TC mismatch: Got {}, but ExecCtx says {}", (uint8_t)thread_context, (uint8_t)ExecCtx::get().tc));
		}
	}*/

	Click::Click(ClickType type) noexcept
		: Click(type, ExecCtx::get().tc)
	{
	}

	Click::Click(ClickType type, ThreadContext thread_context) noexcept
		: type(type), thread_context(thread_context), issued_by(g_player), issued_for(issued_by), issued_by_user(true)
	{
		//validateTc(thread_context);
	}

	Click::Click(ClickType type, ThreadContext thread_context, compactplayer_t issuer) noexcept
		: type(type), thread_context(thread_context), issued_by(issuer), issued_for(issuer), issued_by_user(issuer == g_player)
	{
		//validateTc(thread_context);
	}

	Click::Click(const Click& b) noexcept
		: type(b.type),
		thread_context(b.thread_context),
		issued_by(b.issued_by),
		issued_for(b.issued_for),
		issued_by_user(b.issued_by_user),
		issuer_is_explicit(b.issuer_is_explicit),
		chain_state(b.chain_state),
		sound_feedback(b.sound_feedback),
		response(b.response),
		response_type(b.response_type),
		response_acknowledged(b.response_acknowledged)
	{
	}

	Click::Click(Click&& b) noexcept
		: type(b.type),
		thread_context(b.thread_context),
		issued_by(b.issued_by),
		issued_for(b.issued_for),
		issued_by_user(b.issued_by_user),
		issuer_is_explicit(b.issuer_is_explicit),
		chain_state(b.chain_state),
		sound_feedback(b.sound_feedback),
		response(std::move(b.response)),
		response_type(b.response_type),
		response_acknowledged(b.response_acknowledged)
	{
		b.forgetResponse();
	}

	Click::~Click()
	{
		if (!response_acknowledged && response_type == RESPONSE_CUSTOM)
		{
			//ensureResponse();
			//respond(TC_OTHER);
			respond();
		}
	}

	Click Click::derive(ClickType type) const noexcept
	{
		Click click{ type, thread_context };
		/*click.issued_by = issued_by;
		click.issued_for = issued_for;
		click.issuer_is_explicit = issuer_is_explicit;*/
		return click;
	}

	Click Click::deriveAuto() const noexcept
	{
		return derive(CLICK_AUTO);
	}

	bool Click::canShowCommandBox() const noexcept
	{
		return !isAuto()
			&& issuedByUser()
			;
	}

	void Click::showCommandBoxIfPossible(const std::wstring& prefill)
	{
		if (canShowCommandBox())
		{
			showCommandBox(prefill);
		}
	}

	void Click::showCommandBox(const std::wstring& prefill)
	{
		g_gui.showCommandBox(prefill, *this);
	}

	AbstractPlayer Click::getIssuer() const noexcept
	{
		return AbstractPlayer(issued_by);
	}

	AbstractPlayer Click::getEffectiveIssuer() const noexcept
	{
		return AbstractPlayer(issued_for);
	}

	bool Click::issuedByUser() const noexcept
	{
		return issued_by_user;
	}

	bool Click::issuedByAndForUser() const noexcept
	{
		return issued_by_user && issued_by == issued_for;
	}

	void Click::ensureYieldableScriptThread(std::function<void()>&& func) const
	{
		if (thread_context == TC_SCRIPT_YIELDABLE)
		{
			func();
		}
		else
		{
			FiberPool::queueJob(std::move(func));
		}
	}

	void Click::ensureScriptThread(std::function<void()>&& func) const
	{
		if (thread_context_is_script(thread_context))
		{
			func();
		}
		else
		{
			FiberPool::queueJob(std::move(func));
		}
	}

	void Click::ensureScriptThread(std::function<void(Click&)>&& func)
	{
		if (thread_context_is_script(thread_context))
		{
			func(*this);
		}
		else
		{
			FiberPool::queueJob([click{ *this }, func{ std::move(func) }]() mutable
			{
				click.thread_context = TC_SCRIPT_YIELDABLE;
				func(click);
			});
		}
	}

	void Click::ensureWorkerContext(std::function<void()>&& func) const
	{
		if (thread_context == TC_WORKER)
		{
			func();
		}
		else
		{
			Worker::queueJob(std::move(func));
		}
	}

	bool Click::isMenu() const noexcept
	{
		return type == CLICK_MENU;
	}

	bool Click::isAuto() const noexcept
	{
		return type & CLICK_FLAG_AUTO;
	}

	bool Click::isStand() const noexcept
	{
		switch (type)
		{
		case CLICK_BULK:
		case CLICK_AUTO:
			return true;
		}
		return false;
	}

	bool Click::isBulk() const noexcept
	{
		return type == CLICK_BULK;
	}

	bool Click::isChat() const noexcept
	{
		return type & CLICK_FLAG_CHAT;
	}

	bool Click::isWeb() const noexcept
	{
		return type & CLICK_FLAG_WEB;
	}

	bool Click::isCommandBoxVariety() const noexcept
	{
		return type == CLICK_COMMAND || type == CLICK_WEB_COMMAND || isChat();
	}

	bool Click::canOpenCommandBox() const noexcept
	{
		return type == CLICK_MENU || type == CLICK_HOTKEY || type == CLICK_WEB || isCommandBoxVariety();
	}

	void Click::setUnavailable() noexcept
	{
		sound_feedback = SOUND_ERROR;
		stopInputIfAllowed();
	}

	bool Click::isStopInputAllowed() const noexcept
	{
		return isMenu();
	}

	void Click::stopInputIfAllowed() const noexcept
	{
		if (isStopInputAllowed())
		{
			g_gui.inputStop();
		}
	}

	bool Click::inOnline() noexcept
	{
		if (!is_session_started())
		{
			setResponse(LOC("CMDONL"));
			return false;
		}
		return true;
	}

	bool Click::inOnline() const noexcept
	{
		return Click(*this).inOnline();
	}

	bool Click::isBasicEdition() noexcept // OBFUS!
	{
		if ((isBulk() ? g_auth.license_permissions : g_auth.verifyPermSig()) < LICPERM_BASIC)
		{
			if (!isAuto())
			{
				setResponse(LOC("FREE_T"));
			}
			return false;
		}
		//last_successful_permission_check = get_current_time_millis();
		return true;
	}

	bool Click::isRegularEdition() noexcept // OBFUS!
	{
		if ((isBulk() ? g_auth.license_permissions : g_auth.verifyPermSig()) < LICPERM_REGULAR)
		{
			setResponse(LOC("CMDNEDR"));
			return false;
		}
		last_successful_permission_check = get_current_time_millis();
		return true;
	}

	bool Click::isRegularEditionOrSp() noexcept // OBFUS!
	{
		if (g_auth.license_permissions >= LICPERM_REGULAR || !is_session_or_transition_active())
		{
			return true;
		}
		setResponse(LOC("CMDFULLSP"));
		return false;
	}

	bool Click::isUltimateEdition() noexcept // OBFUS!
	{
		if ((isBulk() ? g_auth.license_permissions : g_auth.verifyPermSig()) < LICPERM_ULTIMATE)
		{
			setResponse(LOC("CMDNEDU"));
			return false;
		}
		last_successful_permission_check = get_current_time_millis();
		return true;
	}

	bool Click::canBypassEditionRestrictions() noexcept
	{
		return isBulk() && g_gui.isFastLoad();
	}

	bool Click::isHost() noexcept
	{
		if (inOnline())
		{
			if (g_player.isHost())
			{
				return true;
			}
			setResponse(LOC("CMDNEEDHOST"));
		}
		return false;
	}

	bool Click::isBoss() noexcept
	{
		if (!g_player.isBoss())
		{
			setResponse(LOC("CMDNBOSS"));
			return false;
		}
		return true;
	}

	bool Click::canExecuteChainCommand() noexcept
	{
		if (chain_state != ChainState::NONE)
		{
			setNoResponse();
			return false;
		}
		chain_state = ChainState::PASSED_THROUGH;
		return true;
	}

	void Click::setChainCommandExecuted() noexcept
	{
		chain_state = ChainState::EXECUTED;
	}

	bool Click::hasResponse() const noexcept
	{
		return !response.empty() || response_type == RESPONSE_NONE;
	}

	bool Click::canHaveResponse() const noexcept
	{
		return type != CLICK_BULK;
	}

	bool Click::canHaveGenericResponse() const noexcept
	{
		return !isAuto();
	}

	bool Click::canHaveGenericErrorResponse() const noexcept
	{
		return canHaveGenericResponse();
	}

	void Click::setResponse(Label&& response) noexcept
	{
		this->response = std::move(response);
		this->response_type = RESPONSE_CUSTOM;
	}

	void Click::setResponse(Label&& response) const
	{
		Click(*this).setResponse(std::move(response));
	}

	void Click::setGenericResponse(Label&& response)
	{
		if (!hasResponse())
		{
			this->response = std::move(response);
			this->response_type = RESPONSE_GENERIC;
		}
	}

	void Click::setNoResponse() noexcept
	{
		response.reset();
		response_type = RESPONSE_NONE;
	}

	void Click::notAvailableOnUser()
	{
		if (issuedByAndForUser())
		{
			setResponse(LOC("CMDOTH"));
		}
		else
		{
			setResponse(LOC("SLFHRM_O"));
		}
	}

	void Click::uwotm8()
	{
		setNoResponse();
		Util::toast_uwotm8();
	}

	void Click::setSpoofUpdate()
	{
		if (is_session_started())
		{
			setResponse(LOC("SPFUPD"));
		}
	}

	void Click::ensureResponse()
	{
		if (!hasResponse() && canHaveGenericResponse())
		{
			setGenericResponse(LOC("CMD_DR_S"));
		}
	}

	void Click::ensureActivationName(const std::string& activation_name)
	{
		if (response_type != RESPONSE_NONE
			&& response.getLocalisedUtf8().find(activation_name) == std::string::npos
			)
		{
			response.makeLiteralLocalised();
			response.literal_str.insert(0, ": ").insert(0, activation_name);
		}
	}

	Label Click::getResponse()
	{
		response_acknowledged = true;
		return response;
	}

	void Click::forgetResponse() noexcept
	{
		chain_state = ChainState::NONE;
		response.reset();
		response_type = RESPONSE_GENERIC;
		response_acknowledged = false;
	}

	void Click::respond()
	{
		if (canHaveResponse()
			&& !response.empty()
			)
		{
			response_acknowledged = true;
			SOUP_IF_LIKELY (issuedByUser())
			{
				Util::toast(response, (isWeb() ? TOAST_WEB : TOAST_ABOVE_MAP));
			}
			else
			{
				const bool team_chat = (type == CLICK_CHAT_TEAM);
				if (ChatCommandsCommon::reply_visibility->value == -1) // Everyone
				{
					Util::toastChat(response, team_chat);
				}
				else
				{
					if (ChatCommandsCommon::reply_visibility->value & (1 << 1)) // Issuer
					{
						Util::toastChat(response, team_chat, issued_by);
					}
					if (ChatCommandsCommon::reply_visibility->value & (1 << 0)) // Me
					{
						Util::toastChat(response, team_chat, g_player);
					}
				}
			}
		}
	}

	bool Click::isSoundAllowed() const noexcept
	{
		if (g_gui.isSoundEnabled())
		{
			switch (type)
			{
			case CLICK_MENU:
			case CLICK_COMMAND:
			case CLICK_HOTKEY:
				return true;
			}
		}
		return false;
	}

	CommandPerm Click::getEffectivePermissions() const
	{
		bool is_friend = false;
		if (issuedByUser())
		{
			if (!isChat() || !self_chat_commands_friends_perms)
			{
				return COMMANDPERM_ALL;
			}
			is_friend = true;
		}
		else
		{
			is_friend = getIssuer().isFriend();
		}
		CommandPerm perms = perms_strangers;
		if (is_friend)
		{
			perms |= perms_friends;
		}
		if (getIssuer().isCrewMember())
		{
			perms |= perms_crew;
		}
		if (type == CLICK_CHAT_TEAM)
		{
			perms |= perms_team;
		}
		return perms;
	}

	bool Click::shouldAutomaticallyProceedOnWarning(bool skippable) const noexcept
	{
		if (type == CLICK_HOTKEY)
		{
			return auto_proceed_warnings_for_hotkeys;
		}
		if (isCommandBoxVariety())
		{
			return auto_proceed_warnings_for_commands;
		}
		if (type == CLICK_SCRIPTED)
		{
			// Scripts must not be able to skip LUA_W_OS warnings.
			return skippable;
		}
		return false;
	}

	void Click::setResponseCommandUnknownOrAmbiguous(bool is_unknown, const std::wstring& input)
	{
		if (is_unknown)
		{
			setResponse(LIT(LANG_FMT("CMDUNK", StringUtils::utf16_to_utf8(input))));
		}
		else
		{
			setResponse(LIT(LANG_FMT("CMDAMBIG", StringUtils::utf16_to_utf8(input))));
		}
	}

	CommandIssuable* Click::getCommandIssuableFromArg(const std::wstring& arg)
	{
#if COMPACT_COMMAND_NAMES
		CommandName command_name = StringUtils::utf16_to_utf8(arg);
#else
		const CommandName& command_name = arg;
#endif
		std::vector<CommandIssuable*> commands{};
		getRootListPtr()->findCommandsWhereCommandNameStartsWith<CommandIssuable>(commands, command_name, getEffectivePermissions());
		if (commands.size() == 1)
		{
			return *commands.cbegin();
		}
		setResponseCommandUnknownOrAmbiguous(commands.empty(), arg);
		return nullptr;
	}

	CommandPhysical* Click::getCommandPhysicalFromArg(const std::wstring& arg)
	{
#if COMPACT_COMMAND_NAMES
		CommandName command_name = StringUtils::utf16_to_utf8(arg);
#else
		const CommandName& command_name = arg;
#endif
		std::vector<CommandPhysical*> commands{};
		getRootListPtr()->findCommandsWhereCommandNameStartsWith<CommandPhysical>(commands, command_name, getEffectivePermissions());
		if (commands.size() == 1)
		{
			return *commands.cbegin();
		}
		setResponseCommandUnknownOrAmbiguous(commands.empty(), arg);
		return nullptr;
	}
}
