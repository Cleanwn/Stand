#include "CommandVisualZCorrection.hpp"

#include "AbstractEntity.hpp"
#include "free_movement.hpp"
#include "get_current_time_millis.hpp"
#include "Gui.hpp"
#include "gta_ped.hpp"
#include "natives.hpp"
#include "UniqueThread.hpp"

namespace Stand
{
	CommandVisualZCorrection::CommandVisualZCorrection(CommandList* parent)
		: CommandSliderFloat(parent, LOC("VISZ"), { CMDNAME("visualz") }, LOC("VISZ_H"), -1000000, 1000000, 0, 10)
	{
	}

	void CommandVisualZCorrection::onChange(Click& click, int prev_value)
	{
		if (value == 0)
		{
			stopThreadIfRunning();
		}
		else
		{
			int value = this->value;
			registerScriptTickEventHandler(click, [this, value]
			{
				HANDLER_CHECK(value == this->value)
				if (PED::IS_PED_FALLING(g_player_ped))
				{
					if (getFloatValue() > 0.5f)
					{
						if (fall_start == 0)
						{
							fall_start = get_current_time_millis();
						}
						else if (GET_MILLIS_SINCE(fall_start) > 1000)
						{
							timeout_end = get_current_time_millis() + 1000;
						}
					}
				}
				else
				{
					fall_start = 0;
				}
				if (g_player_veh.isValid() || levitate_is_on || timeout_end > get_current_time_millis())
				{
					stopThreadIfRunning();
				}
				else
				{
					ensureThreadIsRunning();
				}
				HANDLER_END
			});
		}
	}

	void CommandVisualZCorrection::stopThreadIfRunning()
	{
		if (thread != nullptr)
		{
			thread->stop();
			thread = nullptr;
		}
	}

	void CommandVisualZCorrection::ensureThreadIsRunning()
	{
		if (thread == nullptr)
		{
			thread = UniqueThread::create([this](UniqueThread* tp)
			{
				float visual_z = -FLT_MAX;
				while (!tp->stopped)
				{
					if (((CPed*)g_player_ped)->matrix.z != visual_z)
					{
						visual_z = (((CPed*)g_player_ped)->matrix.z += getFloatValue());
					}
				}
			});
		}
	}
}
