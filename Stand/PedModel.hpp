#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Label.hpp"
#include "String2Hash.hpp"

namespace Stand
{
	struct PedModel
	{
		String2Hash hash;
		const Label menu_name;
		std::vector<CommandName> command_names;
		const hash_t category;

		constexpr PedModel(const char* model_name, Label&& menu_name, std::vector<CommandName>&& command_names, hash_t category)
			: hash(model_name), menu_name(std::move(menu_name)), command_names(std::move(command_names)), category(category)
		{
		}

		static std::vector<PedModel> all;

		[[nodiscard]] static const PedModel* fromHash(hash_t hash) noexcept;
	};
}
