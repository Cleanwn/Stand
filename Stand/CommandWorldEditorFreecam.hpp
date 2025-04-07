#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandWorldEditorFreecam : public CommandToggle
	{
	public:
		explicit CommandWorldEditorFreecam(CommandList* const parent)
			: CommandToggle(parent, LOC("FC"), { CMDNAME("editorfreecam") }, NOLABEL, true)
		{
		}

		void onEnable(Click& click) final
		{
			parent->parent->as<CommandWorldEditor>()->freecam = true;
			if (parent->parent->as<CommandWorldEditor>()->active)
			{
				CommandWorldEditor::enableFreecam();
			}
		}

		void onDisable(Click& click) final
		{
			parent->parent->as<CommandWorldEditor>()->freecam = false;
			if (parent->parent->as<CommandWorldEditor>()->active)
			{
				FiberPool::queueJob([]
				{
					g_tb_freecam.disable();
				});
			}
		}
	};
}
