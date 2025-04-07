#include "Label.hpp"

#include <soup/base.hpp>

#include "lang.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"

namespace Stand
{
	const Label Label::sNoLabel = NOLABEL;

	Label::Label(const std::wstring& str, TagLiteral)
		: Label(StringUtils::utf16_to_utf8(str), TagLiteral{})
	{
	}

	Label Label::combineWithSpace(const Label& primary, const Label& secondary)
	{
		std::string str = primary.getLocalisedUtf8();
		str.push_back(' ');
		str.append(secondary.getLocalisedUtf8());
		return LIT(std::move(str));
	}

	Label Label::combineWithBrackets(const Label& primary, const Label& secondary)
	{
		std::string str = primary.getLocalisedUtf8();
		str.append(" (");
		str.append(secondary.getLocalisedUtf8());
		str.push_back(')');
		return LIT(std::move(str));
	}

	void Label::operator=(const Label& b) noexcept
	{
		hash = b.hash;
		literal_str = b.literal_str;
	}

	void Label::operator=(Label&& b) noexcept
	{
		this->hash = b.hash;
		this->literal_str = std::move(b.literal_str);
	}

	void Label::setLiteral(const std::string& str) noexcept
	{
		this->hash = 0;
		this->literal_str = str;
	}

	void Label::setLiteral(std::string&& str) noexcept
	{
		this->hash = 0;
		this->literal_str = std::move(str);
	}

	void Label::setLocalised(hash_t hash) noexcept
	{
		this->hash = hash ^ MAGIC_LABEL;
		this->literal_str.clear();
	}

	bool Label::isLiteral() const noexcept
	{
		return hash == 0;
	}

	bool Label::empty() const noexcept
	{
		return isLiteral() && literal_str.empty();
	}

	void Label::reset() noexcept
	{
		setLiteral({});
	}

	bool Label::operator==(const hash_t hash) const noexcept
	{
		return !this->isLiteral() && this->getLocalisationHash() == hash;
	}

	bool Label::operator==(const Label& b) const noexcept
	{
		if (isLiteral())
		{
			if (!b.isLiteral())
			{
				return false;
			}
			return literal_str == b.literal_str;
		}
		else
		{
			if (b.isLiteral())
			{
				return false;
			}
			return hash == b.hash;
		}
	}

	bool Label::operator!=(const Label& b) const noexcept
	{
		return !operator==(b);
	}

	bool Label::isLiteralString(const std::string& b) const noexcept
	{
		return /*isLiteral() &&*/ literal_str == b;
	}

	uint64_t Label::getHash() const noexcept
	{
		if (!isLiteral())
		{
			return hash;
		}
		return (1i64 << 32) | rage::atStringHash(literal_str);
	}

	hash_t Label::getLocalisationHash() const noexcept
	{
		return hash ^ MAGIC_LABEL;
	}

	std::string Label::getWebString() const noexcept
	{
		if (isLiteral())
		{
			return literal_str;
		}
		const auto localisation_hash = getLocalisationHash();
		SOUP_IF_LIKELY (Lang::getStandKeys().contains(localisation_hash))
		{
			return Util::to_padded_hex_string(localisation_hash);
		}
		return getLocalisedUtf8();
	}

	std::string Label::getLiteralUtf8() const noexcept
	{
#ifdef STAND_DEBUG
		SOUP_ASSERT(isLiteral());
#endif
		return literal_str;
	}

	std::wstring Label::getLiteralUtf16() const noexcept
	{
		return StringUtils::utf8_to_utf16(getLiteralUtf8());
	}

	CommandName Label::getLiteralForCommandName() const noexcept
	{
#if COMPACT_COMMAND_NAMES
		return getLiteralUtf8();
#else
		return getLiteralUtf16();
#endif
	}

    std::string Label::getLocalisedUtf8() const noexcept
    {
		return isLiteral() ? literal_str : Lang::get(getLocalisationHash());
    }

	std::wstring Label::getLocalisedUtf16() const noexcept
	{
		return isLiteral() ? StringUtils::utf8_to_utf16(literal_str) : Lang::getW(getLocalisationHash());
	}

	std::string Label::getEnglishUtf8() const noexcept
	{
		return isLiteral() ? literal_str : Lang::get_en(getLocalisationHash());
	}

	std::wstring Label::getEnglishUtf16() const noexcept
	{
		return StringUtils::utf8_to_utf16(getEnglishUtf8());
	}

	CommandName Label::getEnglishForCommandName() const noexcept
	{
#if COMPACT_COMMAND_NAMES
		return getEnglishUtf8();
#else
		return getEnglishUtf16();
#endif
	}

	void Label::makeLiteralLocalised() noexcept
	{
		if (!isLiteral())
		{
			setLiteral(Lang::get(getLocalisationHash()));
		}
	}
}
