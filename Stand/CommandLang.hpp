#pragma once

#include "CommandListSelect.hpp"

#include "CommandboxGrid.hpp"
#include "Gui.hpp"
#include "RelayCon.hpp"
#include "RootMgr.hpp"

#define LANG_ENTRY(id, ...) \
if (Lang::isEnabled(id)) \
{ \
	options.emplace_back(CommandListActionItemData{ id, LIT(Lang::id_to_name(id)), __VA_ARGS__ }); \
}

namespace Stand
{
	class CommandLang : public CommandListSelect
	{
	private:
		static inline bool working = false;

		[[nodiscard]] static std::vector<CommandListActionItemData> getOptions()
		{
			std::vector<CommandListActionItemData> options{};

			LANG_ENTRY(LANG_ZH, CMDNAMES("zh"));
			LANG_ENTRY(LANG_NL, CMDNAMES("nl"));
			LANG_ENTRY(LANG_EN, CMDNAMES("enuk", "en", "engb"));
			LANG_ENTRY(LANG_ENUS, CMDNAMES("enus"));
			LANG_ENTRY(LANG_FR, CMDNAMES("fr"));
			LANG_ENTRY(LANG_DE, CMDNAMES("de"));
			LANG_ENTRY(LANG_IT, CMDNAMES("it"));
			LANG_ENTRY(LANG_JA, CMDNAMES("ja"));
			LANG_ENTRY(LANG_KO, CMDNAMES("ko"));
			LANG_ENTRY(LANG_LT, CMDNAMES("lt"));
			LANG_ENTRY(LANG_PL, CMDNAMES("pl"));
			LANG_ENTRY(LANG_PT, CMDNAMES("pt"), LOC("LANG_H2"));
			LANG_ENTRY(LANG_RU, CMDNAMES("ru"));
			LANG_ENTRY(LANG_ES, CMDNAMES("es"));
			LANG_ENTRY(LANG_TR, CMDNAMES("tr"));
			LANG_ENTRY(LANG_VI, CMDNAMES("vi"));

			LANG_ENTRY(LANG_SEX, CMDNAMES("sex"));
			LANG_ENTRY(LANG_UWU, CMDNAMES("uwu"));
			LANG_ENTRY(LANG_HORNYUWU, CMDNAMES("hornyuwu"));

			return options;
		}

	public:
		explicit CommandLang(CommandList* const parent)
			: CommandListSelect(parent, LOC("LANG"), CMDNAMES("lang"), LOC("LANG_H"), getOptions(), Lang::default_id)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			if (Lang::active_id == (lang_t)value)
			{
				return;
			}
			if (working)
			{
				onChangeImplUnavailable(click, prev_value);
				return;
			}
			FiberPool::queueJob([=]
			{
				if (Lang::active_id != (lang_t)value && !working && !g_gui.isUnloadPending())
				{
					working = true;

					// If this is invoked as part of the root state update, we want to give lua scripts a tick to
					// set up their environment before we can tear it down again.
					Script::current()->yield();

					Lang::setActive((lang_t)value);
					g_commandbox_grid.update();
					if (g_gui.root_lang != Lang::active_id)
					{
						ensurePhysicalFocus(TC_SCRIPT_YIELDABLE);
						RootMgr::updateRootState();
					}
					if (!g_renderer.isUserFontAcceptableForLanguage())
					{
						g_renderer.switchToPresetFont(g_renderer.getFontForLanguage());
					}
					if (g_gui.isWebGuiActive())
					{
						g_relay.sendLang();
					}
					working = false;
				}
			});
		}

		void applyDefaultState() final
		{
			if (!g_gui.about_to_update_root_state && !g_gui.isUnloadPending())
			{
				CommandListSelect::applyDefaultState();
			}
		}
	};
}
