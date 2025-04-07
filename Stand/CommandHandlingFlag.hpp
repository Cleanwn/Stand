#pragma once

#include "CommandOnCurrentVehicle.hpp"

#include "Offset.hpp"
#include "CHandlingData.hpp"
#include "gta_vehicle.hpp"

namespace Stand
{
	template <class HandlingType>
	class CommandHandlingFlag : public CommandOnCurrentVehicleToggle
	{
	private:
		static constexpr uint32_t NOT_A_FLAG = 3;

		using OffsetType = Offset<HandlingType, uint32_t>;

		uint32_t flag;
		OffsetType offset;

		uint32_t og_flags = NOT_A_FLAG;
		bool use_og_flags;

	protected:
		virtual void doChangeExtra(AbstractEntity& vehicle)
		{
		}

		virtual void undoChangeExtra(AbstractEntity& vehicle)
		{
		}

	public:
		explicit CommandHandlingFlag(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& cmds, Label&& help_text, OffsetType offset, uint32_t flag, bool use_og_flags = false)
			: CommandOnCurrentVehicleToggle(parent, std::move(menu_name), std::move(cmds), std::move(help_text)), offset(offset), flag(flag), use_og_flags(use_og_flags)
		{
		}

		void doChangeImpl(AbstractEntity& vehicle) final
		{
			SOUP_IF_LIKELY (auto handling = getHandlingData(vehicle))
			{
				if (use_og_flags && og_flags == NOT_A_FLAG)
				{
					og_flags = offset.resolve(*handling);
				}

				offset.resolve(*handling) |= flag;
			}

			doChangeExtra(vehicle);
		}

		void undoChangeImpl(AbstractEntity& vehicle) final
		{
			SOUP_IF_LIKELY (auto handling = getHandlingData(vehicle))
			{
				if (use_og_flags)
				{
					SOUP_ASSERT(og_flags != NOT_A_FLAG);
					offset.resolve(*handling) = og_flags;
					og_flags = NOT_A_FLAG;
				}
				else
				{
					offset.resolve(*handling) &= ~flag;
				}
			}

			undoChangeExtra(vehicle);
		}

		bool requiresChange(AbstractEntity& vehicle) override
		{
			SOUP_IF_LIKELY (auto handling = getHandlingData(vehicle))
			{
				return !(offset.resolve(*handling) & flag);
			}

			return CommandOnCurrentVehicleToggle::requiresChange(vehicle);
		}

		[[nodiscard]] HandlingType* getHandlingData(AbstractEntity& vehicle)
		{
			SOUP_IF_LIKELY (auto cveh = vehicle.getCVehicle())
			{
				SOUP_IF_LIKELY (auto handling = cveh->handling_data)
				{
					if constexpr (std::is_same_v<HandlingType, CCarHandlingData>)
					{
						if (auto subhandling = handling->getSubhandlingData(HANDLING_TYPE_CAR))
						{
							return (CCarHandlingData*)subhandling;
						}
					}
					else
					{
						return handling;
					}
				}
			}

			return nullptr;
		}
	};
}