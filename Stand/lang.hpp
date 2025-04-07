#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "fwddecl.hpp"
#include "typedecl.hpp"

#include "atStringHash.hpp"
#include "xormagics.hpp"

#define LANG_GET(key) Lang::getXor(ATSTRINGHASH(key) ^ MAGIC_LANG_GET)
#define LANG_GET_W(key) Lang::getXorW(ATSTRINGHASH(key) ^ MAGIC_LANG_GET_W)
#define LANG_GET_EN(key) Lang::getXorEn(ATSTRINGHASH(key) ^ MAGIC_LANG_GET_EN)
#define LANG_FMT(key, ...) fmt::format(fmt::runtime(LANG_GET(key)), __VA_ARGS__)
#define LANG_FMT_W(key, ...) fmt::format(fmt::runtime(LANG_GET_W(key)), __VA_ARGS__)

namespace Stand
{
	struct Lang
	{
		static lang_t active_id;
		static const LangData* active_data;
		static void setActive(lang_t id);
		[[nodiscard]] static bool activeIsLocalised();

		static lang_t default_id;

		static void ensureStandKeys();
		[[nodiscard]] static const std::unordered_set<hash_t>& getStandKeys();

		[[nodiscard]] static bool doesLabelWithKeyExist(hash_t key);

		[[nodiscard]] static std::string get(const char* key) noexcept;
		[[nodiscard]] static std::string get(const std::string& key) noexcept;
		[[nodiscard]] static std::string get(const hash_t key) noexcept;
		[[nodiscard]] static __declspec(noinline) std::string getXor(const hash_t key) noexcept;

		[[nodiscard]] static std::wstring getW(const hash_t key) noexcept;
		[[nodiscard]] static __declspec(noinline) std::wstring getXorW(const hash_t key) noexcept;

		[[nodiscard]] static std::string get_en(const hash_t key) noexcept;
		[[nodiscard]] static __declspec(noinline) std::string getXorEn(const hash_t key) noexcept;

		[[nodiscard]] static bool isEnabled(const lang_t lang_id) noexcept;
		[[nodiscard]] static const LangData* id_to_data(const lang_t lang_id) noexcept;
		[[nodiscard]] static std::unordered_map<hash_t, std::wstring>* id_to_map(const lang_t lang_id) noexcept;
		[[nodiscard]] static const char* idToCodeA(const lang_t lang_id) noexcept;
		[[nodiscard]] static const wchar_t* idToCodeW(const lang_t lang_id) noexcept;
		[[nodiscard]] static const char* idToApiCode(const lang_t lang_id) noexcept;
		[[nodiscard]] static const char* idToCodeForRockstar(const lang_t lang_id) noexcept;
		[[nodiscard]] static const char* idToCodeForSoup(const lang_t lang_id) noexcept;
		[[nodiscard]] static const char* activeToCodeUpperForGeoip() noexcept;
		[[nodiscard]] static lang_t code_to_id(const std::string& code) noexcept;
		[[nodiscard]] static const char* id_to_name(const lang_t lang_id) noexcept;
		[[nodiscard]] static lang_t name_to_id(const std::string& name) noexcept;
		[[nodiscard]] static lang_t apiCodeToId(const char* name) noexcept;
		[[nodiscard]] static bool isSupportedByRockstar(const lang_t lang_id) noexcept;
		[[nodiscard]] static lang_t getActiveLangForRockstar() noexcept;
		[[nodiscard]] static lang_t isSupportedByWebInterface(const lang_t lang_id) noexcept;

		static void deinit();
	};
}
