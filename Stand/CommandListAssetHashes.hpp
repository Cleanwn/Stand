#pragma once

namespace Stand
{
	class CommandListAssetHashes : public CommandList
	{
	public:
		inline static CommandListAssetHashes* instance;

		CommandToggleNoCorrelation* fix;

		explicit CommandListAssetHashes(CommandList* const parent);

		void processCrcUpdate();
	};
}
