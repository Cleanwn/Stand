#include "CommandOutfit.hpp"

#include <fstream>

#include <fmt/core.h>

#include "AbstractEntity.hpp"
#include "AbstractPlayer.hpp"
#include "AbstractModel.hpp"
#include "ColonsAndTabs.hpp"
#include "CommandListFolder.hpp"
#include "lang.hpp"
#include "PedType.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandOutfit::CommandOutfit(CommandList* const parent, std::wstring&& outfit_name, CommandListFolderItemData&& folder_item_data)
		: CommandWithEntityPreview(parent, LIT(outfit_name), { CMDNAME("outfit") }, NOLABEL, CMDFLAG_SEARCH_FINISHLIST | CMDFLAG_FEATURELIST_SKIP), outfit_name(std::move(outfit_name)), folder_item_data(std::move(folder_item_data))
	{
#if COMPACT_COMMAND_NAMES
		StringUtils::simplify(command_names.at(0).append(StringUtils::utf16_to_utf8(this->outfit_name)));
#else
		StringUtils::simplify(command_names.at(0).append(this->outfit_name));
#endif
	}

	bool CommandOutfit::ensureOutfit()
	{
		if (!outfit.empty())
		{
			return true;
		}
		std::ifstream file_in(getFilePath());
		ColonsAndTabs::read(file_in, outfit);
		if (outfit.empty())
		{
			return false;
		}
		if (outfit.find("Model") != outfit.end())
		{
			made_for_model = Util::string_to_ped_model_hash(outfit.at("Model"));
		}
		return true;
	}

	void CommandOutfit::onTickInGameViewport()
	{
		if (!receivesOnTickInWebViewportThisTick())
		{
			onTickInWebViewport();
		}
		CommandWithEntityPreview::onTickInGameViewport();
	}

	void CommandOutfit::onTickInWebViewport()
	{
		if (ensureOutfit())
		{
			const bool current_model = (made_for_model == g_player_ped.getModel());
			if (current_model_indicator != current_model)
			{
				current_model_indicator = current_model;
				if (current_model_indicator)
				{
					auto str = StringUtils::utf16_to_utf8(outfit_name);
					str.append(" [").append(LANG_GET("CURMDL"));
					str.push_back(']');
					setMenuName(LIT(std::move(str)));
				}
				else
				{
					setMenuName(LIT(outfit_name));
				}
			}
		}
	}

	void CommandOutfit::onClickScriptThread(Click& click)
	{
		this->outfit.clear();
		if (!ensureOutfit())
		{
			getCommandListFolder()->refresh();
		}
		else if (outfit.find("Model") == outfit.end())
		{
			click.setResponse(LOC("LDOTFT_E1")); // "Outfit didn't specify which model it was made for. :?"
		}
		else
		{
			Util::loadOutfit(this, click, outfit);
		}
	}

	Hash CommandOutfit::getModelHash()
	{
		if (ensureOutfit())
		{
			return made_for_model;
		}
		return 0;
	}

	AbstractEntity CommandOutfit::createPreviewEntity()
	{
		auto ped = Util::createPed(PED_TYPE_MISSION, getModelHash(), {}, 0.0f, false);
		if (ped.getModel().isOnlineCharacter() && NETWORK::NETWORK_HAS_CACHED_PLAYER_HEAD_BLEND_DATA(g_player))
		{
			NETWORK::NETWORK_APPLY_CACHED_PLAYER_HEAD_BLEND_DATA(ped, g_player);
		}
		Util::applyOutfit(ped, outfit);
		return ped;
	}
}
