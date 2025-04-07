#pragma once

#include "CommandPhysical.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandSlider : public CommandPhysical
	{
	public:
		int min_value;
		int max_value;
		int value;
		int default_value;
		unsigned int step_size;
		bool is_click_to_apply;

		inline static bool prefill_value = true;
		inline static bool selectable_na = false;

		explicit CommandSlider(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const int min_value, const int max_value, const int default_value, const unsigned int step_size = 1, const commandflags_t flags = CMDFLAGS_SLIDER, const CommandPerm perm = COMMANDPERM_USERONLY, bool is_click_to_apply = false, const std::vector<Hotkey>& default_hotkeys = {}, CommandType type = COMMAND_SLIDER);

		[[nodiscard]] bool isNotApplicable() const noexcept;
		[[nodiscard]] bool isSelectable() const noexcept;
		void onFocus(ThreadContext thread_context, Direction momentum) override;

		[[nodiscard]] float getFloatValue() const;
		template <typename T> [[nodiscard]] T getTValue() const;
		[[nodiscard]] virtual std::wstring formatNumber(int num, bool allow_replacements = true) const;
		[[nodiscard]] bool currentNumberHasReplacement() const;

		[[nodiscard]] std::wstring getCommandSyntax() const final;
		[[nodiscard]] std::wstring getRangeString() const;

		void onClick(Click& click) override;

		void onCommand(Click& click, std::wstring& args) override;
	protected:
		bool onCommandImplKeyword(const std::wstring& arg);
		bool onCommandImplValue(Click& click, std::wstring&& arg);
		void onCommandImplDispatch(Click& click, int prev_value);

	public:
		void openCommandBoxWithPrefill(Click& click);

		bool onLeft(Click& click, bool holding) final;
		bool onRight(Click& click, bool holding) final;

	private:
		void updateValue(Click& click, int value);

	public:
		void setValue(Click& click, int value);

		[[nodiscard]] std::string getState() const override;
		[[nodiscard]] std::string getDefaultState() const override;
		void setState(Click& click, const std::string& state) override;
		void applyDefaultState() override;

		virtual void onChange(Click& click, int prev_value);
		void onChangeImplUnavailable(Click& click, int prev_value);

		void setValueIndicator(const Click& click, int value);
		
		void updateState(const Click& click);

		void setMinValue(int min_value);
		void setMaxValue(int max_value);
		void setStepSize(int step_size);
	private:
		void updateWebMaxValue() const;
	};
#pragma pack(pop)

	template<>
	inline int CommandSlider::getTValue() const
	{
		return value;
	}

	template<>
	inline float CommandSlider::getTValue() const
	{
		return getFloatValue();
	}
}
