#include "CommandSetGameLanguage.hpp"

#include "CProfileSettings.hpp"
#include "eMenuPref.hpp"
#include "sysLanguage.hpp"

#include "CommandListActionItem.hpp"
#include "Label.hpp"
#include "lang.hpp"
#include "LangId.hpp"
#include "pointers.hpp"

namespace Stand
{
	[[nodiscard]] static Label getLanguageMenuName(rage::sysLanguage lang)
	{
		switch (lang)
		{
		case LANGUAGE_ENGLISH: return LIT(Lang::id_to_name(LANG_ENUS));
		case LANGUAGE_FRENCH: return LIT(Lang::id_to_name(LANG_FR));
		case LANGUAGE_GERMAN: return LIT(Lang::id_to_name(LANG_DE));
		case LANGUAGE_ITALIAN: return LIT(Lang::id_to_name(LANG_IT));
		case LANGUAGE_SPANISH: return LIT(Lang::id_to_name(LANG_ES));
		case LANGUAGE_PORTUGUESE: return LIT(Lang::id_to_name(LANG_PT));
		case LANGUAGE_POLISH: return LIT(Lang::id_to_name(LANG_PL));
		case LANGUAGE_RUSSIAN: return LIT(Lang::id_to_name(LANG_RU));
		case LANGUAGE_KOREAN: return LIT(Lang::id_to_name(LANG_KO));
		case LANGUAGE_CHINESE_TRADITIONAL: return LIT((const char*)u8"Chinese (Traditional) - 繁體中文");
		case LANGUAGE_JAPANESE: return LIT(Lang::id_to_name(LANG_JA));
		case LANGUAGE_MEXICAN: return LIT("Spanish (Mexican) - Española (Mexicana)");
		case LANGUAGE_CHINESE_SIMPLIFIED: return LIT(Lang::id_to_name(LANG_ZH));
		}
		return NOLABEL;
	}

	CommandSetGameLanguage::CommandSetGameLanguage(CommandList* const parent)
		: CommandListAction(parent, LOC("SGMELNG"), CMDNAMES_OBF("gamelang"), LOC("SGMELNG_H"), {
			{ LANGUAGE_CHINESE_SIMPLIFIED, getLanguageMenuName(LANGUAGE_CHINESE_SIMPLIFIED), CMDNAMES_SHRT("zh") },
			{ LANGUAGE_CHINESE_TRADITIONAL, getLanguageMenuName(LANGUAGE_CHINESE_TRADITIONAL), CMDNAMES_SHRT("zht") },
			{ LANGUAGE_ENGLISH, getLanguageMenuName(LANGUAGE_ENGLISH), CMDNAMES_SHRT("en") },
			{ LANGUAGE_FRENCH, getLanguageMenuName(LANGUAGE_FRENCH), CMDNAMES_SHRT("fr") },
			{ LANGUAGE_GERMAN, getLanguageMenuName(LANGUAGE_GERMAN), CMDNAMES_SHRT("de") },
			{ LANGUAGE_ITALIAN, getLanguageMenuName(LANGUAGE_ITALIAN), CMDNAMES_SHRT("it") },
			{ LANGUAGE_JAPANESE, getLanguageMenuName(LANGUAGE_JAPANESE), CMDNAMES_SHRT("ja") },
			{ LANGUAGE_KOREAN, getLanguageMenuName(LANGUAGE_KOREAN), CMDNAMES_SHRT("ko") },
			{ LANGUAGE_POLISH, getLanguageMenuName(LANGUAGE_POLISH), CMDNAMES_SHRT("pl") },
			{ LANGUAGE_PORTUGUESE, getLanguageMenuName(LANGUAGE_PORTUGUESE), CMDNAMES_SHRT("pt") },
			{ LANGUAGE_RUSSIAN, getLanguageMenuName(LANGUAGE_RUSSIAN), CMDNAMES_SHRT("ru") },
			{ LANGUAGE_SPANISH, getLanguageMenuName(LANGUAGE_SPANISH), CMDNAMES_SHRT("es") },
			{ LANGUAGE_MEXICAN, getLanguageMenuName(LANGUAGE_MEXICAN), CMDNAMES_SHRT("esmx") },
		})
	{
	}

	void CommandSetGameLanguage::onItemClick(Click& click, CommandListActionItem* item)
	{
		ensureYieldableScriptThread(click, [item]
		{
			const auto lang = (rage::sysLanguage)item->value;

			// CLanguageSelect::ShowLanguageSelectScreen - B9 14 00 00 00 E8 ? ? ? ? 39 1D ? ? ? ? 74

			pointers::CPauseMenu_SetMenuPreference(PREF_CURRENT_LANGUAGE, lang);
			pointers::CProfileSettings_Set(*pointers::profile_settings, CProfileSettings::DISPLAY_LANGUAGE, lang);
			pointers::CProfileSettings_Set(*pointers::profile_settings, CProfileSettings::NEW_DISPLAY_LANGUAGE, lang);
			pointers::rage_rlSetLanguage(lang);

			// Force game to reload TheText instantly
			pointers::CPauseMenu_UpdateProfileFromMenuOptions(true);
		});
	}
}
