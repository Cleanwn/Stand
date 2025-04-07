#pragma once

#include "typedecl.hpp"

namespace Stand
{
	enum LangId : lang_t
	{
		LANG_EN = 0,
		LANG_DE,
		LANG_NL,
		LANG_LT,
		LANG_PT,
		LANG_ZH,
		LANG_KO,
		LANG_RU,
		LANG_ES,
		LANG_FR,
		LANG_TR,
		LANG_PL,
		LANG_IT,
		LANG_JA,
		LANG_VI,

		// Below: lang.is_english
		LANG_SEX,

		// Below: lang.is_automatically_translated
		LANG_REAL_END,

		LANG_ENUS = LANG_REAL_END,
		LANG_UWU,
		LANG_HORNYUWU,

		LANG_SIZE,
	};
}
