#include "CommandDiscord.hpp"

#include <discord_rpc.h>

#include <soup/ObfusString.hpp>
#include <soup/rand.hpp>

#include "Gui.hpp"
#include "main.hpp" // g_windows_7_or_older
#include "Util.hpp"

namespace Stand
{
	CommandDiscord::CommandDiscord(CommandList* const parent)
		: CommandToggle(parent, LOC("DISCRP"), { CMDNAME("discordrichpresence"), CMDNAME("discordrpc") })
	{
		instance = this;
	}

	void CommandDiscord::onEnable(Click& click)
	{
		if (g_windows_7_or_older)
		{
			Util::toast(LIT_OBF("This command is not available on Windows 7 or older."));
			return onChangeImplUnavailable(click);
		}
		updatePresence();
	}

	void CommandDiscord::onDisable(Click& click)
	{
		if (!g_windows_7_or_older)
		{
			Discord_ClearPresence();
		}
	}

	void CommandDiscord::update() const
	{
		if (m_on)
		{
			updatePresence();
		}
	}

	static std::string common_details[] = {
		soup::ObfusString("A Unique Mod Menu").str(),
		soup::ObfusString("Most Polished Mod Menu").str(),
		soup::ObfusString("Most Efficient Mod Menu").str(),
		soup::ObfusString("A Mod Menu Made With Love").str(),
	};

	static std::string rare_details[] = {
		soup::ObfusString("BEST MODE MANU!").str(),
		//soup::ObfusString("SAFEST MENU EVER EXISTED!").str(),
	};

	void CommandDiscord::updatePresence()
	{
		if (!g_windows_7_or_older)
		{
			DiscordRichPresence discordPresence;
			memset(&discordPresence, 0, sizeof(discordPresence));
			if (soup::rand.one_in(100))
			{
				discordPresence.details = ARRAY_RAND(rare_details).c_str();
			}
			else
			{
				discordPresence.details = ARRAY_RAND(common_details).c_str();
			}
			discordPresence.state = soup::ObfusString("www.stand.sh").c_str();
			discordPresence.largeImageKey = "icon";
			discordPresence.largeImageText = g_gui.root_list->menu_name.literal_str.c_str();
			Discord_UpdatePresence(&discordPresence);
		}
	}
}
