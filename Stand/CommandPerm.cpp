#include "CommandPerm.hpp"

namespace Stand
{
	Label getCommandPermLabel(CommandPerm perm)
	{
		switch (perm)
		{
		case COMMANDPERM_FRIENDLY:
			return LOC("PERM_F");

		case COMMANDPERM_NEUTRAL:
			return LOC("PERM_N");

		case COMMANDPERM_SPAWN:
			return LOC("PERM_S");

		case COMMANDPERM_RUDE:
			return LOC("PERM_R");

		case COMMANDPERM_AGGRESSIVE:
			return LOC("PERM_A");

		case COMMANDPERM_TOXIC:
			return LOC("PERM_T");
		}
		return NOLABEL;
	}
}
