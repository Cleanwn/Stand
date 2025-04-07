#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListConcealer : public CommandList
	{
	private:
		std::vector<Command*> order{};
		size_t invisible_children = 0;

	public:
		explicit CommandListConcealer(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, commandflags_t flags = CMDFLAGS_LIST_CONCEALER, CommandType type = COMMAND_LIST_CONCEALER);

		void onClick(Click& click) override;

		void resetChildrenWithPreDelete() noexcept;

		[[nodiscard]] static bool isVisible(Command* const cmd);
		void setVisible(Command* const cmd, const bool toggle);
		bool setVisibleBulk(Command* const cmd, const bool toggle);
		void setVisibleRaw(Command* const cmd) noexcept;

		void processChildrenVisibilityUpdate();

		void emplaceVisible(std::unique_ptr<Command>&& cmd);

	private:
		[[nodiscard]] bool wasOrderCaptured() const noexcept;
		void captureOrder();
		[[nodiscard]] size_t getOrderIndex(Command* const cmd) const noexcept;

	public:
		void moveChildrenAllVisible();
		void moveChildrenConceal();

		[[nodiscard]] size_t countInvisibleChildren() const noexcept;
		[[nodiscard]] size_t countVisibleChildren() const noexcept;
	};
}
