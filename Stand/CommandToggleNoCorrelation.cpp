#include "CommandToggleNoCorrelation.hpp"

#include "get_next_arg.hpp"
#include "Gui.hpp"
#include "regular_event.hpp"
#include "RelayCon.hpp"
#include "Script.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandToggleNoCorrelation::CommandToggleNoCorrelation(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const bool default_on, const commandflags_t flags, const CommandPerm perm, const std::vector<Hotkey>& default_hotkeys, CommandType type)
		: CommandPhysical(type, parent, std::move(menu_name), std::move(command_names), std::move(help_text), flags, perm, default_hotkeys), default_on(default_on), m_on(default_on)
	{
	}

	std::wstring CommandToggleNoCorrelation::getCommandSyntax() const
	{
		return command_names.empty() ? std::wstring{} : std::move(LANG_GET_W("CMD").append(L": ").append(cmdNameToUtf16(command_names.at(0))).append(L" [on/off]"));
	}

	void CommandToggleNoCorrelation::onClick(Click& click)
	{
		if (!job_queued)
		{
			onClickSoundEffect(click);
			toggleState(click);
			onChange(click);
		}
	}

	void CommandToggleNoCorrelation::onClickSoundEffect(Click& click) const noexcept
	{
		click.sound_feedback = m_on ? SOUND_NO : SOUND_YES;
	}

	void CommandToggleNoCorrelation::onClickImplUnavailable(Click& click)
	{
		click.setUnavailable();
		if (click.isWeb())
		{
			updateWebState();
		}
	}

	void CommandToggleNoCorrelation::onChange(Click& click)
	{
		if (m_on)
		{
			onEnable(click);
		}
		else
		{
			onDisable(click);
		}
	}

	void CommandToggleNoCorrelation::onEnable(Click& click)
	{
	}

	void CommandToggleNoCorrelation::onDisable(Click& click)
	{
	}

	void CommandToggleNoCorrelation::onChangeImplUnavailable(Click& click)
	{
		click.setUnavailable();
		setOnIndication(!m_on);
	}

	void CommandToggleNoCorrelation::onCommand(Click& click, std::wstring& args)
	{
		auto arg = get_next_arg(args);
		StringUtils::to_lower(arg);
		bool desired_on = (arg == L"on" || arg == L"yes" || arg == L"true");
		if (!desired_on && (arg == L"default"))
		{
			desired_on = default_on;
		}
		if (arg.empty() || desired_on != m_on)
		{
			onClick(click);
		}
		else if (click.canHaveGenericErrorResponse())
		{
			auto activation_name = getActivationName();
			if (activation_name == ATSTRINGHASH("ON"))
			{
				click.setResponse(m_on ? LOC("CMDSTATE_OON_E") : LOC("CMDSTATE_OOFF_E"));
			}
			else
			{
				click.setResponse(LIT(fmt::format(fmt::runtime(Lang::get(m_on ? ATSTRINGHASH("CMDSTATE_LON_E") : ATSTRINGHASH("CMDSTATE_LOFF_E"))), activation_name.getLocalisedUtf8())));
			}
		}
	}

	std::string CommandToggleNoCorrelation::getState() const
	{
		return (m_on ? "On" : "Off");
	}

	std::string CommandToggleNoCorrelation::getDefaultState() const
	{
		return (default_on ? "On" : "Off");
	}

	void CommandToggleNoCorrelation::setState(Click& click, const std::string& _state)
	{
		setStateBool(click, _state == "On");
	}

	void CommandToggleNoCorrelation::applyDefaultState()
	{
		Click click(CLICK_BULK, TC_SCRIPT_YIELDABLE);
		applyDefaultStateImpl(click, default_on);
	}

	void CommandToggleNoCorrelation::applyDefaultStateImpl(Click& click, bool should_be_on)
	{
		if (m_on != should_be_on)
		{
			for (int j = 0; j < 2; j++)
			{
				onClick(click);
				if (click.sound_feedback == SOUND_ERROR)
				{
					break;
				}
				for (int i = 0; i < 100; i++)
				{
					if (m_on == should_be_on)
					{
						return;
					}
					Script::current()->yield();
				}
			}
		}
	}

	void CommandToggleNoCorrelation::setStateBool(Click& click, bool toggle)
	{
		if (m_on != toggle)
		{
			onClick(click);
		}
	}

	void CommandToggleNoCorrelation::toggleState(Click& click)
	{
		m_on = !m_on;
		updateState(click);
	}

	void CommandToggleNoCorrelation::updateState(Click& click)
	{
		if (click.canHaveGenericResponse())
		{
			auto activation_name = getActivationName();
			if (activation_name == ATSTRINGHASH("ON"))
			{
				click.setGenericResponse(m_on ? LOC("CMDSTATE_OON") : LOC("CMDSTATE_OOFF"));
			}
			else
			{
				click.setGenericResponse(LIT(fmt::format(fmt::runtime(Lang::get(m_on ? ATSTRINGHASH("CMDSTATE_LON") : ATSTRINGHASH("CMDSTATE_LOFF"))), activation_name.getLocalisedUtf8())));
			}
		}
		processVisualUpdate(true);
		if (click.type != CLICK_BULK && supportsStateOperations())
		{
			processStateChange();
		}
		if (!click.isWeb())
		{
			updateWebState();
		}
	}

	void CommandToggleNoCorrelation::setOnIndication(bool on)
	{
		if (m_on != on)
		{
			m_on = on;
			processVisualUpdate(true);
			updateWebState();
		}
	}

	void CommandToggleNoCorrelation::updateWebState()
	{
		if (isActiveOnWeb())
		{
			g_relay.sendLineAsync(std::move(std::string(m_on ? "n " : "f ").append(menu_name.getWebString())));
		}
	}

	void CommandToggleNoCorrelation::toggleScriptTickEventHandler(const bool on, Click& click, void handler())
	{
		if (on)
		{
			if (thread_context_is_script(click.thread_context))
			{
				handler();
			}
			reScriptTickEvent::registerHandler(handler);
		}
		else
		{
			reScriptTickEvent::unregisterHandler(handler);
		}
	}

	void CommandToggleNoCorrelation::onClickToggleScriptTickEventHandler(Click& click, void handler()) const
	{
		toggleScriptTickEventHandler(!m_on, click, handler);
	}

	void CommandToggleNoCorrelation::onChangeToggleScriptTickEventHandler(bool execute_now_if_on, void handler()) const
	{
		if (m_on)
		{
			if (execute_now_if_on)
			{
				handler();
			}
			reScriptTickEvent::registerHandler(handler);
		}
		else
		{
			reScriptTickEvent::unregisterHandler(handler);
		}
	}

	void CommandToggleNoCorrelation::onChangeToggleScriptTickEventHandler(Click& click, void handler()) const
	{
		toggleScriptTickEventHandler(m_on, click, handler);
	}

	void CommandToggleNoCorrelation::onChangeTogglePostTickEventHandler(void handler()) const
	{
		if (m_on)
		{
			rePostTickEvent::registerHandler(handler);
		}
		else
		{
			rePostTickEvent::unregisterHandler(handler);
		}
	}
}
