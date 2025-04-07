#pragma once

#include "CommandInput.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandHistoricPlayerNote : public CommandInput
	{
	public:
		explicit CommandHistoricPlayerNote(CommandList* parent, const CommandName& simple_name);

		void onCommand(Click& click, std::wstring& args) final;

		void onTickInGameViewport() final;
		void onTickInWebViewport() final;
	};
#pragma pack(pop)
}
