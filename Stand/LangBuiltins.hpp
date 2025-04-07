#pragma once

#include <unordered_map>

#define ENABLE_LANG_EN	true
#define ENABLE_LANG_DE	true
#define ENABLE_LANG_NL	true
#define ENABLE_LANG_LT	true
#define ENABLE_LANG_PT	true
#define ENABLE_LANG_ZH	true
#define ENABLE_LANG_KO	true
#define ENABLE_LANG_RU	true
#define ENABLE_LANG_ES	true
#define ENABLE_LANG_FR	true
#define ENABLE_LANG_TR	true
#define ENABLE_LANG_PL	true
#define ENABLE_LANG_IT	true
#define ENABLE_LANG_JA	true
#define ENABLE_LANG_VI	true
#define ENABLE_LANG_SEX	true

namespace Stand
{
	struct LangBuiltins
	{
		static std::unordered_map<hash_t, std::wstring> en;
		static std::unordered_map<hash_t, std::wstring> de;
		static std::unordered_map<hash_t, std::wstring> nl;
		static std::unordered_map<hash_t, std::wstring> lt;
		static std::unordered_map<hash_t, std::wstring> pt;
		static std::unordered_map<hash_t, std::wstring> zh;
		static std::unordered_map<hash_t, std::wstring> ko;
		static std::unordered_map<hash_t, std::wstring> ru;
		static std::unordered_map<hash_t, std::wstring> es;
		static std::unordered_map<hash_t, std::wstring> fr;
		static std::unordered_map<hash_t, std::wstring> tr;
		static std::unordered_map<hash_t, std::wstring> pl;
		static std::unordered_map<hash_t, std::wstring> it;
		static std::unordered_map<hash_t, std::wstring> ja;
		static std::unordered_map<hash_t, std::wstring> vi;

		static std::unordered_map<hash_t, std::wstring> sex;
	};
}
