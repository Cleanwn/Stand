#include "LangData.hpp"

#include <soup/ObfusString.hpp>

#include "lang.hpp"

namespace Stand
{
	LangData::LangData(const std::unordered_map<hash_t, std::wstring>* const map) noexcept
		: map(map)
	{
	}

	std::string LangData::get(const char* key) const noexcept
	{
		auto res = getImpl(rage::atStringHash(key));
		SOUP_IF_LIKELY (res.has_value())
		{
			return StringUtils::utf16_to_utf8(res.value());
		}
		return key;
	}

	std::string LangData::get(const std::string& key) const noexcept
	{
		auto res = getImpl(rage::atStringHash(key));
		SOUP_IF_LIKELY (res.has_value())
		{
			return StringUtils::utf16_to_utf8(res.value());
		}
		return key;
	}

	std::wstring LangData::getFallbackString() noexcept
	{
		return StringUtils::utf8_to_utf16(soup::ObfusString("/!\\ STRING NOT FOUND /!\\").str());
	}

	std::optional<std::wstring> LangDataMap::getImpl(const hash_t key) const noexcept
	{
		auto ent = map->find(key);
		if (ent != map->end())
		{
			return ent->second;
		}
		return {};
	}

	std::optional<std::wstring> LangEnUs::getImpl(const hash_t key) const noexcept
	{
		auto ent = map->find(key);
		if (ent != map->end())
		{
			std::wstring str = ent->second;

			StringUtils::replace_all(str, L"Tyre", L"Tire");

			StringUtils::replace_all(str, L"etre", L"eter"); // metres
			StringUtils::replace_all(str, L"ntre", L"nter"); // centre
			StringUtils::replace_all(str, L"ntring", L"ntering"); // centring -> centering
			// Exception: Stream

			StringUtils::replace_all(str, L"alise", L"alize"); // finalise
			// Exception: milliseconds
			/* This exception really wouldn't be a problem with \b, e.g.:
			soup::Regex r(R"(\Blis(?=(?:e|ed|ing)\b))");
			std::string str = "Finalise. Finalising. Finalised. Milliseconds.";
			r.replaceAll(str, "liz");
			std::cout << str; // "Finalize. Finalizing. Finalized. Milliseconds."
			*/

			StringUtils::replace_all(str, L"isat", L"izat"); // Organisation
			// Exception: Disable

			StringUtils::replace_all(str, L"gorise", L"gorize"); // Categorised
			StringUtils::replace_all(str, L"mise", L"mize"); // customise
			StringUtils::replace_all(str, L"nise", L"nize"); // Weaponised
			StringUtils::replace_all(str, L"itise", L"itize"); // Prioritise; not Advertiser

			StringUtils::replace_all(str, L"iour", L"ior"); // Behaviour
			StringUtils::replace_all(str, L"mour", L"mor"); // Armour
			StringUtils::replace_all(str, L"vour", L"vor"); // Favourite
			StringUtils::replace_all(str, L"logue", L"log"); // Analogue
			if (str.find(L"{") == std::wstring::npos)
			{
				StringUtils::replace_all(str, L"lour", L"lor"); // Colour
			}

			StringUtils::replace_all(str, L"Grey", L"Gray");
			StringUtils::replace_all(str, L"Aluminium", L"Aluminum");
			StringUtils::replace_all(str, L"cancelled", L"canceled");

			//StringUtils::replace_all(str, L"ynch", L"ync"); // Synching, synched, synch

			return str;
		}
		return {};
	}

	std::optional<std::wstring> LangUwu::getImpl(const hash_t key) const noexcept
	{
		std::wstring result{};
		auto ent = map->find(key);
		if (ent != map->end())
		{
			result = StringUtils::owoifyWithFmtException(ent->second);
		}
		return result;
	}
}
