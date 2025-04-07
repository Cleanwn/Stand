#pragma once

#include "CommandWithOnTickFocused.hpp"
#include "CommandInput.hpp"

#include "vector.hpp"

namespace Stand
{
	class CommandPositionSelection : public CommandWithOnTickFocused<CommandInput>
	{
	public:
		bool is_being_changed = false;
		v3 pos{ 467.1256f, 5543.1304f, 785.0175f };

		explicit CommandPositionSelection(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names);

		void onClick(Click& click) final;
		void onCommand(Click& click, std::wstring& args) final;

		void onTickFocused() final;

	private:
		void setPosFromValue();

	protected:
		virtual void onChange();
	};
}
