#pragma once

#include "CommandSpawnObjectBase.hpp"
#include "CommandActionModel.hpp"

#include "get_next_arg.hpp"
#include "lang.hpp"
#include "pointers.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class CommandSpawnAnyObject : public CommandSpawnObjectBase<CommandActionModel>
	{
	public:
		explicit CommandSpawnAnyObject(CommandList* parent, CommandList* spawned_objects)
			: CommandSpawnObjectBase(parent, spawned_objects, LOC("INPTVEHMDL"), { CMDNAME("spawnobject") }, LOC("INPTOBJMDL_H"))
		{
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (click.isRegularEdition())
			{
				CommandSpawnObjectBase<CommandActionModel>::onCommand(click, args);
			}
		}

		bool spawnIfValid(const AbstractModel& model, Click& click, const Label& name) const final
		{
			if (model.isValidModel())
			{
				doSpawn(Label(name), model);
				return true;
			}
			return false;
		}
	};
}
