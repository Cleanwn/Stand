#pragma once

#include "CommandAction.hpp"

#include "gta_fwddecl.hpp"

namespace Stand
{
	class CommandInteriorCustomisable : public CommandAction
	{
	protected:
		explicit CommandInteriorCustomisable(CommandList* const parent, std::vector<CommandName>&& command_names);
		
	public:
		void onClick(Click& click) final;
		void onOptionChanged() const;
		void loadFreshRandomised() const;

	protected:
		[[nodiscard]] int getInteriorId() const;

		virtual v3 getPosition() const = 0;
		virtual void toggleEntitySets() const = 0;
		virtual void randomiseEntitySets() const = 0;

		static void toggleEntitySet(const int interior_id, const char* const name, const bool toggle);
		void toggleEntitySet(const char* const name, const bool toggle) const;
	};
}
