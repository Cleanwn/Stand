#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "input.hpp"
#include "tbPositionSelection.hpp"
#include "TpUtil.hpp"

namespace Stand
{
	class CommandTpGun : public CommandToggle
	{
	private:
		bool enabled_pos_sel = false;

	public:
		explicit CommandTpGun(CommandList* const parent)
			: CommandToggle(parent, LOC("TPGUN"), CMDNAMES("tpgun", "tpweapon", "guntp", "weapontp"))
		{
		}

	private:
		void enablePosSel()
		{
			if (!enabled_pos_sel)
			{
				enabled_pos_sel = true;
				g_tb_position_selection.enable();
			}
		}

		void disablePosSel()
		{
			if (enabled_pos_sel)
			{
				enabled_pos_sel = false;
				g_tb_position_selection.disable();
			}
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				if (m_on)
				{
					if (g_player_ped.isAiming())
					{
						enablePosSel();
						ENTITY::SET_ENTITY_HAS_GRAVITY(g_player_ped, FALSE);
						if (Input::isPressingAttack())
						{
							TpUtil::teleport(g_tb_position_selection.pos, &nullsub<>);
						}
					}
					else
					{
						ENTITY::SET_ENTITY_HAS_GRAVITY(g_player_ped, TRUE);
						disablePosSel();
					}
					return true;
				}
				disablePosSel();
				return false;
			});
		}
	};
}
