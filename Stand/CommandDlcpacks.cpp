#include "CommandDlcpacks.hpp"

#include "CommandDlcpack.hpp"
#include "CommandLink.hpp"
#include "CommandListAssetHashes.hpp"
#include "CommandTogglePointer.hpp"
#include "Hooking.hpp"

namespace Stand
{
	CommandDlcpacks::CommandDlcpacks(CommandList* const parent)
		: CommandListFolder(parent, StringUtils::utf8_to_utf16(soup::ObfusString("Custom DLCs")), {}, LOC("CUSTDLC"), {}, LOC("CUSTDLC_H"))
	{
	}

	void CommandDlcpacks::onClick(Click& click)
	{
		if (click.isRegularEditionOrSp())
		{
			CommandListFolder::onClick(click);
		}
	}

	void CommandDlcpacks::populateHead()
	{
		createOpenFolderAction(LOC("CUSTDLCS_OPNFLD"));
		createChild<CommandLink>(CommandListAssetHashes::instance->fix);

		// Spoof Models
		{
			auto list = createChild<CommandList>(LOC("CUSTDLC_SPF"), {});

			list->createChild<CommandTogglePointer>(&g_hooking.spoof_custom_models, LOC("CUSTDLC_SPF"), {}, LOC("CUSTDLC_SPF_H"));

			{
				const auto vehicle = list->createChild<CommandListSelectVehicleModel>(LOC("VEHS"), CMDNAMES("spoofmodelveh"), NOLABEL, ATSTRINGHASH("adder"), false, false, std::vector<Hash>{ ATSTRINGHASH("rail"), ATSTRINGHASH("boat"), ATSTRINGHASH("helicopter"), ATSTRINGHASH("plane"), ATSTRINGHASH("cycle") }, CommandListSelectVehicleModel::ZERO_SEATERS);
				const auto motor = list->createChild<CommandListSelectVehicleModel>(LOC("motorcycle"), CMDNAMES("spoofmodelmotorbike"), NOLABEL, ATSTRINGHASH("ruffian"), false, false, CommandListSelectVehicleModel::disallowAllExcept(ATSTRINGHASH("motorcycle")));
				const auto plane = list->createChild<CommandListSelectVehicleModel>(LOC("plane"), CMDNAMES("spoofmodelplane"), NOLABEL, ATSTRINGHASH("molotok"), false, false, CommandListSelectVehicleModel::disallowAllExcept(ATSTRINGHASH("plane")));
				const auto heli = list->createChild<CommandListSelectVehicleModel>(LOC("helicopter"), CMDNAMES("spoofmodelheli"), NOLABEL, ATSTRINGHASH("buzzard"), false, false, CommandListSelectVehicleModel::disallowAllExcept(ATSTRINGHASH("helicopter")));
				const auto boat = list->createChild<CommandListSelectVehicleModel>(LOC("boat"), CMDNAMES("spoofmodelboat"), NOLABEL, ATSTRINGHASH("squalo"), false, false, CommandListSelectVehicleModel::disallowAllExcept(ATSTRINGHASH("boat")));
				const auto cycle = list->createChild<CommandListSelectVehicleModel>(LOC("cycle"), CMDNAMES("spoofmodelcycle"), NOLABEL, ATSTRINGHASH("bmx"), false, false, CommandListSelectVehicleModel::disallowAllExcept(ATSTRINGHASH("cycle")));

				// Included entries with nullptr = don't bother spoofing, unincluded entries = spoof to a constant model inside getSpoofForVehicleType since there's no point in having variety
				model_spoofs.emplace(VEHICLE_TYPE_VEHICLE, vehicle);
				model_spoofs.emplace(VEHICLE_TYPE_PLANE, plane);
				// model_spoofs.emplace(VEHICLE_TYPE_TRAILER, nullptr);
				model_spoofs.emplace(VEHICLE_TYPE_QUADBIKE, motor);
				model_spoofs.emplace(VEHICLE_TYPE_DRAFT, nullptr);
				model_spoofs.emplace(VEHICLE_TYPE_SUBMARINECAR, vehicle);
				model_spoofs.emplace(VEHICLE_TYPE_AMPHIBIOUS_AUTOMOBILE, vehicle);
				model_spoofs.emplace(VEHICLE_TYPE_AMPHIBIOUS_QUADBIKE, motor);
				model_spoofs.emplace(VEHICLE_TYPE_HELI, heli);
				// model_spoofs.emplace(VEHICLE_TYPE_BLIMP, nullptr);
				model_spoofs.emplace(VEHICLE_TYPE_AUTOGYRO, nullptr);
				model_spoofs.emplace(VEHICLE_TYPE_BIKE, motor);
				model_spoofs.emplace(VEHICLE_TYPE_BICYCLE, cycle);
				model_spoofs.emplace(VEHICLE_TYPE_BOAT, cycle);
				// model_spoofs.emplace(VEHICLE_TYPE_TRAIN, nullptr);
				// model_spoofs.emplace(VEHICLE_TYPE_SUBMARINE, nullptr);
			}
		}
	}

	CommandListFolderItemData* CommandDlcpacks::getItemData(Command* cmd) const noexcept
	{
		return getItemDataImpl<CommandDlcpack>(cmd);
	}

	void CommandDlcpacks::createItem(CommandList* list, std::wstring&& filename_no_ext, CommandListFolderItemData&& data) const
	{
		list->createChild<CommandDlcpack>(std::move(filename_no_ext), std::move(data));
	}

	std::optional<Hash> CommandDlcpacks::getSpoofForVehicleType(VehicleType type)
	{
		if (const auto spoof = model_spoofs.find(type); spoof != model_spoofs.end())
		{
			if (spoof->second != nullptr)
			{
				return (Hash)spoof->second->value;
			}
		}
		else
		{
			switch (type)
			{
			case VEHICLE_TYPE_BLIMP:
				return ATSTRINGHASH("blimp");

			case VEHICLE_TYPE_TRAIN:
				return ATSTRINGHASH("freight");

			case VEHICLE_TYPE_TRAILER:
				return ATSTRINGHASH("trailers2");

			case VEHICLE_TYPE_SUBMARINE:
				return ATSTRINGHASH("kosatka");
			}
		}

		return std::nullopt;
	}
}
