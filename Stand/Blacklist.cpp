#include "Blacklist.hpp"

#include "AbstractPlayer.hpp"
#include "Auth.hpp"
#include "Codename.hpp"
#include "Exceptional.hpp"
#include "Util.hpp"

namespace Stand
{
	bool Blacklist::isAdvertiserV2(const rage::rlGamerInfo& gi)
	{
		bool ret = false;
		EXCEPTIONAL_LOCK(mtx)
		for (const auto& a : advertisers)
		{
			if (a->appliesTo(gi))
			{
				ret = true;
				break;
			}
		}
		EXCEPTIONAL_UNLOCK(mtx)
		return ret;
	}

	bool Blacklist::isAdvertiserByRid(int64_t rid)
	{
		bool ret = false;
		EXCEPTIONAL_LOCK(mtx)
		for (const auto& a : advertisers)
		{
			if (a->appliesTo(rid))
			{
				ret = true;
				break;
			}
		}
		EXCEPTIONAL_UNLOCK(mtx)
		return ret;
	}

	void Blacklist::addAdvertiser(const rage::rlGamerInfo& gi, uint8_t ad_level)
	{
		// Does the remote care?
		if (g_auth.hasApiCredentials()
			&& !isAdvertiserV2(gi)
			)
		{
			auto gid = GamerIdentifierV4::fromGamerInfo(gi);

			// Did we not report them before or is it a higher ad level this time?
			auto e = reported_advertisers.find(gid.xored_and_scrambled_rockstar_id);
			if (e == reported_advertisers.end()
				|| e->second < ad_level
				)
			{
				// Inform remote
				if (has_remote_data)
				{
					std::string data = gid.toString();
					data.append(fmt::to_string(ad_level));
					g_auth.reportEvent("B3", std::move(data));
				}

				// Update local data
				if (e != reported_advertisers.end())
				{
					e->second = ad_level;
				}
				else
				{
					reported_advertisers.emplace(gid.xored_and_scrambled_rockstar_id, ad_level);
				}
			}
		}
	}
}
