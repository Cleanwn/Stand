#include "CommandRidTools.hpp"

#include "CommandDivider.hpp"

#include "CommandNameJoin.hpp"
#include "CommandNameSpectate.hpp"
#include "CommandNameInvite.hpp"
#include "CommandNameProfile.hpp"
//#include "CommandNameKick.hpp"
#include "CommandRidJoin.hpp"
#include "CommandRidSpectate.hpp"
#include "CommandRidInvite.hpp"
#include "CommandRidProfile.hpp"
//#include "CommandRidKick.hpp"
#ifdef STAND_DEBUG
#include "CommandGsinfoJoin.hpp"
#endif

namespace Stand
{
	CommandRidTools::CommandRidTools(CommandList* const parent)
		: CommandList(parent, LOC("RIDTOOLS"))
	{
		this->createChild<CommandDivider>(LOC("NME"));
		this->createChild<CommandNameJoin>();
#if CAN_SPECTATE
		this->createChild<CommandNameSpectate>();
#endif
		this->createChild<CommandNameInvite>();
		this->createChild<CommandNameProfile>();
		//this->createChild<CommandNameKick>();
		this->createChild<CommandDivider>(LIT("Rockstar ID"));
		this->createChild<CommandRidJoin>();
#if CAN_SPECTATE
		this->createChild<CommandRidSpectate>();
#endif
		this->createChild<CommandRidInvite>();
		this->createChild<CommandRidProfile>();
		//this->createChild<CommandRidKick>();
#ifdef STAND_DEBUG
		this->createChild<CommandDivider>(LIT("gsinfo"));
		this->createChild<CommandGsinfoJoin>();
#endif
	}
}
