#pragma once

#include <soup/macros.hpp>

#include "atStringHash.hpp"
#include "ePedVarComp.hpp"

namespace Stand
{
	struct PedComponent
	{
		const int id;
		hash_t menu_name;
		CommandName command_name;
		hash_t variation_menu_name;
		CommandName variation_command_name;
		hash_t online_menu_name = menu_name;
		CommandName online_command_name = command_name;
		hash_t online_variation_menu_name = variation_menu_name;
		CommandName online_variation_command_name = variation_command_name;
	};

	struct PedProp
	{
		const int id;
		hash_t menu_name;
		CommandName command_name;
		hash_t variation_menu_name;
		CommandName variation_command_name;
	};

	inline const PedComponent ped_components[] = {
		{PV_COMP_HEAD, ATSTRINGHASH("HEAD"), CMDNAME("head"), ATSTRINGHASH("HEAD_V"), CMDNAME("headvar")},
		{PV_COMP_BERD, ATSTRINGHASH("FACIALHAIR"), CMDNAME("face"), ATSTRINGHASH("FACIALHAIR_V"), CMDNAME("facevar"), ATSTRINGHASH("MASK"), CMDNAME("mask"), ATSTRINGHASH("MASK_V"), CMDNAME("maskvar")},
		{PV_COMP_HAIR, ATSTRINGHASH("HAIR"), CMDNAME("hair"), ATSTRINGHASH("HAIR_V"), CMDNAME("hairvar")},
		{PV_COMP_UPPR, ATSTRINGHASH("TOP1"), CMDNAME("top"), ATSTRINGHASH("TOP1_V"), CMDNAME("topvar"), ATSTRINGHASH("TORSO"), CMDNAME("gloves"), ATSTRINGHASH("TORSO_V"), CMDNAME("glovesvar")},
		{PV_COMP_JBIB, ATSTRINGHASH("TOP2"), CMDNAME("top2"), ATSTRINGHASH("TOP2_V"), CMDNAME("top2var"), ATSTRINGHASH("TOP1"), CMDNAME("top"), ATSTRINGHASH("TOP1_V"), CMDNAME("topvar")},
		{PV_COMP_ACCS, ATSTRINGHASH("TOP3"), CMDNAME("top3"), ATSTRINGHASH("TOP3_V"), CMDNAME("top3var"), ATSTRINGHASH("TOP2"), CMDNAME("top2"), ATSTRINGHASH("TOP2_V"), CMDNAME("top2var")},
		{PV_COMP_TASK, ATSTRINGHASH("BAG"), CMDNAME("bag"), ATSTRINGHASH("BAG_V"), CMDNAME("bagvar"), ATSTRINGHASH("TOP3"), CMDNAME("top3"), ATSTRINGHASH("TOP3_V"), CMDNAME("top3var")},
		{PV_COMP_HAND, ATSTRINGHASH("GLOVES"), CMDNAME("gloves"), ATSTRINGHASH("GLOVES_V"), CMDNAME("glovesvar"), ATSTRINGHASH("PCHUTE"), CMDNAME("parachute"), ATSTRINGHASH("PCHUTE_V"), CMDNAME("parachutevar")},
		{PV_COMP_LOWR, ATSTRINGHASH("PANTS"), CMDNAME("pants"), ATSTRINGHASH("PANTS_V"), CMDNAME("pantsvar")},
		{PV_COMP_FEET, ATSTRINGHASH("SHOES"), CMDNAME("shoes"), ATSTRINGHASH("SHOES_V"), CMDNAME("feetvar")},
		{PV_COMP_TEEF, ATSTRINGHASH("ACCESSORIES"), CMDNAME("accessories"), ATSTRINGHASH("ACCESSORIES_V"), CMDNAME("accessoriesvar")},
		{PV_COMP_DECL, ATSTRINGHASH("DECALS"), CMDNAME("decals"), ATSTRINGHASH("DECALS_V"), CMDNAME("decalsvar")},
	};

	inline const PedProp ped_props[] = {
		{0, ATSTRINGHASH("HAT"), CMDNAME("hat"), ATSTRINGHASH("HAT_V"), CMDNAME("hatvar")},
		{1, ATSTRINGHASH("GLASSES"), CMDNAME("glasses"), ATSTRINGHASH("GLASSES_V"), CMDNAME("glassesvar")},
		{2, ATSTRINGHASH("EARWEAR"), CMDNAME("ears"), ATSTRINGHASH("EARWEAR_V"), CMDNAME("earsvar")},
		{6, ATSTRINGHASH("WATCH"), CMDNAME("watch"), ATSTRINGHASH("WATCH_V"), CMDNAME("watchvar")},
		{7, ATSTRINGHASH("BRACELET"), CMDNAME("bracelet"), ATSTRINGHASH("BRACELET_V"), CMDNAME("braceletvar")},
	};
}

#define NUM_PED_COMPONENTS COUNT(ped_components)
#define NUM_PED_PROPS COUNT(ped_props)

#define COMPACT_OUTFIT_LEGACY_SIZE ((NUM_PED_COMPONENTS + NUM_PED_PROPS) * 2)
// components + props + hair highlight colour
#define COMPACT_OUTFIT_SIZE (COMPACT_OUTFIT_LEGACY_SIZE + 1)
