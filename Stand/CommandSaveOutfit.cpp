#include "CommandSaveOutfit.hpp"

#include "AbstractEntity.hpp"
#include "CommandListRefreshable.hpp"
#include "lang.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandSaveOutfit::CommandSaveOutfit(CommandList* const parent)
		: CommandListSave(parent, LOC("SVEOTFT"), { CMDNAME("saveoutfit") })
	{
	}

	std::wstring CommandSaveOutfit::getFolder() const
	{
		return std::move(std::wstring(_wgetenv(L"appdata")).append(LR"(\Stand\Outfits\)"));
	}

	std::unordered_map<std::string, std::string> CommandSaveOutfit::getDataEn() const
	{
		return Util::get_colons_outfit_from_ped(g_player_ped);
	}

	bool CommandSaveOutfit::canEntryBeOmitted(hash_t key) const
	{
		return key != ATSTRINGHASH("MDL");
	}

	std::vector<hash_t> CommandSaveOutfit::getDataOrderRaw() const
	{
		return Util::getOutfitDataOrderRaw();
	}

	void CommandSaveOutfit::onSaved() const
	{
		parent->as<CommandListRefreshable>()->refresh();
	}
}
