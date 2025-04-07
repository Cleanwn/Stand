#pragma once

#include <unordered_map>

#include <soup/base.hpp>

#include "hashtype.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class LangData
	{
	protected:
		const std::unordered_map<hash_t, std::wstring>* const map;

	public:
		explicit LangData(const std::unordered_map<hash_t, std::wstring>* const map) noexcept;
		virtual ~LangData() = default;

		[[nodiscard]] std::string get(const char* key) const noexcept;
		[[nodiscard]] std::string get(const std::string& key) const noexcept;

		[[nodiscard]] __forceinline std::string get(const hash_t key) const noexcept
		{
			return StringUtils::utf16_to_utf8(getW(key));
		}

		[[nodiscard]] __forceinline std::wstring getW(const hash_t key) const noexcept
		{
			auto res = getImpl(key);
			SOUP_IF_LIKELY (res.has_value())
			{
				return std::move(res.value());
			}
			return getFallbackString();
		}

	protected:
		[[nodiscard]] static std::wstring getFallbackString() noexcept;

		[[nodiscard]] virtual std::optional<std::wstring> getImpl(const hash_t key) const noexcept = 0;
	};

	class LangDataMap : public LangData
	{
	public:
		using LangData::LangData;

		[[nodiscard]] std::optional<std::wstring> getImpl(const hash_t key) const noexcept final;
	};

	class LangEnUs : public LangData
	{
	public:
		using LangData::LangData;

		[[nodiscard]] std::optional<std::wstring> getImpl(const hash_t key) const noexcept final;
	};

	class LangUwu : public LangData
	{
	public:
		using LangData::LangData;

		[[nodiscard]] std::optional<std::wstring> getImpl(const hash_t key) const noexcept final;
	};
}
