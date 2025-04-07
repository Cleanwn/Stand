#include "CommandListAssetHashes.hpp"

#include "CNetworkAssetVerifier.hpp"
#include "CommandDivider.hpp"
#include "CommandLambdaAction.hpp"
#include "CommandTogglePointer.hpp"
#include "Hooking.hpp"
#include "pointers.hpp"

#include "CommandFixAssetHashes.hpp"
#include "CommandSetAssetChecksum.hpp"
#include "CommandSpoofedExtraCrc.hpp"
#include "CommandExtraCrcPresets.hpp"

namespace Stand
{
	CommandListAssetHashes::CommandListAssetHashes(CommandList* const parent)
		: CommandList(parent, LOC("AST"))
	{
		instance = this;

		fix = this->createChild<CommandFixAssetHashes>();
		this->createChild<CommandDivider>(LOC("ASTCRC"));
		this->createChild<CommandSetAssetChecksum>();
		this->createChild<CommandLambdaAction>(LOC("ASTRST"), CMDNAMES_OBF("resetassetchecksum"), NOLABEL, [](Click& click)
		{
			(*pointers::asset_verifier)->refreshCrc();
		});
		this->createChild<CommandDivider>(LOC("XTRCRC"));
		this->createChild<CommandTogglePointer>(&g_hooking.spoof_extra_content_crc, LOC("OVRXTR"), CMDNAMES_OBF("extratoggle"));
		auto extra_crc = this->createChild<CommandSpoofedExtraCrc>();
		this->createChild<CommandExtraCrcPresets>(extra_crc);
	}

	void CommandListAssetHashes::processCrcUpdate()
	{
		if (fix->m_on)
		{
 			fix->as<CommandFixAssetHashes>()->setLegitValues();
		}
	}
}
