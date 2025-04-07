#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListUserkv : public CommandList
	{
	private:
		const std::string default_state;
	protected:
		std::vector<std::pair<std::string, CommandPhysical*>> entries = {};
	private:
		size_t body_offset;

	protected:
		explicit CommandListUserkv(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, std::string&& default_state = {});

	public:
		[[nodiscard]] std::string getState() const final;
		[[nodiscard]] std::string getDefaultState() const final;
		void setState(Click& click, const std::string& state) final;
		void applyDefaultState() final;

	public:
		void addItem(ThreadContext thread_context, std::string&& key, std::string&& value);
	private:
		CommandPhysical* addItem(std::string&& key, std::string&& value);
	public:
		void removeItem(const std::string& key);

	protected:
		virtual void populateHead() = 0;
		[[nodiscard]] virtual std::vector<std::pair<std::string, CommandPhysical*>>::iterator find(const std::string& key);
		[[nodiscard]] virtual CommandPhysical* emplaceItem(const std::string& key, std::string&& value) = 0;
		virtual void focusItem(ThreadContext thread_context, CommandPhysical* item) const = 0;
		[[nodiscard]] virtual std::string getValue(const std::string& key, CommandPhysical* item) const = 0;
		virtual void onRemoveItem(CommandPhysical* item) = 0;
	};
}
