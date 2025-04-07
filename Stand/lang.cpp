#include "lang.hpp"

#include "LangBuiltins.hpp"
#include "LangData.hpp"
#include "LangId.hpp"
#include "ParachuteModels.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	static LangDataMap lang_data_en(&LangBuiltins::en);
	static LangDataMap lang_data_de(&LangBuiltins::de);
	static LangDataMap lang_data_nl(&LangBuiltins::nl);
	static LangDataMap lang_data_lt(&LangBuiltins::lt);
	static LangDataMap lang_data_pt(&LangBuiltins::pt);
	static LangDataMap lang_data_zh(&LangBuiltins::zh);
	static LangDataMap lang_data_ko(&LangBuiltins::ko);
	static LangDataMap lang_data_ru(&LangBuiltins::ru);
	static LangDataMap lang_data_es(&LangBuiltins::es);
	static LangDataMap lang_data_fr(&LangBuiltins::fr);
	static LangDataMap lang_data_tr(&LangBuiltins::tr);
	static LangDataMap lang_data_pl(&LangBuiltins::pl);
	static LangDataMap lang_data_it(&LangBuiltins::it);
	static LangDataMap lang_data_ja(&LangBuiltins::ja);
	static LangDataMap lang_data_vi(&LangBuiltins::vi);

	static LangDataMap lang_data_sex(&LangBuiltins::sex);

	static LangEnUs lang_data_enus(&LangBuiltins::en);
	static LangUwu lang_data_uwu(&LangBuiltins::en);
	static LangUwu lang_data_hornyuwu(&LangBuiltins::sex);

	lang_t Lang::active_id = LANG_EN;
	const LangData* Lang::active_data = &lang_data_en;

	lang_t Lang::default_id = LANG_EN;

	void Lang::setActive(lang_t id)
	{
		active_id = id;
		active_data = id_to_data(id);

		generate_parachute_models();
	}

	bool Lang::activeIsLocalised()
	{
		return active_id != LANG_EN;
	}

	static std::unordered_set<hash_t> stand_keys{};

	void Lang::ensureStandKeys()
	{
		if (stand_keys.empty())
		{
			stand_keys.reserve(LangBuiltins::en.size());
			for (const auto& e : LangBuiltins::en)
			{
				stand_keys.emplace(e.first);
			}
		}
	}

	const std::unordered_set<hash_t>& Lang::getStandKeys()
	{
		ensureStandKeys();
		return stand_keys;
	}

	bool Lang::doesLabelWithKeyExist(hash_t key)
	{
		return LangBuiltins::en.contains(key);
	}

	// Needed by vk_string_map
	std::string Lang::get(const char* key) noexcept
	{
		return active_data->get(key);
	}

	// Needed by CommandListVpnNodes
	std::string Lang::get(const std::string& key) noexcept
	{
		return active_data->get(key);
	}

	std::string Lang::get(const hash_t key) noexcept
	{
		return active_data->get(key);
	}

	std::string Lang::getXor(const hash_t key) noexcept
	{
		return get(key ^ MAGIC_LANG_GET);
	}

	std::wstring Lang::getW(const hash_t key) noexcept
	{
		return active_data->getW(key);
	}

	std::wstring Lang::getXorW(const hash_t key) noexcept
	{
		return getW(key ^ MAGIC_LANG_GET_W);
	}

	std::string Lang::get_en(const hash_t key) noexcept
	{
		LangDataMap data(&LangBuiltins::en);
		return data.get(key);
	}

	std::string Lang::getXorEn(const hash_t key) noexcept
	{
		return get_en(key ^ MAGIC_LANG_GET_EN);
	}

	bool Lang::isEnabled(const lang_t lang_id) noexcept
	{
		return !id_to_map(lang_id)->empty();
	}

	const LangData* Lang::id_to_data(const lang_t lang_id) noexcept
	{
		switch (lang_id)
		{
		case LANG_DE: return &lang_data_de;
		case LANG_NL: return &lang_data_nl;
		case LANG_LT: return &lang_data_lt;
		case LANG_PT: return &lang_data_pt;
		case LANG_ZH: return &lang_data_zh;
		case LANG_KO: return &lang_data_ko;
		case LANG_RU: return &lang_data_ru;
		case LANG_ES: return &lang_data_es;
		case LANG_FR: return &lang_data_fr;
		case LANG_TR: return &lang_data_tr;
		case LANG_PL: return &lang_data_pl;
		case LANG_IT: return &lang_data_it;
		case LANG_JA: return &lang_data_ja;
		case LANG_VI: return &lang_data_vi;

		case LANG_SEX: return &lang_data_sex;

		case LANG_ENUS: return &lang_data_enus;
		case LANG_UWU: return &lang_data_uwu;
		case LANG_HORNYUWU: return &lang_data_hornyuwu;
		}
		return &lang_data_en;
	}

	std::unordered_map<hash_t, std::wstring>* Lang::id_to_map(const lang_t lang_id) noexcept
	{
		switch (lang_id)
		{
		case LANG_DE: return &LangBuiltins::de;
		case LANG_NL: return &LangBuiltins::nl;
		case LANG_LT: return &LangBuiltins::lt;
		case LANG_PT: return &LangBuiltins::pt;
		case LANG_ZH: return &LangBuiltins::zh;
		case LANG_KO: return &LangBuiltins::ko;
		case LANG_RU: return &LangBuiltins::ru;
		case LANG_ES: return &LangBuiltins::es;
		case LANG_FR: return &LangBuiltins::fr;
		case LANG_TR: return &LangBuiltins::tr;
		case LANG_PL: return &LangBuiltins::pl;
		case LANG_IT: return &LangBuiltins::it;
		case LANG_JA: return &LangBuiltins::ja;
		case LANG_VI: return &LangBuiltins::vi;

		case LANG_SEX: case LANG_HORNYUWU: return &LangBuiltins::sex;
		}
		return &LangBuiltins::en;
	}

