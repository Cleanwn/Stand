#include "CommandSpawnedObject.hpp"

#include <fmt/format.h>

#include "AbstractEntity.hpp"
#include "Box.hpp"
#include "CommandEntityPosition.hpp"
#include "CommandLambdaActionScript.hpp"
#include "Renderer.hpp"

namespace Stand
{
	CommandSpawnedObject::CommandSpawnedObject(CommandList* parent, const Label& menu_name, Object handle)
		: CommandWithOnTickFocused(parent, Label(menu_name)), handle(std::move(handle))
	{
		this->createChild<CommandLambdaActionScript>(LOC("DEL"), {}, NOLABEL, [this](const Click& click)
		{
			this->getEntity().removeFromPlaneOfExistence();
			click.ensureWorkerContext([this]
			{
				this->beGone();
			});
		});
		this->createChild<CommandEntityPosition>(std::vector<CommandName>(1, utf8ToCmdName(fmt::format("objpos{}", handle))), getEntity());
	}

	CommandSpawnedObject::~CommandSpawnedObject()
	{
		getEntity().removeFromPlaneOfExistence();
	}

	AbstractEntity CommandSpawnedObject::getEntity() const
	{
		return AbstractEntity::get(this->handle);
	}

	void CommandSpawnedObject::onTickFocused()
	{
		this->getEntity().getBox().drawLines(int(g_renderer.arColour.x * 255.0f), int(g_renderer.arColour.y * 255.0f), int(g_renderer.arColour.z * 255.0f), int(g_renderer.arColour.w * 255.0f));
	}
}
