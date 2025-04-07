#pragma once

#include "CommandAction.hpp"

#include "natives.hpp"
#include "TpUtil.hpp"

namespace Stand
{
	class CommandInterior : public CommandAction
	{
	private:
		const v3 pos;
		const bool needs_mp_world_state;

	public:
		explicit CommandInterior(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, v3&& pos, const bool needs_mp_world_state = false);
		explicit CommandInterior(CommandList* const parent, Label&& menu_name, v3&& pos, const bool needs_mp_world_state = false);

		[[nodiscard]] int getInteriorId() const;
		[[nodiscard]] static int getInteriorId(const v3& pos);

		static void disable(const int interior_id);
		void disable() const;

		void enable() const;
		static void enable(const v3& pos, const bool needs_mp_world_state = false);
		static void enable(const int interior_id);

		void teleport() const;
		static void teleport(const v3& pos, const bool needs_mp_world_state);

		void onClick(Click& click) final;
	};
}
