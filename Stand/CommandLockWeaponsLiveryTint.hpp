#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
	class CommandLockWeaponsLiveryTint : public CommandListSelect
	{
	public:
		explicit CommandLockWeaponsLiveryTint(CommandList* const parent, const CommandName& command_names_prefix)
			: CommandListSelect(parent, LOC("WEAPLIVTNT"), combineCommandNames(command_names_prefix, CMDNAMES("liverytint")), NOLABEL, {
				{ 0, LOC("CN_GR"), CMDNAMES("grey", "gray")},
				{ 1, LOC("CN_GR_D"), CMDNAMES("darkgrey", "darkgray")},
				{ 2, LOC("CN_L"), CMDNAMES("black") },
				{ 3, LOC("CN_W"), CMDNAMES("white") },
				{ 4, LOC("CN_B"), CMDNAMES("blue") },
				{ 5, LOC("CN_C"), CMDNAMES("cyan") },
				{ 6, LOC("CN_AQUA"), CMDNAMES("aqua") },
				{ 7, LOC("CN_B_COOL"), CMDNAMES("coolblue") },
				{ 8, LOC("CN_B_D"), CMDNAMES("darkblue") },
				{ 9, LOC("CN_B_ROYAL"), CMDNAMES("royalblue") },
				{ 10, LOC("CN_PLUM"), CMDNAMES("plum") },
				{ 11, LOC("CN_U_D"), CMDNAMES("darkpurple") },
				{ 12, LOC("CN_U"), CMDNAMES("purple") },
				{ 13, LOC("CN_R"), CMDNAMES("red") },
				{ 14, LOC("CN_R_WINE"), CMDNAMES("winered") },
				{ 15, LOC("CN_M"), CMDNAMES("magenta") },
				{ 16, LOC("CN_P"), CMDNAMES("pink") },
				{ 17, LOC("CN_SALMON"), CMDNAMES("salmon") },
				{ 18, LOC("CN_P_H"), CMDNAMES("hotpink") },
				{ 19, LOC("CN_O_RUST"), CMDNAMES("rustorange") },
				{ 20, LOC("CN_BR"), CMDNAMES("brown") },
				{ 21, LOC("CN_EARTH"), CMDNAMES("earth") },
				{ 22, LOC("CN_O"), CMDNAMES("orange") },
				{ 23, LOC("DN_O_L"), CMDNAMES("lightorange") },
				{ 24, LOC("CN_Y_D"), CMDNAMES("darkyellow") },
				{ 25, LOC("CN_Y"), CMDNAMES("yellow") },
				{ 26, LOC("CN_BR_L"), CMDNAMES("lightbrown") },
				{ 27, LOC("CN_G_LIME"), CMDNAMES("limegreen") },
				{ 28, LOC("CN_OLIVE"), CMDNAMES("olive") },
				{ 29, LOC("CN_MOSS"), CMDNAMES("moss") },
				{ 30, LOC("CN_TUR"), CMDNAMES("turqouise") },
				{ 31, LOC("CN_G_D"), CMDNAMES("darkgreen") },
			}, 0)
		{
		}
	};
}