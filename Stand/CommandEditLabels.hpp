#pragma once

#include "CommandListUserkv.hpp"

namespace Stand
{
	class CommandEditLabels : public CommandListUserkv
	{
	public:
		explicit CommandEditLabels(CommandList* const parent);

		[[nodiscard]] static uint32_t keyToHash(const std::string& key);

	protected:
		void populateHead() final;
		[[nodiscard]] std::vector<std::pair<std::string, CommandPhysical*>>::iterator find(const std::string& key) final;
		[[nodiscard]] CommandPhysical* emplaceItem(const std::string& key, std::string&& value) final;
		void focusItem(ThreadContext thread_context, CommandPhysical* item) const final;
		[[nodiscard]] std::string getValue(const std::string& key, CommandPhysical* item) const final;
		void onRemoveItem(CommandPhysical* item) final;
	};
}
