#pragma once

#include <string>

#include <soup/UniquePtr.hpp>

#include "fwddecl.hpp"
#include "gta_fwddecl.hpp"

namespace Stand
{
	class GamerIdentifier
	{
	public:
		bool is_on_devdbg;

	protected:
		void unpackAuthoritativeData(std::string& str);

	public:
		[[nodiscard]] static soup::UniquePtr<GamerIdentifier> fromString(std::string str);

		[[nodiscard]] bool appliesTo(AbstractPlayer p) const;
		[[nodiscard]] bool appliesTo(const rage::rlGamerInfo& gi) const;
		[[nodiscard]] virtual bool appliesTo(int64_t rid) const = 0;
	};

	// Supported since 99.5, with leading zero patch since 100.1.
	struct GamerIdentifierV4 : public GamerIdentifier
	{
		uint64_t xored_and_scrambled_rockstar_id;

		[[nodiscard]] static GamerIdentifierV4 fromGamerInfo(const rage::rlGamerInfo& gamer_info);
		[[nodiscard]] static GamerIdentifierV4 fromRockstarId(int64_t rid);
		[[nodiscard]] static GamerIdentifierV4 fromString(std::string str);
		[[nodiscard]] static GamerIdentifierV4 fromStringWithoutVersion(std::string str);

		[[nodiscard]] std::string toString() const; // up to 14 chars
		[[nodiscard]] std::string toStringWithoutVersion() const; // up to 13 chars

		using GamerIdentifier::appliesTo;
		[[nodiscard]] bool appliesTo(int64_t rid) const final;

		[[nodiscard]] int64_t getRockstarId() const;
	};

	using GamerIdentifierLatest = GamerIdentifierV4;
}
