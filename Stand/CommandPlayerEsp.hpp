#pragma once

#include "CommandToggle.hpp"

#include "AbstractPlayer.hpp"
#include "CommandSlider.hpp"
#include "tbScreenshotMode.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandPlayerEsp : public CommandToggle
	{
	protected:
		CommandSlider* const max_distance;

	public:
		explicit CommandPlayerEsp(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, CommandSlider* max_distance)
			: CommandToggle(parent, std::move(menu_name), std::move(command_names)), max_distance(max_distance)
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				if (!m_on)
				{
					return false;
				}
				if (!g_tb_screenshot_mode.isEnabled())
				{
					const auto cam_pos = CAM::GET_FINAL_RENDERED_CAM_COORD();
					const auto max_distance = (float)this->max_distance->value;
					for (AbstractPlayer p : AbstractPlayer::listExcept(true))
					{
						v3 pos = p.getPos();
						if (!pos.isNull())
						{
							float dist = pos.distance(cam_pos);
							if (dist > max_distance)
							{
								continue;
							}
							drawEsp(std::move(p), std::move(pos), std::move(dist));
						}
					}
				}
				return true;
			});
		}

		virtual void drawEsp(AbstractPlayer&& p, v3&& pos, float&& dist) const = 0;
	};
}
