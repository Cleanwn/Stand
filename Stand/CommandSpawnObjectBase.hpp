#pragma once

#include "AbstractEntity.hpp"
#include "AbstractModel.hpp"
#include "CommandSpawnedObject.hpp"
#include "using_model.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "Util.hpp"

namespace Stand
{
	template <typename T>
	class CommandSpawnObjectBase : public T
	{
	private:
		CommandList* const spawned_objects;

	protected:
		explicit CommandSpawnObjectBase(CommandList* const parent, CommandList* spawned_objects, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, const commandflags_t flags = 0)
			: T(parent, std::move(menu_name), std::move(command_names), std::move(help_text), flags), spawned_objects(spawned_objects)
		{
		}

	public:
		void doSpawn(Label&& name, const AbstractModel& model) const
		{
			using_model(model, [this, name{ std::move(name) }, model]() mutable
			{
				doSpawnStreamed(std::move(name), model);
			});
		}

		AbstractEntity doSpawnStreamed(Label&& name, const AbstractModel& model) const
		{
			auto obj = Util::createObject(model, g_player_ped.getPos());
			obj.setOrigin(g_player_ped.getPos());
			auto rot = g_player_ped.getRot();
			ENTITY::SET_ENTITY_ROTATION(obj, rot.x, rot.y, rot.z + 90.0f, 2, true);
			spawned_objects->createChild<CommandSpawnedObject>(std::move(name), obj);
			return obj;
		}
	};
}
