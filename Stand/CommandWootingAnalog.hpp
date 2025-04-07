#pragma once

#include "CommandToggle.hpp"

#include <soup/DelayedCtor.hpp>
#include <soup/SharedLibrary.hpp>

#include "pointers.hpp"
#include "Util.hpp"

namespace Stand
{
	// rage_input_helper.asm
	extern "C" void rage_input_helper_set_override(float(*pArray)[256]);

	class CommandWootingAnalog : public CommandToggle
	{
	private:
		using WootingAnalogResult = int;

		using wooting_analog_initialise_t = WootingAnalogResult(*)();
		using wooting_analog_uninitialise_t = WootingAnalogResult(*)();
		using wooting_analog_set_keycode_mode_t = WootingAnalogResult(*)(int mode);
		using wooting_analog_read_full_buffer_t = int(*)(unsigned short* code_buffer, float* analog_buffer, unsigned int len);

		soup::DelayedCtor<soup::SharedLibrary> sdk;
		wooting_analog_read_full_buffer_t read_full_buffer;
		float arr[256];

	public:
		explicit CommandWootingAnalog(CommandList* const parent)
			: CommandToggle(parent, LIT("[Debug Build] Wooting Analog Support"), { CMDNAME("wooting") }, PH("If you have a Wooting keyboard, this makes the game understand how strongly you are pressing your keys. The Wooting Analog SDK has to be installed; after the installation, the game and its launchers need to be restarted."))
		{
			this->flags |= CMDFLAG_FEATURELIST_SKIP; // because only available in debug build right now
			memset(arr, 0, sizeof(arr));
		}

		void onEnable(Click& click) final
		{
			// Prevent Wooting Analog SDK writing to our console.
			SetEnvironmentVariableA("RUST_LOG", "off");

			sdk.construct("wooting_analog_sdk");
			if (sdk->isLoaded()
				&& ((wooting_analog_initialise_t)sdk->getAddress("wooting_analog_initialise"))() >= 0
				)
			{
				// Set to VirtualKey mode
				((wooting_analog_set_keycode_mode_t)sdk->getAddress("wooting_analog_set_keycode_mode"))(2);

				read_full_buffer = (wooting_analog_read_full_buffer_t)sdk->getAddress("wooting_analog_read_full_buffer");

				rage_input_helper_set_override(&arr);

				registerPresentEventHandler([this]
				{
					HANDLER_CHECK(m_on);
					if (GetForegroundWindow() == pointers::hwnd)
					{
						unsigned short code_buffer[16];
						float analog_buffer[16];
						const int num = read_full_buffer(code_buffer, analog_buffer, 16);
						if (num >= 0)
						{
							memset(arr, 0, sizeof(arr));
							for (int i = 0; i != num; ++i)
							{
								if (code_buffer[i] < COUNT(arr))
								{
									arr[code_buffer[i]] = analog_buffer[i];
								}
							}
						}
					}
					else
					{
						memset(arr, 0, sizeof(arr));
					}
					HANDLER_END;
				});
			}
			else
			{
				Util::toast(PH("Failed to initialise Wooting Analog SDK. Ensure you have it installed and restarted the game and its launchers after doing so."));
			}
		}

		void onDisable(Click& click) final
		{
			rage_input_helper_set_override(nullptr);

			if (sdk->isLoaded())
			{
				((wooting_analog_uninitialise_t)sdk->getAddress("wooting_analog_uninitialise"))();
			}
			sdk.destroy();
		}
	};
}
