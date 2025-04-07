#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandSpoofClanPreset : public CommandAction
	{
	private:
		const int64_t id;
		const char* const tag;
		const std::string name;
		const char* const motto;
		const bool alt_badge;

	public:
		explicit CommandSpoofClanPreset(CommandList* const parent, const int64_t id, Label&& menu_name, const char* const tag, std::string&& name, const char* const motto, const bool alt_badge = false)
			: CommandAction(parent, std::move(menu_name)), id(id), tag(tag), name(std::move(name)), motto(motto), alt_badge(alt_badge)
		{
		}

		explicit CommandSpoofClanPreset(CommandList* const parent, const int64_t id, const char* const tag, std::string&& name, const char* const motto, const bool alt_badge = false)
			: CommandSpoofClanPreset(parent, id, CommandSpoofClan::getClanLabel(tag, name, alt_badge), tag, std::move(name), motto, alt_badge)
		{
		}

		void onClick(Click& click) final
		{
			g_hooking.spoofed_clan_membership.clan.id = id;
			strcpy(g_hooking.spoofed_clan_membership.clan.tag, tag);
			strcpy(g_hooking.spoofed_clan_membership.clan.name, name.c_str());
			strcpy(g_hooking.spoofed_clan_membership.clan.motto, motto);
			g_hooking.spoofed_clan_membership.clan.alt_badge = alt_badge;

			parent->parent->as<CommandSpoofClan>()->setValuesFromHooking(click);
		}
	};
}
