#include "CommandPositionSelection.hpp"

#include "DrawUtil3d.hpp"
#include "tbFreecam.hpp"
#include "Gui.hpp"
#include "parse_coord.hpp"
#include "tbPositionSelection.hpp"
#include "tbReticle.hpp"

namespace Stand
{
	CommandPositionSelection::CommandPositionSelection(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names)
		: CommandWithOnTickFocused(parent, std::move(menu_name), std::move(command_names), NOLABEL, "467.1256, 5543.1304, 785.0175")
	{
	}

	void CommandPositionSelection::onClick(Click& click)
	{
		if (g_gui.opened)
		{
			g_gui.close(click.thread_context);
		}
		g_tb_position_selection.pos = pos;
		{
			const bool just_enabled_freecam = !g_tb_freecam.isEnabled();
			g_tb_freecam.ensureMovementControlsAreEnabled();
			g_tb_freecam.enable();
			if (just_enabled_freecam)
			{
				g_tb_freecam.pos += 30.0f;
			}
		}
		g_tb_position_selection.enable();
		g_tb_reticle.enable();
		is_being_changed = true;
		click.setResponse(LIT(LANG_FMT("SELPOS_T", Gui::getMenuOpenCloseButtonString())));
		registerScriptTickEventHandler(click, [this]
		{
			if (!g_gui.opened && !g_gui.isUnloadPending())
			{
				DrawUtil3d::draw_ar_beacon(g_tb_position_selection.pos);
				return true;
			}
			pos = g_tb_position_selection.pos;
			g_tb_position_selection.disable();
			g_tb_freecam.disable();
			g_tb_reticle.disable();
			is_being_changed = false;
			setValue(pos);
			onChange();
			return false;
		});
	}

	void CommandPositionSelection::onCommand(Click& click, std::wstring& args)
	{
		onCommandApply(click, args);
		setPosFromValue();
	}

	void CommandPositionSelection::onTickFocused()
	{
		if (!is_being_changed)
		{
			DrawUtil3d::draw_line_and_ar_beacon(pos);
		}
	}

	void CommandPositionSelection::setPosFromValue()
	{
		auto res = parse_coord(value);
		if (res.has_value())
		{
			pos = res.value();
		}
		else
		{
			pos.reset();
		}
		this->value = pos;
		onChange();
	}

	void CommandPositionSelection::onChange()
	{
	}
}
