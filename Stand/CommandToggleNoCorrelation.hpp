#pragma once

#include "CommandPhysical.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandToggleNoCorrelation : public CommandPhysical
	{
	public:
		const bool default_on;
		bool m_on;

		explicit CommandToggleNoCorrelation(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, const bool default_on = false, const commandflags_t flags = CMDFLAGS_TOGGLE, const CommandPerm perm = COMMANDPERM_USERONLY, const std::vector<Hotkey>& default_hotkeys = {}, CommandType type = COMMAND_TOGGLE_NO_CORRELATION);

		[[nodiscard]] std::wstring getCommandSyntax() const final;

		void onClick(Click& click) override;
	protected:
		void onClickSoundEffect(Click& click) const noexcept;
		void onClickImplUnavailable(Click& click);

	public:
		virtual void onChange(Click& click);
		virtual void onEnable(Click& click);
		virtual void onDisable(Click& click);
		void onChangeImplUnavailable(Click& click);

		void onCommand(Click& click, std::wstring& args) override;

		std::string getState() const override;
		std::string getDefaultState() const override;
		void setState(Click& click, const std::string& _state) override;
		void applyDefaultState() override;
	protected:
		void applyDefaultStateImpl(Click& click, bool should_be_on);

	public:
		void setStateBool(Click& click, bool toggle);
		void toggleState(Click& click);
		void updateState(Click& click);
		void setOnIndication(bool on);

	protected:
		void updateWebState();

		static void toggleScriptTickEventHandler(const bool on, Click& click, void handler());

		void onClickToggleScriptTickEventHandler(Click& click, void handler()) const;

		void onChangeToggleScriptTickEventHandler(bool execute_now_if_on, void handler()) const;
		void onChangeToggleScriptTickEventHandler(Click& click, void handler()) const;

		void onChangeTogglePostTickEventHandler(void handler()) const;
	};
#pragma pack(pop)
}
