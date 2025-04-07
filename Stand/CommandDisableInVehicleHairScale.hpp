#pragma once

#include "CommandToggle.hpp"

#include <soup/Bytepatch.hpp>

namespace Stand
{
	class CommandDisableInVehicleHairScale : public CommandToggle
	{
	private:
		soup::Bytepatch m_patch;

	public:
		explicit CommandDisableInVehicleHairScale(CommandList* const parent);

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;
	};
}
