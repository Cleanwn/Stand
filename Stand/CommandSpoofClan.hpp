#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandSpoofClanId;
	class CommandSpoofClanTag;
	class CommandSpoofClanName;
	class CommandSpoofClanMotto;
	class CommandSpoofClanAlt;
	class CommandSpoofClanColour;

	class CommandSpoofClan : public CommandList
	{
	private:
		CommandSpoofClanId* id;
		CommandSpoofClanTag* tag;
		CommandSpoofClanName* name;
		CommandSpoofClanMotto* motto;
		CommandSpoofClanAlt* alt;
		CommandSpoofClanColour* colour;

	public:
		inline static CommandSpoofClan* instance;

		explicit CommandSpoofClan(CommandList* const parent);

		[[nodiscard]] static Label getClanLabel(const char* const tag, const std::string& name, const bool alt_badge);

		void setValuesFromHooking(Click& click) const;
		static void updateData();
	};
}
