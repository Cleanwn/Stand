#include "CommandListSelectVehicleModel.hpp"

#include "AbstractModel.hpp"
#include "get_vehicle_display_name.hpp"

namespace Stand
{
	std::vector<Hash> CommandListSelectVehicleModel::disallowAllExcept(Hash cls)
	{
		constexpr Hash all_classes[] = {
			ATSTRINGHASH("super"),
			ATSTRINGHASH("service"),
			ATSTRINGHASH("utility"),
			ATSTRINGHASH("helicopter"),
			ATSTRINGHASH("motorcycle"),
			ATSTRINGHASH("suv"),
			ATSTRINGHASH("plane"),
			ATSTRINGHASH("sport"),
			ATSTRINGHASH("emergency"),
			ATSTRINGHASH("military"),
			ATSTRINGHASH("sport_classic"),
			ATSTRINGHASH("compacts"),
			ATSTRINGHASH("sedan"),
			ATSTRINGHASH("boat"),
			ATSTRINGHASH("commercial"),
			ATSTRINGHASH("off_road"),
			ATSTRINGHASH("van"),
			ATSTRINGHASH("muscle"),
			ATSTRINGHASH("cycle"),
			ATSTRINGHASH("industrial"),
			ATSTRINGHASH("rail"),
			ATSTRINGHASH("coupe"),
			ATSTRINGHASH("open_wheel")
		};

		std::vector<Hash> res;

		for (auto hash : all_classes)
		{
			if (hash != cls)
			{
				res.emplace_back(hash);
			}
		}

		return res;
	}

	bool CommandListSelectVehicleModel::checkShouldUseCategories(std::vector<std::pair<Hash, const VehicleItem*>>& vehicles)
	{
		return std::adjacent_find(vehicles.cbegin(), vehicles.cend(), [](const std::pair<Hash, const VehicleItem*>& a, const std::pair<Hash, const VehicleItem*>& b)
		{
			return a.second->clazz != b.second->clazz;
		}) != vehicles.cend();
	}

	std::vector<CommandListActionItemData> CommandListSelectVehicleModel::getOptions(const bool include_random, const bool include_none, const std::vector<hash_t>& disallowed_classes, Disallows disallows)
	{
		std::vector<std::pair<Hash, const VehicleItem*>> vehicles{};
		std::vector<CommandListActionItemData> options{};
		if (include_none)
		{
			options.emplace_back(CommandListActionItemData(0, LOC("NONE")));
		}
		if (include_random)
		{
			options.emplace_back(CommandListActionItemData(-1, LOC("RNG")));
		}
		for (const VehicleItem* vehicle = &vehicle_items[0]; vehicle != &vehicle_items[COUNT(vehicle_items)]; vehicle++)
		{
			auto model_hash = rage::atStringHash(vehicle->name);
			if (isAllowed(vehicle, model_hash, disallowed_classes, disallows))
			{
				vehicles.emplace_back(std::make_pair(model_hash, vehicle));
			}
		}
		const bool use_categories = checkShouldUseCategories(vehicles);
		for (const auto& [hash, vehicle] : vehicles)
		{
			options.emplace_back(CommandListActionItemData(
				hash,
				LIT(get_vehicle_display_name_no_manufacturer(hash)),
				{},
				LIT(fmt::format("{}: {}", LANG_GET("MDL"), vehicle->name)),
				COMMANDPERM_USERONLY,
				CMDFLAG_FEATURELIST_FINISHLIST,
				use_categories ? LOC_RT(vehicle->clazz) : NOLABEL,
				true
			));
		}
		return options;
	}

	CommandListSelectVehicleModel::CommandListSelectVehicleModel(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, long long default_value, bool include_random, bool include_none, std::vector<hash_t>&& disallowed_classes, Disallows disallows)
		: CommandListSelect(parent, std::move(menu_name), std::move(command_names), std::move(help_text), getOptions(include_random, include_none, disallowed_classes, disallows), default_value), disallowed_classes(std::move(disallowed_classes)), disallows(disallows)
	{
	}

	hash_t CommandListSelectVehicleModel::getVehicle() const
	{
		if (this->value != -1)
		{
			return (hash_t)this->value;
		}

		const VehicleItem* vehicle;
		hash_t model_hash;
		do
		{
			vehicle = &ARRAY_RAND(vehicle_items);
			model_hash = rage::atStringHash(vehicle->name);
		} while (!isAllowed(vehicle, model_hash, disallowed_classes, disallows));
		return model_hash;
	}

	bool CommandListSelectVehicleModel::isAllowed(const VehicleItem* vehicle, hash_t model_hash, const std::vector<hash_t>& disallowed_classes, Disallows disallows)
	{
		if (std::find(disallowed_classes.begin(), disallowed_classes.end(), vehicle->clazz) != disallowed_classes.end())
		{
			return false;
		}

		AbstractModel model = AbstractModel(model_hash);

		if ((disallows & Disallows::ZERO_SEATERS)
			&& model.getNumSeats() == 0
			)
		{
			return false;
		}

		if ((disallows & Disallows::BIG_VEHICLES)
			&& model.isBigVehicle()
			)
		{
			return false;
		}

		return true;
	}
}
