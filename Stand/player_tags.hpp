#pragma once

#include "fwddecl.hpp"
#include "typedecl.hpp"

#include "conf_classifier.hpp"

namespace Stand
{
	// Don't forget to update CommandPlayer::flag_chars

	enum PlayerTag : playerflag_t
	{
		FLAG_SELF,
		FLAG_FRIEND,
		FLAG_CREW,
		FLAG_TIMEOUT,
		FLAG_MOD,
#if HAS_MODAMN
		FLAG_MODAMN,
#endif
		FLAG_AMN,
		FLAG_LIKELY_MODDER,
		FLAG_ATTACKED,
		FLAG_HOST,
		FLAG_NEXT_HOST,
		FLAG_SCRIPT_HOST,
		FLAG_GOD,
		FLAG_VEHGOD,
		FLAG_INVEH,
		FLAG_RC,
		FLAG_OTR,
		FLAG_INT,
		FLAG_SPEC,
		FLAG_TYPING,
		FLAG_DEAD,
		FLAG_INVISIBLE,
		FLAG_VPN,
		FLAG_ORG_MEMBER,

		FLAG_STAND_USER,
		FLAG_GEOIP,

		FLAG_COUNT
	};

	[[nodiscard]] extern Label PlayerTag_getLabel(playerflag_t f);
	[[nodiscard]] extern Label PlayerTag_getPluralLabel(playerflag_t f);
}

static_assert(HAS_MODAMN == false); // need to manually change the comment in FOR_ALL_FLAGS!

#define FOR_ALL_FLAGS(f) \
f(FLAG_SELF, "FLAG_SELF"); \
f(FLAG_FRIEND, "FLAG_FRIEND"); \
f(FLAG_CREW, "FLAG_CREW"); \
f(FLAG_ORG_MEMBER, "FLAG_ORG_MEMBER"); \
f(FLAG_TIMEOUT, "FLAG_TIMEOUT"); \
f(FLAG_MOD, "FLAG_MOD"); \
/*f(FLAG_MODAMN, "FLAG_MODAMN");*/ \
f(FLAG_AMN, "FLAG_AMN"); \
f(FLAG_LIKELY_MODDER, "CLSFN_LM"); \
f(FLAG_ATTACKED, "FLAG_ATTACKED"); \
f(FLAG_HOST, "FLAG_HOST"); \
f(FLAG_NEXT_HOST, "NXTHST"); \
f(FLAG_SCRIPT_HOST, "FLAG_SCRIPT_HOST"); \
f(FLAG_GOD, "FLAG_GOD"); \
f(FLAG_VEHGOD, "FLAG_VEHGOD"); \
f(FLAG_INVEH, "FLAG_INVEH"); \
f(FLAG_RC, "FLAG_RC"); \
f(FLAG_OTR, "FLAG_OTR"); \
f(FLAG_INT, "FLAG_INT"); \
f(FLAG_SPEC, "FLAG_SPEC"); \
f(FLAG_TYPING, "FLAG_TYPING"); \
f(FLAG_DEAD, "FLAG_DEAD"); \
f(FLAG_INVISIBLE, "FLAG_INVISIBLE"); \
f(FLAG_VPN, "FLAG_VPN");
