#pragma once

#include "struct_base.hpp"
#include "gta_fwddecl.hpp"

#include "netMessage.hpp"

#include "netMessageId.hpp"

#pragma pack(push, 1)
namespace rage
{
	struct msgScript : public netMessage // made up
	{
		scriptIdBase* script_id;
	};
	static_assert(sizeof(msgScript) == 8);

	struct msgScriptMigrateHost : public msgScript
	{
		AUTOID_MSG(MSG_SCRIPT_MIGRATE_HOST);

		/* 0x08 */ uint16_t host_token;
		PAD(0x08 + 2, 0x0C) uint32_t net_array_data_size;

		template <typename T>
		bool ser(T& bb)
		{
			return script_id->ser(bb)
				&& bb.serU16(host_token)
				&& bb.serU32(net_array_data_size)
				;
		}
	};
	static_assert(sizeof(msgScriptMigrateHost) == 0x0C + 4);

	struct msgScriptNewHost : public msgScript
	{
		/* 0x08 */ uint16_t host_token;
	};
	static_assert(sizeof(msgScriptNewHost) == 0x08 + 2);

	struct msgScriptHostRequest : public msgScript
	{
	};
}
#pragma pack(pop)
