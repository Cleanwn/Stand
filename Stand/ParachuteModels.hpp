#pragma once

#include <vector>

#include "atStringHash.hpp"
#include "Label.hpp"

namespace Stand
{
	struct ParachuteModelVariation
	{
		int idx{};
		Label name{};
	};

	struct ParachuteModel
	{
		hash_t model{};
		std::vector<ParachuteModelVariation> variations{};
		//bool ped_applicable{};

		constexpr ParachuteModel() noexcept = default;

		constexpr ParachuteModel(hash_t model, std::vector<ParachuteModelVariation>&& variations) noexcept
			: model(model), variations(std::move(variations))//, ped_applicable(variations.size() == 1)
		{
		}

		constexpr ParachuteModel(hash_t model, Label&& name, bool ped_applicable) noexcept
			: model(model), variations(1, ParachuteModelVariation{ 0, std::move(name) })//, ped_applicable(ped_applicable)
		{
		}
	};

	inline ParachuteModel g_parachute_models[6]{};
	extern void generate_parachute_models();
}
