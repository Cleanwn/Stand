#include "CommandSessionType.hpp"

#include <soup/ObfusString.hpp>

#include "CNetworkSession.hpp"
#include "CommandListActionItem.hpp"
#include "gsType.hpp"
#include "NetworkSessionUtils.hpp"
#include "pointers.hpp"
#include "sc_presence_attributes.hpp"
#include "SessionSpoofing.hpp"
#include "SessionType.hpp"

namespace Stand
{
	CommandSessionType::CommandSessionType(CommandList* const parent)
		: CommandListAction(parent, LOC("SETSESSTYP"), CMDNAMES("setsessiontype", "setsesstype", "sesstype", "sessiontype"), NOLABEL, {
			{SOLO, LOC("SESSTYP_S"), CMDNAMES("solo")},
			{INVITE_ONLY, LOC("SESSTYP_I"), CMDNAMES("inviteonly", "invonly")},
			{CLOSED_FRIEND, LOC("SESSTYP_F"), CMDNAMES("closedfriend")},
			{CLOSED_CREW, LOC("SESSTYP_CC"), CMDNAMES("closedcrew")},
			{CREW, LOC("SESSTYP_C"), CMDNAMES("crew")},
			{NEW_PUBLIC, LOC("SESSTYP_P"), CMDNAMES("public")},
		})
	{
	}

	static void updateGsType(int64_t gstype)
	{
		if (!SessionSpoofing::isActive())
		{
			auto attr = rage::rlScPresenceAttribute::findByName(soup::ObfusString("gstype").c_str());
			if (attr != nullptr)
			{
				attr->setInt(gstype);
			}
		}
	}
	
	static void updateSessionType(int64_t gstype, uint32_t host_flags)
	{
		updateGsType(gstype);
		(*pointers::network_session)->m_nHostFlags[ST_Physical] = host_flags;
	}

	void CommandSessionType::onItemClick(Click& click, CommandListActionItem* item)
	{
		if (!click.isHost())
		{
			return;
		}

		(*pointers::network_session)->setSlots((SessionType)item->value);

		switch (item->value)
		{
		case SOLO:
			updateSessionType(GS_SOLO, HF_SingleplayerOnly | HF_IsPrivate);
			break;

		case INVITE_ONLY:
			updateSessionType(GS_INVITE, HF_IsPrivate);
			break;

		case CLOSED_FRIEND:
			updateSessionType(GS_CLOSED_FRIEND, HF_ClosedFriends | HF_Closed | HF_NoMatchmaking);
			break;

		case CLOSED_CREW:
			updateSessionType(GS_CLOSED_CREW, HF_ClosedCrew | HF_Closed | HF_NoMatchmaking);
			break;

		case CREW:
			updateSessionType(GS_CREW, 0);
			break;

		case NEW_PUBLIC:
			updateSessionType(GS_PUBLIC, 0);
			break;
		}
	}
}
