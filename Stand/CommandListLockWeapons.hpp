#pragma once

#include "CommandListWeapons.hpp"

namespace Stand
{
	class CommandListLockWeapons : public CommandListWeapons
	{
	public:
		time_t time_since_last_transition = 0;
		std::vector<hash_t> weapons = {};
		CommandToggle* invert = nullptr;

		explicit CommandListLockWeapons(CommandList* const parent);

		void onTickEnabled();

		void giveWeapons() const;
		void changeWeapon(hash_t weapon, bool state);
		void createCommand(CommandList* list, Label&& menu_name, std::string&& config_name, std::vector<hash_t>&& weapons) final;

		[[nodiscard]] bool shouldHaveWeapon(hash_t weapon) const;
		[[nodiscard]] std::vector<hash_t> getAllOfOurWeapons() const;
	};
}