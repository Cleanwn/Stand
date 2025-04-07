#pragma once

#include "CommandList.hpp"

#include "Position2d.hpp"

namespace Stand
{
	class CommandPosition2d : public CommandList
	{
	private:
		CommandSlider* x;
		CommandSlider* y;
		CommandToggleNoCorrelation* mouse;

	public:
		explicit CommandPosition2d(CommandList* parent, Label&& menu_name, const CommandName& command_name, const Position2d& default_pos, Label&& help_text = NOLABEL);

		[[nodiscard]] Position2d getPosition() const;
		void setPosition(Position2d&& pos) const;
	private:
		void onChange() const;
	protected:
		virtual void onChange(Position2d&& pos) const = 0;

	public:
		void onBack(ThreadContext thread_context) final;

		std::string getState() const final;
		void setState(Click& click, const std::string& state) final;
		void applyDefaultState() final;

	private:
		void ensureMouseMoveIsDisabled(ThreadContext thread_context);
	};
}
