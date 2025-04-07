#pragma once

#include "CommandWithOnTickFocused.hpp"
#include "CommandList.hpp"

#include "natives_decl.hpp"

namespace Stand
{
	class CommandSpawnedObject : public CommandWithOnTickFocused<CommandList>
	{
	public:
		const Object handle;

		explicit CommandSpawnedObject(CommandList* parent, const Label& menu_name, Object handle);

		~CommandSpawnedObject() final;

		[[nodiscard]] AbstractEntity getEntity() const;

		void onTickFocused() final;
	};
}
