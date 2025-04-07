#include "GamerIdentifier.hpp"

#include <soup/CustomEncoding.hpp>
#include <soup/StringReader.hpp>
#include <soup/StringWriter.hpp>

#include "AbstractPlayer.hpp"
#include "rlGamerInfo.hpp"

namespace Stand
{
	void GamerIdentifier::unpackAuthoritativeData(std::string& str)
	{
		if (is_on_devdbg = (str.back() == '#'))
		{
			str.pop_back();
		}
	}

	soup::UniquePtr<GamerIdentifier> GamerIdentifier::fromString(std::string str)
	{
		if (str.front() == 'H')
		{
			str.erase(0, 1);
			return soup::make_unique<GamerIdentifierV4>(GamerIdentifierV4::fromStringWithoutVersion(std::move(str)));
		}
		return {};
	}

	bool GamerIdentifier::appliesTo(AbstractPlayer p) const
	{
		return appliesTo(p.getRockstarId());
	}

	bool GamerIdentifier::appliesTo(const rage::rlGamerInfo& gi) const
	{
		return appliesTo(gi.getHandle().rockstar_id);
	}

	// MySQL is case-insensitive, so not using base64. Also having a custom encoding further avoids people reading the data.
	using GIV3Encoding = soup::CustomEncoding<"K2NPFQHAZLXTJGBV1O5YRSWM803764ED9UIC">;

#define MAGIC_GI_V4_RID 0x872e797cd366135d

	[[nodiscard]] static uint64_t rid_reversible_scramble(uint64_t val)
	{
		const uint16_t xorkey = ((uint16_t*)&val)[0];
		((uint16_t*)&val)[1] ^= xorkey;
		((uint16_t*)&val)[2] ^= xorkey;
		((uint16_t*)&val)[3] ^= xorkey;
		return val;
	}

	GamerIdentifierV4 GamerIdentifierV4::fromGamerInfo(const rage::rlGamerInfo& gamer_info)
	{
		return fromRockstarId(gamer_info.getHandle().rockstar_id);
	}

	GamerIdentifierV4 GamerIdentifierV4::fromRockstarId(int64_t rid)
	{
		GamerIdentifierV4 gi;
		gi.xored_and_scrambled_rockstar_id = rid_reversible_scramble(rid ^ MAGIC_GI_V4_RID);
		return gi;
	}

	GamerIdentifierV4 GamerIdentifierV4::fromString(std::string str)
	{
		SOUP_ASSERT(str.front() == 'H');
		str.erase(0, 1);
		return fromStringWithoutVersion(std::move(str));
	}

	GamerIdentifierV4 GamerIdentifierV4::fromStringWithoutVersion(std::string str)
	{
		GamerIdentifierV4 gi;
		gi.unpackAuthoritativeData(str);
		str = GIV3Encoding::decode(std::move(str));
		while (str.length() < 8)
		{
			str.insert(0, 1, 0);
		}
		soup::StringReader sr(std::move(str));
		sr.u64_be(gi.xored_and_scrambled_rockstar_id);
		return gi;
	}

	std::string GamerIdentifierV4::toString() const
	{
		auto str = toStringWithoutVersion();
		str.insert(0, 1, 'H');
		return str;
	}

	std::string GamerIdentifierV4::toStringWithoutVersion() const
	{
		soup::StringWriter sw;
		sw.u64_be(const_cast<uint64_t&>(xored_and_scrambled_rockstar_id));
		return GIV3Encoding::encode(std::move(sw.data));
	}

	bool GamerIdentifierV4::appliesTo(int64_t rid) const
	{
		return rid == getRockstarId();
	}

	int64_t GamerIdentifierV4::getRockstarId() const
	{
		return rid_reversible_scramble(xored_and_scrambled_rockstar_id) ^ MAGIC_GI_V4_RID;
	}
}
