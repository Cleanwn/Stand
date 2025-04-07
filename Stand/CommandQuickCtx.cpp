#include "CommandQuickCtx.hpp"

#include "CommandQuickCtxSave.hpp"
#include "CommandQuickCtxLoad.hpp"
#include "CommandQuickCtxDefault.hpp"
#include "CommandQuickCtxRDefault.hpp"
#include "CommandQuickCtxMin.hpp"
#include "CommandQuickCtxMax.hpp"

namespace Stand
{
	CommandQuickCtx::CommandQuickCtx(CommandList* const parent)
		: CommandList(parent, LOC("QWQCTX"))
	{
		instance = this;

		save_state = this->createChild<CommandQuickCtxSave>();
		load_state = this->createChild<CommandQuickCtxLoad>();
		apply_default_state = this->createChild<CommandQuickCtxDefault>();
		apply_default_state_to_children = this->createChild<CommandQuickCtxRDefault>();
		min = this->createChild<CommandQuickCtxMin>();
		max = this->createChild<CommandQuickCtxMax>();
	}

	CommandQuickCtx::~CommandQuickCtx()
	{
		if (instance == this)
		{
			instance = nullptr;
		}
	}
}
