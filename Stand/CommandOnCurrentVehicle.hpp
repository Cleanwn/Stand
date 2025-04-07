#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"

namespace Stand
{
	template <class Base>
	class CommandOnCurrentVehicleBase : public Base
	{
	protected:
		AbstractEntity last_vehicle = AbstractEntity::invalid();

	public:
		using Base::Base;

	protected:
		virtual void doChangeImpl(AbstractEntity& vehicle) = 0;
		virtual void undoChangeImpl(AbstractEntity& vehicle) = 0;

		[[nodiscard]] virtual bool requiresChange(AbstractEntity& vehicle)
		{
			return false;
		}

		void undoChange(AbstractEntity& vehicle)
		{
#if false
			Util::toast("undoChange has been called, resetting last_vehicle.");
#endif
			undoChangeImpl(vehicle);
			last_vehicle.reset();
		}

		void doChange(AbstractEntity& vehicle)
		{
			SOUP_ASSERT(!last_vehicle.isValid() || last_vehicle == vehicle);

			if (requiresChange(vehicle))
			{
#if false
				Util::toast("Doing action and setting last_vehicle to the current vehicle.");
#endif
				doChangeImpl(vehicle);
				last_vehicle = vehicle;
			}
		}
	};

	class CommandOnCurrentVehicleToggle : public CommandOnCurrentVehicleBase<CommandToggle>
	{
	public:
		explicit CommandOnCurrentVehicleToggle(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& cmds = {}, Label&& help_text = NOLABEL)
			: CommandOnCurrentVehicleBase<CommandToggle>(parent, std::move(menu_name), std::move(cmds), std::move(help_text))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler([this]
			{
				HANDLER_CHECK(m_on);

				if (last_vehicle.isValid() && g_player_veh != last_vehicle)
				{
					undoChange(last_vehicle);
				}
				else if (g_player_veh.isValid() && g_player_veh.isOwnerOfVehicleAndDriver())
				{
					doChange(g_player_veh);
				}

				HANDLER_END;
			});
		}

		void onDisable(Click& click) final
		{
			if (last_vehicle.isValid())
			{
				undoChange(last_vehicle);
			}
		}
	};
}