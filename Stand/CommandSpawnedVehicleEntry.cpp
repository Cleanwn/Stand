#include "CommandSpawnedVehicleEntry.hpp"

#include "Box.hpp"
#include "CommandLambdaActionScript.hpp"
#include "dedsec_hacks.hpp"
#include "Renderer.hpp"

namespace Stand
{
	CommandSpawnedVehicleEntry::CommandSpawnedVehicleEntry(CommandList* const parent, Label&& menu_name, AbstractEntity entity)
		: CommandWithOnTickFocused(parent, std::move(menu_name)), entity(entity)
	{
		createChild<CommandLambdaActionScript>(LOC("VEH_DRV"), {}, NOLABEL, [this](const Click& click)
		{
			DedsecHack::drive.execute(this->entity);
		});

		createChild<CommandLambdaActionScript>(LOC("VEH_ENT"), {}, NOLABEL, [this](const Click& click)
		{
			DedsecHack::enter.execute(this->entity);
		});

		createChild<CommandLambdaActionScript>(LOC("VEH_EMPTY"), {}, NOLABEL, [this](const Click& click)
		{
			DedsecHack::empty.execute(this->entity);
		});

		createChild<CommandLambdaActionScript>(LOC("DEL"), {}, NOLABEL, [this](const Click& click)
		{
			this->entity.removeFromPlaneOfExistence();
			beGone();
		});

		createChild<CommandLambdaActionScript>(LOC("CALLVEH"), {}, NOLABEL, [this](const Click& click)
		{
			this->entity.call();
		});
	}

	void CommandSpawnedVehicleEntry::onTickFocused()
	{
		drawBox();
	}

	void CommandSpawnedVehicleEntry::onTickAsActiveList()
	{
		drawBox();
	}

	void CommandSpawnedVehicleEntry::onTickInGameViewport()
	{
		if (!entity.exists())
		{
			beGone();
		}
		else
		{
			return CommandWithOnTickFocused::onTickInGameViewport();
		}
	}

	void CommandSpawnedVehicleEntry::onTickInWebViewport()
	{
		return onTickInWebViewportImplRedirect();
	}

	void CommandSpawnedVehicleEntry::drawBox()
	{
		entity.getBox().drawLines(int(g_renderer.arColour.x * 255.0f), int(g_renderer.arColour.y * 255.0f), int(g_renderer.arColour.z * 255.0f), int(g_renderer.arColour.w * 255.0f));
	}
}