#define ID_TO_CODE_IMPL(prefix) \
switch (lang_id) \
{ \
	case LANG_DE: return prefix ## "de"; \
	case LANG_NL: return prefix ## "nl"; \
	case LANG_LT: return prefix ## "lt"; \
	case LANG_PT: return prefix ## "pt"; \
	case LANG_ZH: return prefix ## "zh"; \
	case LANG_KO: return prefix ## "ko"; \
	case LANG_RU: return prefix ## "ru"; \
	case LANG_ES: return prefix ## "es"; \
	case LANG_FR: return prefix ## "fr"; \
	case LANG_TR: return prefix ## "tr"; \
	case LANG_PL: return prefix ## "pl"; \
	case LANG_IT: return prefix ## "it"; \
	case LANG_JA: return prefix ## "ja"; \
	case LANG_VI: return prefix ## "vi"; \
} \
return prefix ## "en";

	const char* Lang::idToCodeA(const lang_t lang_id) noexcept
	{
#define EMPTY_MACRO
		ID_TO_CODE_IMPL(EMPTY_MACRO); // avoid warning because no parameter to 1-parameter function-like macro
	}

	const wchar_t* Lang::idToCodeW(const lang_t lang_id) noexcept
	{
		ID_TO_CODE_IMPL(L);
	}

	const char* Lang::idToApiCode(const lang_t lang_id) noexcept
	{
		switch (lang_id)
		{
		case LANG_ENUS:
			return "en-us";

		case LANG_SEX:
			return "sex";

		case LANG_UWU:
			return "uwu";

		case LANG_HORNYUWU:
			return "hornyuwu";
		}
		return idToCodeA(lang_id);
	}

	const char* Lang::idToCodeForRockstar(const lang_t lang_id) noexcept
	{
		if (lang_id == LANG_JA)
		{
			return "jp";
		}
		return idToCodeA(lang_id);
	}

	const char* Lang::idToCodeForSoup(const lang_t lang_id) noexcept
	{
		switch (lang_id)
		{
		case LANG_DE:
			return "DE";
		case LANG_NL:
			return "NL";
		case LANG_LT:
			return "LT";
		case LANG_PT:
			return "PT";
		case LANG_ZH:
			return "ZH-CN";
		case LANG_KO:
			return "KO";
		case LANG_RU:
			return "RU";
		case LANG_ES:
			return "ES";
		case LANG_FR:
			return "FR";
		case LANG_TR:
			return "TR";
		case LANG_PL:
			return "PL";
		case LANG_IT:
			return "IT";
		case LANG_JA:
			return "JA";
		case LANG_VI:
			return "VI";
		}
		return "EN";
	}

	const char* Lang::activeToCodeUpperForGeoip() noexcept
	{
		return idToCodeForSoup(active_id);
	}

	lang_t Lang::code_to_id(const std::string& code) noexcept
	{
		switch (rage::atStringHash(code))
		{
		case ATSTRINGHASH("de"):
			return LANG_DE;
		case ATSTRINGHASH("nl"):
			return LANG_NL;
		case ATSTRINGHASH("lt"):
			return LANG_LT;
		case ATSTRINGHASH("pt"):
			return LANG_PT;
		case ATSTRINGHASH("zh"):
			return LANG_ZH;
		case ATSTRINGHASH("ko"):
			return LANG_KO;
		case ATSTRINGHASH("ru"):
			return LANG_RU;
		case ATSTRINGHASH("es"):
			return LANG_ES;
		case ATSTRINGHASH("fr"):
			return LANG_FR;
		case ATSTRINGHASH("tr"):
			return LANG_TR;
		case ATSTRINGHASH("pl"):
			return LANG_PL;
		case ATSTRINGHASH("it"):
			return LANG_IT;
		case ATSTRINGHASH("ja"):
			return LANG_JA;
		case ATSTRINGHASH("vi"):
			return LANG_VI;
		}
		return LANG_EN;
	}

	const char* Lang::id_to_name(const lang_t lang_id) noexcept
	{
		switch (lang_id)
		{
		case LANG_DE:
			return (const char*)u8"German - Deutsch";
		case LANG_NL:
			return (const char*)u8"Dutch - Nederlands";
		case LANG_LT:
			return (const char*)u8"Lithuanian - Lietuvių";
		case LANG_PT:
			return (const char*)u8"Portuguese - Português";
		case LANG_ZH:
			return (const char*)u8"Chinese (Simplified) - 简体中文";
		case LANG_KO:
			return (const char*)u8"Korean - 한국어";
		case LANG_RU:
			return (const char*)u8"Russian - русский";
		case LANG_ES:
			return (const char*)u8"Spanish - Español";
		case LANG_FR:
			return (const char*)u8"French - Français";
		case LANG_TR:
			return (const char*)u8"Turkish - Türkçe";
		case LANG_PL:
			return (const char*)u8"Polish - Polski";
		case LANG_IT:
			return (const char*)u8"Italian - Italiana";
		case LANG_JA:
			return (const char*)u8"Japanese - 日本語";
		case LANG_VI:
			return (const char*)u8"Vietnamese - Tiếng Việt";

		case LANG_SEX:
			return (const char*)u8"Horny English";

		case LANG_ENUS:
			return (const char*)u8"English (US)";
		case LANG_UWU:
			return (const char*)u8"Engwish";
		case LANG_HORNYUWU:
			return (const char*)u8"Howny Engwish";
		}
		return (const char*)u8"English (UK)";
	}

	lang_t Lang::name_to_id(const std::string& name) noexcept
	{
		lang_t i = 0;
		for (; i != LANG_SIZE; ++i)
		{
			if (name == id_to_name(i))
			{
				break;
			}
		}
		return i;
	}

	lang_t Lang::apiCodeToId(const char* name) noexcept
	{
		lang_t i = 0;
		for (; i != LANG_SIZE; ++i)
		{
			if (strcmp(idToApiCode(i), name) == 0)
			{
				break;
			}
		}
		return i;
	}

	bool Lang::isSupportedByRockstar(const lang_t lang_id) noexcept
	{
		// https://support.rockstargames.com/xx/services/status.json

		switch (lang_id)
		{
		case LANG_EN:
		case LANG_DE:
		case LANG_ZH:
		case LANG_RU:
		case LANG_ES:
		case LANG_FR:
		case LANG_PL:
		case LANG_IT:
		case LANG_JA:
			return true;
		}
		return false;
	}

	lang_t Lang::getActiveLangForRockstar() noexcept
	{
		if (isSupportedByRockstar(active_id))
		{
			return active_id;
		}
		return LANG_EN;
	}

	lang_t Lang::isSupportedByWebInterface(const lang_t lang_id) noexcept
	{
		switch (lang_id)
		{
		case LANG_ENUS:
		case LANG_UWU:
		case LANG_HORNYUWU:
			return false;
		}
		return true;
	}

	void Lang::deinit()
	{
		for (lang_t i = LANG_EN; i != LANG_REAL_END; ++i)
		{
			Lang::id_to_map(i)->clear();
		}

		stand_keys.clear();
	}
}
