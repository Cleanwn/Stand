#include "RootNameMgr.hpp"

#include <soup/ObfusString.hpp>

#include "conf.hpp"
#include "Gui.hpp"

namespace Stand
{
	const std::string RootNameMgr::stock_root_name = soup::ObfusString(STAND_RN_NAMEVERSION).str();

	const std::string RootNameMgr::pre100[] = {
		soup::ObfusString("" STAND_RN_NAMEVERSION " Early Access").str(),
		soup::ObfusString("Sit " STAND_RN_VERSION).str(),
		soup::ObfusString("Stan " STAND_RN_VERSION).str(),
		soup::ObfusString("Sand " STAND_RN_VERSION).str(),
		soup::ObfusString("Kiddion's VIP Menu " STAND_RN_VERSION).str(),
	};

	const std::string RootNameMgr::all_toxic[] = {
		soup::ObfusString("Rockstar Admin Tool " STAND_RN_VERSION).str(),
		soup::ObfusString("" STAND_RN_NAMEVERSION " Femboy Edition").str(),
		soup::ObfusString("" STAND_RN_NAMEVERSION " Crash Addict Edition").str(),
		soup::ObfusString("" STAND_RN_NAMEVERSION " 崩溃小子版").str(),
	};

	const std::string RootNameMgr::early_toxic[] = {
		soup::ObfusString("iStand " STAND_RN_VERSION).str(),
		soup::ObfusString("iSit " STAND_RN_VERSION).str(),
		soup::ObfusString("" STAND_RN_NAMEVERSION " Donator Edition").str(),
		soup::ObfusString("" STAND_RN_NAMEVERSION " Penis Enlargement Edition").str(),
		soup::ObfusString("" STAND_RN_NAMEVERSION " Dickhead Edition").str(),
		soup::ObfusString("" STAND_RN_NAMEVERSION " Retard Edition").str(),
		soup::ObfusString("2Take1Menu " STAND_RN_VERSION " Superior Edition").str(),
		soup::ObfusString("" STAND_RN_NAMEVERSION " 狂拽酷炫版").str(),
		soup::ObfusString("" STAND_RN_NAMEVERSION " 捐赠者版").str(),
		soup::ObfusString("2Take1Menu " STAND_RN_VERSION " 高级版").str(),
	};

	const std::string RootNameMgr::used100[] = {
		soup::ObfusString("Stand 💯").str(),
	};

	const std::string RootNameMgr::rndbool_true[] = {
		soup::ObfusString("" STAND_RN_NAMEVERSION " Team Lavender").str(),
		soup::ObfusString("" STAND_RN_NAMEVERSION " Red Edition").str(),
		soup::ObfusString("" STAND_RN_NAMEVERSION " Model X").str(),
	};

	const std::string RootNameMgr::rndbool_false[] = {
		soup::ObfusString("" STAND_RN_NAMEVERSION " Team Ruby").str(),
		soup::ObfusString("" STAND_RN_NAMEVERSION " Blue Edition").str(),
		soup::ObfusString("" STAND_RN_NAMEVERSION " Model Y").str(),
	};

	long long RootNameMgr::getDefaultRootNameIdForState()
	{
		auto rn = getRootNameIdForState();
		if (rn != RN_NULL)
		{
			return rn;
		}
		return RN_AUTHED;
	}

	std::string RootNameMgr::getCurrentRootNameForState()
	{
		auto rn = getRootNameIdForState();
		if (rn == RN_NULL)
		{
			rn = current;
		}
		return idToString(rn);
	}

	long long RootNameMgr::getRootNameIdForState()
	{
		if (g_gui.root_state == GUI_MINIMAL)
		{
			return RN_STOCK;
		}
		if (g_gui.isRootStateFreeEdition())
		{
			return RN_FREE;
		}
		return RN_NULL;
	}

	std::string RootNameMgr::idToString(long long id)
	{
		switch (id)
		{
		case RN_NAME_ONLY:
			return soup::ObfusString(STAND_NAME);

		case RN_VERSION_ONLY:
			return STAND_RN_VERSION;

		case RN_STOCK:
			return stock_root_name;

		case RN_FREE:
			return soup::ObfusString(STAND_RN_NAMEVERSION " (Free)");

		case RN_AUTHED:
			return authed_root_name;
		}
		if (id >= RN_PRE100 && id < (RN_PRE100 + NUM_RN_PRE100))
		{
			return pre100[id - RN_PRE100];
		}
		if (id >= RN_ALL_TOXIC && id < (RN_ALL_TOXIC + NUM_RN_ALL_TOXIC))
		{
			return all_toxic[id - RN_ALL_TOXIC];
		}
		if (id >= RN_EARLY_TOXIC && id < (RN_EARLY_TOXIC + NUM_RN_EARLY_TOXIC))
		{
			return early_toxic[id - RN_EARLY_TOXIC];
		}
		if (id >= RN_USED100 && id < (RN_USED100 + NUM_RN_USED100))
		{
			return used100[id - RN_USED100];
		}
		if (id >= RN_RNDBOOL_TRUE && id < (RN_RNDBOOL_TRUE + NUM_RN_RNDBOOL_TRUE))
		{
			return rndbool_true[id - RN_RNDBOOL_TRUE];
		}
		if (id >= RN_RNDBOOL_FALSE && id < (RN_RNDBOOL_FALSE + NUM_RN_RNDBOOL_FALSE))
		{
			return rndbool_false[id - RN_RNDBOOL_FALSE] ;
		}
		SOUP_ASSERT_UNREACHABLE;
	}
}
