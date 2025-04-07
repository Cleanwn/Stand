#pragma once

#include "CommandSliderFloat.hpp"

#include <unordered_map>

#include "AbstractEntity.hpp"
#include "AbstractModel.hpp"
#include "atoffset.hpp"
#include "CommandVehicleHandling.hpp"
#include "CHandlingData.hpp"
#include "gta_vehicle.hpp"

namespace Stand
{
	template <typename T, typename Base>
	class CommandVehicleHandling : public Base
	{
	private:
		uintptr_t value_offset = 0;
		std::unordered_map<hash_t, T> default_values{};

		[[nodiscard]] T* getAddr() const noexcept
		{
			if (auto v = g_player_veh.getCVehicle())
			{
				const auto data = v->handling_data;
				if (this->parent->as<CommandListSubhandling>()->handling_type == HANDLING_TYPE_INVALID)
				{
					return atoffset<T>(data, value_offset);
				}
				if (auto subhandling = data->getSubhandlingData(this->parent->as<CommandListSubhandling>()->handling_type))
				{
					return atoffset<T>(subhandling, value_offset);
				}
			}
			return nullptr;
		}

		void setVisualValueOnly(T value)
		{
			Click click(CLICK_BULK, TC_SCRIPT_NOYIELD);
			if constexpr (std::is_same_v<T, float>)
			{
				this->setValueIndicator(click, int(value * Base::getPrecisionScalar()));
			}
			else
			{
				this->setValueIndicator(click, value);
			}
		}

	public:
		explicit CommandVehicleHandling(CommandList* const parent, uintptr_t offset, Label&& menu_name)
			: Base(parent, std::move(menu_name), { CMDNAME("vh") }, NOLABEL, -100000000, 100000000, 0, 5, CMDFLAGS_SLIDER | CMDFLAG_TEMPORARY | CMDFLAG_FEATURELIST_FINISHLIST), value_offset(offset)
		{
			Base::addSuffixToCommandNames(this->menu_name.getEnglishForCommandName());
			if constexpr (std::is_same_v<Base, CommandSliderFloat>)
			{
				Base::step_size = 500;
				Base::precision = 4;
			}
		}

		void onChange(Click& click, int prev_value) final
		{
			if (auto addr = getAddr())
			{
				if (const hash_t this_hash = g_player_veh.getModel(); !default_values.contains(this_hash))
				{
					this->default_values.emplace(this_hash, *addr);
				}

				*addr = Base::template getTValue<T>();
			}
		}

		void applyDefaultState() final
		{
			if (g_player_veh.isValid())
			{
				if (const auto entry = this->default_values.find(g_player_veh.getModel()); entry != this->default_values.end())
				{
					this->setVisualValueOnly(entry->second);

					auto click = Click(CLICK_BULK, TC_APPLYDEFAULTSTATE);
					this->onChange(click, 0);
				}
			}
		}

		void onTickInGameViewport() final
		{
			if (auto addr = getAddr())
			{
				this->setVisualValueOnly(*addr);
				this->setMinValue(-100000000);
				this->setMaxValue(100000000);
				return;
			}
			//this->setVisualValueOnly(0);
			this->setMinValue(0);
			this->setMaxValue(0);
		}

		void onTickInWebViewport() final
		{
			Base::onTickInWebViewportImplRedirect();
		}

		void onPreScriptedAccess() final
		{
			if (!Base::isInViewport())
			{
				onTickInGameViewport();
			}
		}
	};

	using CommandVehicleHandlingFloat = CommandVehicleHandling<float, CommandSliderFloat>;
	using CommandVehicleHandlingInt = CommandVehicleHandling<int, CommandSlider>;
}
