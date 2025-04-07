#pragma once

#include "CommandListSelectPedModels.hpp"

#include "fwddecl.hpp"
#include "natives_decl.hpp"

namespace Stand
{
	class BodyguardMgr
	{
	public:
		inline static CommandList* list = nullptr;
		inline static CommandSlider* count = nullptr;
		inline static CommandToggle* immortal = nullptr;
		inline static CommandToggle* teleport_when_far = nullptr;
		inline static CommandListSelectPedModels* model = nullptr;
		inline static CommandListSelect* primary_weapon = nullptr;
		inline static CommandListSelect* secondary_weapon = nullptr;
		inline static CommandListSelect* formation = nullptr;
		inline static CommandListSelect* relationship = nullptr;
		inline static CommandDivider* divider = nullptr;

		inline static CommandToggle* ignore_players = nullptr;
		inline static CommandToggle* ignore_friends = nullptr;
		inline static CommandToggle* ignore_authorities = nullptr;
		inline static CommandToggle* ignore_crew_members = nullptr;
		inline static CommandToggle* ignore_organisation_members = nullptr;

		static void push();
		static void clear();
		static void removeDivider();
		static void postDeleteMember();
		static void summon(const bool extra_distance = false);
		static void associate(AbstractEntity& ped, Hash weapon, uint8_t group);
		static void onTick();

		[[nodiscard]] static int getGroup();
		[[nodiscard]] static Hash getWeapon(AbstractEntity& bodyguard, bool primary);
		[[nodiscard]] static bool isWeaponValid(Hash weapon, AbstractEntity& bodyguard, AbstractEntity& veh);
		[[nodiscard]] static Hash getBestWeapon(Hash primary, Hash secondary, AbstractEntity& bodyguard);

		[[nodiscard]] static bool isPedBodyguard(AbstractEntity& ped);
	};
}