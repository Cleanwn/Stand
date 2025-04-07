#pragma once

#include "CommandListUserkv.hpp"

namespace Stand
{
	class CommandModelBlacklist : public CommandListUserkv
	{
	public:
		explicit CommandModelBlacklist(CommandList* parent);

	protected:
		void populateHead() final;
		CommandPhysical* emplaceItem(const std::string& key, std::string&& value) final;
		void focusItem(ThreadContext thread_context, CommandPhysical* item) const final;
		std::string getValue(const std::string& key, CommandPhysical* item) const final;
		void onRemoveItem(CommandPhysical* item) final;
	};
}
