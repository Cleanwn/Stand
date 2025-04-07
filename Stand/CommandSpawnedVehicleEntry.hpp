#pragma once

#include "CommandList.hpp"

#include "AbstractEntity.hpp"
#include "CommandWithOnTickFocused.hpp"
#include "CommandListWithOnTickAsActiveList.hpp"

namespace Stand
{
	class CommandSpawnedVehicleEntry : public CommandWithOnTickFocused<CommandListWithOnTickAsActiveList<CommandList>>
	{
	private:
		AbstractEntity entity;

	public:
		explicit CommandSpawnedVehicleEntry(CommandList* const parent, Label&& menu_name, AbstractEntity entity);

		void onTickFocused() final;
		void onTickAsActiveList() final;

		void onTickInWebViewport() final;
		void onTickInGameViewport() final;

	private:
		void drawBox();
	};
}