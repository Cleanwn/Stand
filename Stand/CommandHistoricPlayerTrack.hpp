#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandHistoricPlayerTrack : public CommandListSelect
	{
	public:
		explicit CommandHistoricPlayerTrack(CommandList* parent);

		void onChange(Click& click, long long prev_value) final;

		void onTickInGameViewport() final;
		void onTickInWebViewport() final;
	};
#pragma pack(pop)
}
