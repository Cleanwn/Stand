#include "CommandDevFlag.hpp"

#include "DlcMgr.hpp"
#include "pointers.hpp"
#include "rlRosCredentials.hpp"

namespace Stand
{
	CommandDevFlag::CommandDevFlag(CommandList* parent)
		: CommandToggle(parent, LOC("MDDRDT_RSDEV"), CMDNAMES_OBF("devflag"), LOC("MDDRDT_RSDEV_H"))
	{
	}

	void CommandDevFlag::onChange(Click& click)
	{
		DlcMgr::dev_flag = m_on;
		DlcMgr::update();

		pointers::ros_credentials->setPrivilege(rage::rlRosPrivilegeId::DEVELOPER, m_on);
	}
}
