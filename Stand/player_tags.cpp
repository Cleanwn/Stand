#include "player_tags.hpp"

#include "Label.hpp"

namespace Stand
{
	Label PlayerTag_getLabel(playerflag_t f)
	{
#define GETLABEL_CASE(tag, name) case tag: return LOC(name);
		switch (f)
		{
			FOR_ALL_FLAGS(GETLABEL_CASE)
		}
		return NOLABEL;
	}

	Label PlayerTag_getPluralLabel(playerflag_t f)
	{
		switch (f)
		{
		case FLAG_FRIEND: return LOC("FLAG_FRIEND_P");
		case FLAG_CREW: return LOC("FLAG_CREW_P");
		case FLAG_MOD: return LOC("FLAG_MOD_P");
		case FLAG_AMN: return LOC("FLAG_AMN_P");
		case FLAG_LIKELY_MODDER: return LOC("CLSFN_LM_P");
		case FLAG_ATTACKED: return LOC("FLAG_ATTACKED_P");
		}
		return PlayerTag_getLabel(f);
	}
}
