#pragma once

#include "CommandToggle.hpp"

#include <soup/AnalogueKeyboard.hpp>
#include <soup/os.hpp>

#include "Exceptional.hpp"
#include "pointers.hpp"

namespace Stand
{
	// rage_input_helper.asm
	extern "C" void rage_input_helper_set_override(float(*pArray)[256]);

	class CommandAnalogueInput : public CommandToggle
	{
	public:
		struct ThreadData
		{
			bool running = true;
			soup::AnalogueKeyboard* pKbd = nullptr;
		};

		ThreadData* current_thread_data;

		inline static float pressure_subtrahend = 0.0f;
		inline static float pressure_divisior = 1.0f;

		explicit CommandAnalogueInput(CommandList* const parent)
			: CommandToggle(parent, LOC("ANAL"), { CMDNAME("analogue"), CMDNAME("wooting") }, LOC("ANAL_H"))
		{
		}

		void onEnable(Click& click) final
		{
			current_thread_data = new ThreadData();
			Exceptional::createManagedThread([thread_data{ current_thread_data }]
			{
				__try
				{
					float arr[256];
					while (thread_data->running)
					{
						for (auto& kbd : soup::AnalogueKeyboard::getAll())
						{
							thread_data->pKbd = &kbd;
							rage_input_helper_set_override(&arr);
							while (thread_data->running && !kbd.disconnected)
							{
								float new_arr[256];
								memset(new_arr, 0, sizeof(new_arr));
								if (!kbd.isPoll() || GetForegroundWindow() == pointers::hwnd)
								{
									for (const auto& key : kbd.getActiveKeys())
									{
										const auto vk = key.getVk();
										if (vk >= 0 && vk < 256)
										{
											new_arr[vk] = std::clamp((key.fvalue - pressure_subtrahend) / pressure_divisior, 0.0f, 1.0f);
										}
									}
								}
								if (GetForegroundWindow() == pointers::hwnd)
								{
									memcpy(arr, new_arr, sizeof(new_arr));
								}
								else
								{
									memset(arr, 0, sizeof(arr));
								}
							}
							thread_data->pKbd = nullptr;
						}
						rage_input_helper_set_override(nullptr);
						if (!thread_data->running)
						{
							break;
						}
						soup::os::sleep(100);
					}
				}
				__EXCEPTIONAL()
				{
				}
				delete thread_data;
			});
		}

		void onDisable(Click& click) final
		{
			current_thread_data->running = false;
			if (current_thread_data->pKbd)
			{
				current_thread_data->pKbd->hid.cancelReceiveReport();
			}
		}
	};
}
