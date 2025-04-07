#include "CommandListNotifySettings.hpp"

#include "CommandLambdaAction.hpp"
#include "CommandLambdaSlider.hpp"
#include "CommandSlider.hpp"
#include "CommandSliderPointer.hpp"
#include "create_rainbow.hpp"
#include "Gui.hpp"
#include "lang.hpp"
#include "Renderer.hpp"
#include "Toaster.hpp"
#include "Util.hpp"

#include "CommandNotifyType.hpp"
#include "CommandNotifyPosition.hpp"
#include "CommandNotifyInvertFlow.hpp"
#include "CommandNotifyBorderColour.hpp"
#include "CommandNotifyFlashColour.hpp"
#include "CommandNotifyBgColour.hpp"

#include "CommandSampleToast.hpp"

namespace Stand
{
	CommandListNotifySettings::CommandListNotifySettings(CommandList* parent, CommandColourCustom* primaryColour, CommandColourCustom* bgColour)
		: CommandList(parent, LOC("NOTFS"))
	{
		auto* type = createChild<CommandNotifyType>();
		type->custom_pos = createChild<CommandNotifyPosition>();
		createChild<CommandNotifyInvertFlow>();
		this->createChild<CommandLambdaSlider<CommandSlider>>(LOC("WDT"), CMDNAMES("notifywidth"), NOLABEL, 0, 15000, 400, 10, [](int value, Click& click)
		{
			g_notify_grid.updateSetting([value]
			{
				g_notify_grid.notify_width = value;
			});
		});
		this->createChild<CommandLambdaSlider<CommandSlider>>(LOC("PDDGN"), CMDNAMES("notifypadding"), NOLABEL, 0, 15000, 10, 1, [](int value, Click& click)
		{
			g_notify_grid.updateSetting([value]
			{
				g_notify_grid.notify_padding = value;
			});
		});
		create_rainbow(createChild<CommandNotifyBorderColour>(primaryColour));
		create_rainbow(createChild<CommandNotifyFlashColour>());
		createChild<CommandNotifyBgColour>(bgColour);
		createChild<CommandLambdaAction>(LOC("FLSHNTF"), {}, NOLABEL, [](Click&)
		{
			if (g_toaster == &g_grid_toaster)
			{
				GridToaster::flashPersistentToast();
			}
		});

		// Timing
		{
			timing = this->createChild<CommandList>(LOC("TIMNG"));

			timing->createChild<CommandSliderPointer<CommandSlider, uint16_t>>(LOC("READSPD"), CMDNAMES("readspeed", "readingspeed"), NOLABEL, &g_grid_toaster.reading_speed_wpm, 0, 1'000, 10);
			timing->createChild<CommandSliderPointer<CommandSlider, time_t>>(LOC("READSTRT"), CMDNAMES("readstartdelay", "readingstartdelay"), NOLABEL, &g_grid_toaster.reading_start_delay, 0, 1'000, 50);
			timing->createChild<CommandSliderPointer<CommandSlider, time_t>>(LOC("MINDUR"), CMDNAMES("notifyminduration"), NOLABEL, &g_grid_toaster.notifydur_min, 0, 60'000, 100);
			timing->createChild<CommandSliderPointer<CommandSlider, time_t>>(LOC("MAXDUR"), CMDNAMES("notifymaxduration"), NOLABEL, &g_grid_toaster.notifydur_max, 0, 60'000, 100);
			timing->createChild<CommandSampleToast>();
		}
	}

	void CommandListNotifySettings::onActiveListUpdate()
	{
		if (g_gui.opened
			&& isThisOrSublistCurrentUiListInTab()
			&& !timing->isThisOrSublistCurrentUiListInTab()
			)
		{
			if (!is_active)
			{
				is_active = true;
				Util::ensureToasterContext(g_toaster, []
				{
					showPersistentToast();
				});
			}
		}
		else
		{
			if (is_active)
			{
				is_active = false;
				Util::ensureToasterContext(g_toaster, []
				{
					hidePersistentToast();
				});
			}
		}
	}

	void CommandListNotifySettings::showPersistentToast()
	{
		g_toaster->setPersistentToast(LANG_GET_W("IPSUM"));
	}

	void CommandListNotifySettings::hidePersistentToast()
	{
		g_toaster->clearPersistentToast();
	}

	void CommandListNotifySettings::setToaster(ThreadContext thread_context, Toaster* toaster)
	{
		if (g_toaster == toaster)
		{
			return;
		}
		if (is_active)
		{
			ensureScriptThread(thread_context, [toaster]
			{
				hidePersistentToast();
				g_toaster = toaster;
				showPersistentToast();
			});
		}
		else
		{
			g_toaster = toaster;
		}
	}

	void CommandListNotifySettings::updateGameColour(ThreadContext thread_context)
	{
		if (is_active && g_toaster == &g_game_toaster)
		{
			ensureScriptThread(thread_context, []
			{
				hidePersistentToast();
				showPersistentToast();
			});
		}
	}
}
