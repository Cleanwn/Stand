#include "CommandPosition2dMouse.hpp"

#include "CommandPosition2d.hpp"
#include "Gui.hpp"
#include "Renderer.hpp"

namespace Stand
{
	CommandPosition2dMouse::CommandPosition2dMouse(CommandList* parent, const CommandName& command_name)
		: CommandToggle(parent, LOC("MOVWTHMSE"), { std::move(CommandName(command_name).append(CMDNAME("mousemove"))) }, NOLABEL, false, CMDFLAGS_TOGGLE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS)
	{
	}

	void CommandPosition2dMouse::onEnable(Click& click)
	{
		prev_mouse_mode = g_gui.mouse_mode;
		g_gui.setMouseMode(MouseMode::MOVE);
		registerPresentEventHandler([this]
		{
			if (!m_on)
			{
				return false;
			}
			if (g_renderer.mouse_squeaks)
			{
				if (!parent_start_pos.isValid())
				{
					parent_start_pos = ((CommandPosition2d*)parent)->getPosition();
					cursor_start_pos = g_renderer.getCursorPosH(false);
				}
				else
				{
					updatePos(g_renderer.getCursorPosH(true));
				}
			}
			else
			{
				DirectX::SimpleMath::Vector2 cursor_pos = g_renderer.getCursorPosH(true);
				if (parent_start_pos.isValid())
				{
					updatePos(std::move(cursor_pos));
					parent_start_pos.invalidate();
				}
			}
			return true;
		});
	}

	void CommandPosition2dMouse::updatePos(DirectX::SimpleMath::Vector2&& cursor_pos) const
	{
		Position2d pos(parent_start_pos);
		pos.x += int16_t(cursor_pos.x - cursor_start_pos.x);
		pos.y += int16_t(cursor_pos.y - cursor_start_pos.y);
		((CommandPosition2d*)parent)->setPosition(std::move(pos));
	}

	void CommandPosition2dMouse::onDisable(Click& click)
	{
		g_gui.setMouseMode(prev_mouse_mode);
	}
}
