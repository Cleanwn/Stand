#pragma once

#include "CommandListAction.hpp"

#include "CommandListActionItemData.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandListSelect : public CommandListAction
	{
	protected:
		long long default_value;

	public:
		long long value;

		explicit CommandListSelect(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, std::vector<CommandListActionItemData>&& options, long long default_value, const commandflags_t flags = CMDFLAGS_LIST_SELECT, const commandflags_t item_flags = 0);

		[[nodiscard]] std::wstring getCommandSyntax() const final;

		void onClick(Click& click) override;
		bool onLeft(Click& click, bool holding) final;
		bool onRight(Click& click, bool holding) final;

		[[nodiscard]] CommandListActionItem* getCurrentValueCommand() const;

		[[nodiscard]] Label getCurrentValueMenuName() const;
		[[nodiscard]] Label getDefaultValueMenuName() const;

		[[nodiscard]] Label getCurrentValueHelpText() const;

		void setValue(Click& click, long long value);
		void setValueIndication(long long value);

		std::string getState() const override;
		std::string getDefaultState() const override;
		void setState(Click& click, const std::string& state) override;
		void applyDefaultState() override;

		void onCommand(Click& click, std::wstring& args) override final;

		void onItemClick(Click& click, CommandListActionItem* item) final;

		virtual void onChange(Click& click, long long prev_value);
	protected:
		void onChangeImplWarning(Click& click, long long prev_value, std::wstring&& warning, warningflags_t flags);
		void onChangeImplUnavailable(Click& click, long long prev_value);
		void onChangeProcessScriptTickEventHandler(Click& click, long long prev_value, long long for_value, void handler());

	public:
		void setGenericResponse(Click& click) const;

		void updateState(const Click& click) const;

		[[nodiscard]] std::string getWebState() const;
		void sendDataToRelay() const;
	private:
		void updateWebState() const;
	};
#pragma pack(pop)
}
