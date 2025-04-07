#include "CommandListAnalogue.hpp"

#include <soup/ObfusString.hpp>

#include "CommandDivider.hpp"
#include "CommandLambdaSlider.hpp"
#include "CommandReadonlyLink.hpp"
#include "CommandReadonlyValueLambda.hpp"
#include "CommandSlider.hpp"

#include "CommandAnalogueInput.hpp"

namespace Stand
{
	// rage_input_helper.asm
	extern "C" float(*rage_input_helper_get_override())[256];

	CommandListAnalogue::CommandListAnalogue(CommandList* const parent)
		: CommandList(parent, LOC("ANAL"), {}, LOC("ANAL_H"))
	{
		auto cmd = this->createChild<CommandAnalogueInput>();
		this->createChild<CommandReadonlyLink>(LOC("ANAL_L"), soup::ObfusString("https://analogsense.org/devices"));
		this->createChild<CommandLambdaSlider<CommandSlider>>(LOC("PRSRSUB"), CMDNAMES("analoguesubtrahend", "analogsubtrahend"), NOLABEL, 0, 255, 0, 25, [](int value, Click&)
		{
			CommandAnalogueInput::pressure_subtrahend = static_cast<float>(value) / 255.0f;
		});
		this->createChild<CommandLambdaSlider<CommandSlider>>(LOC("PRSRDIV"), CMDNAMES("analoguedivisor", "analogdivisor"), NOLABEL, 1, 255, 255, 25, [](int value, Click&)
		{
			CommandAnalogueInput::pressure_divisior = static_cast<float>(value) / 255.0f;
		});
		this->createChild<CommandDivider>(LOC("STUS"));
		this->createChild<CommandReadonlyValueLambda>(LOC("DEV"), [cmd]() -> std::wstring
		{
			return cmd->m_on && cmd->current_thread_data->pKbd
				? StringUtils::utf8_to_utf16(cmd->current_thread_data->pKbd->name)
				: LANG_GET_W("NA")
				;
		});
		this->createChild<CommandReadonlyValueLambda>(LOC("PRSR"), []() -> std::wstring
		{
			if (float(*arr)[256] = rage_input_helper_get_override())
			{
				float most_pressure = 0.0f;
				for (const auto& f : *arr)
				{
					if (most_pressure < f)
					{
						most_pressure = f;
					}
				}
				return std::to_wstring(static_cast<uint8_t>(most_pressure * 255.0f));
			}
			return LANG_GET_W("NA");
		});
	}
}
