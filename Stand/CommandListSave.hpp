#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListSave : public CommandList
	{
	private:
		std::vector<CommandName> action_command_names;
	public:
		explicit CommandListSave(CommandList* parent, Label&& menu_name, std::vector<CommandName>&& action_command_names = {});

		void populate();

	public:
		[[nodiscard]] virtual std::wstring getFolder() const = 0;

		[[nodiscard]] virtual std::unordered_map<std::string, std::string> getDataEn() const = 0;
	protected:
		[[nodiscard]] virtual bool canEntryBeOmitted(hash_t key) const;
	public:
		[[nodiscard]] virtual std::vector<hash_t> getDataOrderRaw() const = 0;

		[[nodiscard]] bool shouldOmitEntry(hash_t key) const;
		
		virtual void onSaved() const;
	};
}
