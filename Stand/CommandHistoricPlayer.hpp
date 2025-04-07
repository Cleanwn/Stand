#pragma once

#include "CommandWithOnTickFocused.hpp"
#include "CommandList.hpp"

#include <soup/PointerAndBool.hpp>

#include "HistoricPlayer.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandHistoricPlayer : public CommandWithOnTickFocused<CommandList>
	{
	public:
		soup::PointerAndBool<HistoricPlayer*> player;

		explicit CommandHistoricPlayer(HistoricPlayer* const player);
		void populateCommandsWithoutCommandNames();
		void onPreScriptedAccess() final;

		void onActiveListUpdate() final;

		void onClick(Click& click) final;

		RemoteGamer* update();

		void onTickInGameViewport() final;
		void onTickFocused() final;
	private:
		[[nodiscard]] Label getHelpText();

	public:
		std::string getNameForConfig() const final;
		
		std::string getState() const final;

		static __declspec(noinline) std::string getStateImpl(const CommandHistoricPlayer* _this);
	};
#pragma pack(pop)
}
