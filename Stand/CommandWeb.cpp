#include "CommandWeb.hpp"

#include <soup/ObfusString.hpp>

#include "Auth.hpp"

#include "CommandWebOpen.hpp"

namespace Stand
{
	CommandWeb::CommandWeb(CommandList* const parent)
		: CommandListQrcode(parent, LOC("WEB2"), {}, LOC("WEB2_H"))
	{
		this->createChild<CommandWebOpen>();
	}

	std::string CommandWeb::getText() const
	{
		if (!g_relay.isRunning()
			&& g_auth.license_permissions >= LICPERM_REGULAR
			)
		{
			g_relay.setServerAndInit();
		}
		return getUri();
	}

	std::string CommandWeb::getUri()
	{
		std::string uri = soup::ObfusString("https://stand.sh/web#");
		uri.append(g_auth.activation_key_to_try);
		//uri.push_back('@');
		//uri.append(g_relay.server);
		return uri;
	}
}
