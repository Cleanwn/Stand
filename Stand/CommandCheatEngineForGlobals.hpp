#pragma once

#include "CommandList.hpp"

#include "script_program.hpp"

namespace Stand
{
	class CommandCheatEngineForGlobals : public CommandList
	{
	private:
		std::unordered_map<uint32_t, rage::scrValue> capture;

	public:
		explicit CommandCheatEngineForGlobals(CommandList* const parent)
			: CommandList(parent, LIT("Cheat Engine for Globals"))
		{
			this->createChild<CommandLambdaAction>(LIT("Initial"), CMDNAMES(), NOLABEL, [this](Click&)
			{
				capture.clear();
				capture.reserve(rage::scrProgram::MAX_GLOBAL_BLOCKS * (rage::scrProgram::GLOBAL_SIZE_MASK / sizeof(rage::scrValue)));
				for (uint32_t block = 0; block != rage::scrProgram::MAX_GLOBAL_BLOCKS; ++block)
				{
					if (pointers::rage_scrProgram_sm_Globals[block])
					{
						const uint32_t globals_in_block = pointers::rage_scrProgram_sm_GlobalSizes[block];
						for (uint32_t offset = 0; offset != globals_in_block; ++offset)
						{
							uint32_t global = (block << rage::scrProgram::MAX_GLOBAL_BLOCKS_SHIFT) | offset;
							capture.emplace(global, pointers::rage_scrProgram_sm_Globals[block][offset]);
						}
					}
				}
				Util::toast(fmt::format("{} globals captured.", capture.size()));
			});
			this->createChild<CommandLambdaAction>(LIT("Unchanged"), CMDNAMES(), LIT("Removes all globals that have changed."), [this](Click&)
			{
				for (auto i = capture.begin(); i != capture.end(); )
				{
					if (ScriptGlobal(i->first).get<rage::scrValue>() != i->second)
					{
						i = capture.erase(i);
					}
					else
					{
						++i;
					}
				}
				Util::toast(fmt::format("{} globals remain.", capture.size()));
			});
			this->createChild<CommandLambdaAction>(LIT("Changed"), CMDNAMES(), NOLABEL, [this](Click&)
			{
				for (auto i = capture.begin(); i != capture.end(); )
				{
					auto curr = ScriptGlobal(i->first).get<rage::scrValue>();
					if (curr == i->second)
					{
						i = capture.erase(i);
					}
					else
					{
						i->second = curr;
						++i;
					}
				}
				Util::toast(fmt::format("{} globals remain.", capture.size()));
			});
			this->createChild<CommandLambdaAction>(LIT("Log Globals"), CMDNAMES(), NOLABEL, [this](Click&)
			{
				for (auto i = capture.begin(); i != capture.end(); ++i)
				{
					g_logger.log(fmt::format("Global_{}: {}", i->first, i->second.Any));
				}
			});
		}
	};
}
