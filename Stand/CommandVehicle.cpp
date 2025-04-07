#include "CommandVehicle.hpp"

#include <fmt/format.h>

#include <soup/ObfusString.hpp>

#include "AbstractModel.hpp"
#include "CommandSpawnVehicle.hpp"
#include "conf.hpp"
#include "get_vehicle_display_name.hpp"
#include "lang.hpp"
#include "OnlineVersion.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandVehicle::CommandVehicle(CommandList* const parent, const VehicleItem* data)
		: CommandWithEntityPreview(parent, LIT(get_vehicle_display_name_no_manufacturer(data)), std::vector<CommandName>(1, utf8ToCmdName(data->hash_name)), NOLABEL, CMDFLAGS_ACTION | CMDFLAG_SEARCH_FINISHLIST | CMDFLAG_FEATURELIST_FINISHLIST, COMMANDPERM_SPAWN), data(data)
	{
		SOUP_IF_UNLIKELY (data->dlc.empty())
		{
			return;
		}
		const auto model_hash = rage::atStringHash(data->hash_name);
		help_text.literal_str.append(LANG_GET("VEH_SEATS")).append(": ").append(fmt::to_string(AbstractModel(model_hash).getNumSeats())).push_back('\n');
		help_text.literal_str.append(LANG_GET("VEH_CLASS")).append(": ").append(Lang::get(data->clazz)).push_back('\n');
		help_text.literal_str.append(LANG_GET("DLC")).append(": ").append(data->dlc.getVersionString()).push_back(' ');
		help_text.literal_str.append(data->dlc.getName().getLocalisedUtf8());
	}

	std::string CommandVehicle::getNameForConfig() const
	{
		return get_vehicle_display_name_no_manufacturer(data);
	}

	void CommandVehicle::updateName(bool root_readlocked, bool show_manufacturer)
	{
		setMenuName(LIT(show_manufacturer ? get_vehicle_display_name(data) : get_vehicle_display_name_no_manufacturer(data)));
	}

	void CommandVehicle::onClick(Click& click)
	{
		std::wstring args = cmdNameToUtf16(command_names.at(0));
		return CommandSpawnVehicle::instance->onCommand(click, args);
	}

	Hash CommandVehicle::getModelHash()
	{
		return rage::atStringHash(data->hash_name);
	}

	AbstractEntity CommandVehicle::createPreviewEntity()
	{
		AbstractEntity veh = Util::createVehicle(getModelHash(), {}, 0.0f, false);
		veh.setPlateText(soup::ObfusString(STAND_NAME).str());
		CommandSpawnVehicle::instance->applySpawnTuning(veh);
		return veh;
	}
}
