#pragma once

#include "CommandList.hpp"

namespace Stand
{
	using search_flags_t = uint8_t;
#define SEARCH_SIMPLIFIED			0b1
#define SEARCH_LOWER				0b10
//#define SEARCH_ARG_IS_SINGLE_WORD	0b100

	class CommandSearch : public CommandList
	{
	private:
		const Label og_menu_name;
		const search_flags_t search_flags;
		bool active = false;

	public:
		explicit CommandSearch(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, const search_flags_t search_flags = 0, const commandflags_t flags = CMDFLAGS_SEARCH);

		[[nodiscard]] Label getActivationName() const final;

		std::wstring getCommandSyntax() const final;

		void onClick(Click& click) override;

		void onCommand(Click& click, std::wstring& args) override;
	protected:
		virtual void doSearch(std::string&& arg) = 0;
		void checkForMatchByMenuName(const std::string& arg, CommandPhysical* cmd);
		void resetMenuName();

	public:
		void onActiveListUpdate() final;
	};
}
