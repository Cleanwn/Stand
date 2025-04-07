#include "ResourceMgr.hpp"

#include "AddressGamers.hpp"
#include "ChatHistory.hpp"
#include "RemoteGamer.hpp"
#include "ScAccount.hpp"

namespace Stand
{
	void ResourceMgr::deinit()
	{
		AddressGamers::deinit();
		ChatHistory::history.clear();
		RemoteGamer::deinit();
		ScAccount::deinit();
	}
}
