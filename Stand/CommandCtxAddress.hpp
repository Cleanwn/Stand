#pragma once

#include "CommandTextslider.hpp"

#include <soup/urlenc.hpp>

#include "CommandLink.hpp"
#include "Renderer.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandCtxAddress : public CommandTextslider
	{
	public:
		explicit CommandCtxAddress(CommandList* const parent)
			: CommandTextslider(parent, LOC("CTX_ADDR"), {}, NOLABEL, {
				{0, LOC("CTX_ADDR_0")},
				{1, LOC("CTX_ADDR_1")},
				{2, LOC("CTX_ADDR_2")},
				{3, LOC("CTX_ADDR_3")},
			})
		{
		}

		void onClick(Click& click) final
		{
			auto* target = ContextMenu::getTarget();
			if (target->isT<CommandLink>()
				&& (target->as<CommandLink>()->show_address_in_corner || value == 2)
				)
			{
				target = target->as<CommandLink>()->getTarget();
			}
			if (value == 0) // User
			{
				Util::copy_to_clipboard_utf16(click, target->getLocalisedAddressW(g_renderer.address_separator));
			}
			else if (value == 1) // Default
			{
				Util::copy_to_clipboard_utf16(click, target->getEnglishAddressW(L" > "));
			}
			else if (value == 2) // API
			{
				Util::copy_to_clipboard_utf8(click, target->getPathConfig());
			}
			else // Link
			{
				Util::copy_to_clipboard_utf8(click, soup::ObfusString("https://stand.sh/focus#").str() + soup::urlenc::encode(target->getPathConfig()));
			}
		}
	};
}
