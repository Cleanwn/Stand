#pragma once

#include "CommandListSelect.hpp"

#include "vehicle_items.hpp"

namespace Stand
{
	class CommandListSelectVehicleModel : public CommandListSelect
	{
	public:
		enum Disallows : uint8_t
		{
			ALL_ALLOWED = 0,
			ZERO_SEATERS = 0b1,
			BIG_VEHICLES = 0b10
		};

		[[nodiscard]] static std::vector<Hash> disallowAllExcept(Hash cls);

	private:
		static bool checkShouldUseCategories(std::vector<std::pair<Hash, const VehicleItem*>>& vehicles);
		[[nodiscard]] static std::vector<CommandListActionItemData> getOptions(const bool include_random, const bool include_none, const std::vector<hash_t>& disallowed_classes, Disallows disallows);

	public:
		enum : long long
		{
			RANDOM = -1,
			NONE
		};

		std::vector<hash_t> disallowed_classes;
		Disallows disallows;

		explicit CommandListSelectVehicleModel(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, long long default_value, bool include_random = false, bool include_none = false, std::vector<hash_t>&& disallowed_classes = {}, Disallows disallows = Disallows::ALL_ALLOWED);

		[[nodiscard]] hash_t getVehicle() const;

		[[nodiscard]] static bool isAllowed(const VehicleItem* vehicle, hash_t model_hash, const std::vector<hash_t>& disallowed_classes, Disallows disallows);
	};
}