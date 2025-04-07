#pragma once

#include "CommandToggle.hpp"

#include <fmt/xchar.h>

#include "AptHelper.hpp"
#include "Renderer.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class CommandApartments : public CommandToggle
	{
	public:
		explicit CommandApartments(CommandList* const parent)
			: CommandToggle(parent, LIT("Show Apartments"), CMDNAMES("showapartments"), LIT("Pos. ID -> Apt. ID(s)"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				for (uint8_t pos_id = 1; pos_id <= AptHelper::max_pos_id; pos_id++)
				{
					std::wstring apt_ids{};
					for (uint8_t property_id = 1; property_id <= AptHelper::max_property_id; property_id++)
					{
						if (AptHelper::propertyIdToPosId(property_id) == pos_id)
						{
							StringUtils::list_append(apt_ids, fmt::to_wstring(property_id));
						}
					}
					apt_ids.insert(0, fmt::format(L"{} -> ", pos_id));
					g_renderer.draw3dTextThisTickH(AptHelper::getPosFromId(pos_id), apt_ids, 20.0f);
				}
			});
		}
	};
}
