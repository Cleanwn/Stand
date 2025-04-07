#include "CommandSelectControllerKey.hpp"

namespace Stand
{
	[[nodiscard]] static std::vector<CommandListActionItemData> getOptions(bool has_none)
	{
		std::vector<CommandListActionItemData> options{};
		options.reserve(16 + (has_none ? 1 : 0));
		if (has_none)
		{
			options.emplace_back(-1, LOC("NONE"), CMDNAMES("none"));
		}
		options.emplace_back(INPUT_FRONTEND_ACCEPT, LIT("A"), CMDNAMES("a"));
		options.emplace_back(INPUT_FRONTEND_CANCEL, LIT("B"), CMDNAMES("b"));
		options.emplace_back(INPUT_FRONTEND_X, LIT("X"), CMDNAMES("x"));
		options.emplace_back(INPUT_FRONTEND_Y, LIT("Y"), CMDNAMES("y"));
		options.emplace_back(INPUT_FRONTEND_LB, LOC("CK_LB"), CMDNAMES("lb", "l1"));
		options.emplace_back(INPUT_FRONTEND_RB, LOC("CK_RB"), CMDNAMES("rb", "r1"));
		options.emplace_back(INPUT_FRONTEND_LT, LOC("CK_LT"), CMDNAMES("lt", "l2"));
		options.emplace_back(INPUT_FRONTEND_RT, LOC("CK_RT"), CMDNAMES("rt", "r2"));
		options.emplace_back(INPUT_FRONTEND_LS, LOC("CK_LS"), CMDNAMES("ls", "l3"));
		options.emplace_back(INPUT_FRONTEND_RS, LOC("CK_RS"), CMDNAMES("rs", "r3"));
		options.emplace_back(INPUT_FRONTEND_UP, LOC("CK_D_U"), CMDNAMES("dup"));
		options.emplace_back(INPUT_FRONTEND_DOWN, LOC("CK_D_D"), CMDNAMES("ddown"));
		options.emplace_back(INPUT_FRONTEND_LEFT, LOC("CK_D_L"), CMDNAMES("dleft"));
		options.emplace_back(INPUT_FRONTEND_RIGHT, LOC("CK_D_R"), CMDNAMES("dright"));
		options.emplace_back(INPUT_FRONTEND_PAUSE, LOC("CK_STRT"), CMDNAMES("start"));
		options.emplace_back(INPUT_FRONTEND_SELECT, LOC("CK_BCK"), CMDNAMES("back"));
		return options;
	}

	CommandSelectControllerKey::CommandSelectControllerKey(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, ControlInput default_value, bool has_none)
		: CommandListSelect(parent, std::move(menu_name), std::move(command_names), NOLABEL, getOptions(has_none), default_value, CMDFLAGS_LIST_SELECT, CMDFLAG_FEATURELIST_FINISHLIST)
	{
	}
}
