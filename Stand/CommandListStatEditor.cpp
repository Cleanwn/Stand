#include "CommandListStatEditor.hpp"

#include "CommandKdEditor.hpp"
#include "CommandLink.hpp"
#include "CommandListWarning.hpp"
#include "CommandSearchLiterals.hpp"
#include "CommandStatBoolSlider.hpp"
#include "CommandStatDate.hpp"
#include "CommandStatFloatSlider.hpp"
#include "CommandStatIntSlider.hpp"
#include "CommandStatPos.hpp"
#include "CommandStatProfileSetting.hpp"
#include "CommandStatString.hpp"

namespace Stand
{
	CommandListStatEditor::CommandListStatEditor(CommandList* const parent)
		: CommandList(parent, LOC("STATEDIT"), CMDNAMES_OBF("stateditor"))
	{
		// Popular Stats
		auto popular = createChild<CommandList>(LOC("STAT_POP"));

		popular->createChild<CommandKdEditor>();

		// Raw Stats
		{
			auto raw = createChild<CommandListWarning>(LOC("STAT_RAW"), ATSTRINGHASH("STAT_WARN"));
			raw->createChild<CommandSearchLiterals<false>>(CMDNAMES_OBF("statsearch"));

			for (auto& stat : Stats::stats)
			{
				CommandPhysical* command = nullptr;

				// Unsure how to approach packed bools and integers. It seems like they use indexes over stat hashes (by looking at usage around here), despite being associated with a stat? Confusing.
				/*if (stat.packed)
				{
					continue;
				}*/

				// Some types are excluded here because they're too likely to exceed the current 32-bit slider limit. For example: DOUBLE, LONG, S64, X64, U64.
				switch (stat.type)
				{
				case Stats::INT:
				case Stats::CARHASH:
					command = raw->createChild<CommandStatIntSlider>(stat, -INT32_MAX, INT32_MAX);
					break;

				case Stats::U16:
					command = raw->createChild<CommandStatIntSlider>(stat, 0, UINT16_MAX);
					break;

				case Stats::U32:
				case Stats::CASH:
				case Stats::SHORT: // The stats seem to work fine with these limits.
					command = raw->createChild<CommandStatIntSlider>(stat, 0, INT32_MAX);
					break;

				case Stats::BOOL:
					command = raw->createChild<CommandStatBoolSlider>(stat);
					break;

				case Stats::FLOAT: // May exceed 32-bit limit.
				case Stats::SPEED:
				case Stats::FRACTION:
				case Stats::DISTANCE:
				case Stats::SHORT_DISTANCE:
					command = raw->createChild<CommandStatFloatSlider>(stat, -INT32_MAX, INT32_MAX);
					break;

				case Stats::POS:
					command = raw->createChild<CommandStatPos>(stat);
					break;

				case Stats::TIME: // May exceed 32-bit limit.
					command = raw->createChild<CommandStatIntSlider>(stat, 0, INT32_MAX);
					break;

				case Stats::BAR:
				case Stats::PERCENT:
					command = raw->createChild<CommandStatIntSlider>(stat, 0, 100);
					break;

				case Stats::LABEL:
				case Stats::STRING:
				case Stats::USERID:
				case Stats::PLAYERID:
					command = raw->createChild<CommandStatString>(stat);
					break;

				case Stats::PROFILESETTING:
					command = raw->createChild<CommandStatProfileSetting>(stat);
					break;

				case Stats::DATE:
					command = raw->createChild<CommandStatDate>(stat);
					break;
				}

				if (command != nullptr && stat.isPopular())
				{
					popular->createChild<CommandLink>(command);
				}
			}
		}
	}
}