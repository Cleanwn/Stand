#include "CommandClearArea.hpp"

#include <soup/macros.hpp>

#include "AbstractModel.hpp"
#include "CommandLambdaActionScript.hpp"
#include "CommandSliderProximity.hpp"
#include "CommandToggle.hpp"
#include "get_current_time_millis.hpp"
#include "Script.hpp"
#include "vector.hpp"

namespace Stand
{
	CommandClearArea::CommandClearArea(CommandList* const parent)
		: CommandListWithOnTickAsActiveList(parent, LOC("CLRARE"))
	{
		this->createChild<CommandLambdaActionScript>(LOC("CLRARE"), CMDNAMES("cleararea"), NOLABEL, [this](const Click& click)
		{
			if (hasWorkToDo())
			{
				click.setResponse(LOC("GENWAIT"));
				return;
			}
			to_delete = getEntitiesMatchingFilters();
			while (hasWorkToDo())
			{
				for (auto i = to_delete.begin(); i != to_delete.end(); )
				{
					if (i->exists())
					{
						i->getBox().drawLines(255, 255, 0, 255);
						++i;
					}
					else
					{
						i = to_delete.erase(i);
					}
				}
				if (!to_delete.empty())
				{
					if (deleting.empty() || no_delay->m_on)
					{
						do
						{
							auto ent = to_delete.back();
							to_delete.pop_back();
							if (ent.canBeDeleted())
							{
								deleting.emplace_back(WorkItem{ ent, ent.getBox(), get_current_time_millis() });
								ent.removeFromPlaneOfExistenceNocheck();
							}
						} while (no_delay->m_on && !to_delete.empty());
					}
				}
				for (auto i = deleting.begin(); i != deleting.end(); )
				{
					if (i->ent.exists()
						&& GET_MILLIS_SINCE(i->started_at) < 1000
						)
					{
						i->box.drawLines(255, 0, 0, 255);
						++i;
					}
					else
					{
						i = deleting.erase(i);
					}
				}
				Script::current()->yield();
			}
		});
		distance = this->createChild<CommandSliderProximity>(CMDNAMES("clearproximity"), 5000);
		vehicles = this->createChild<CommandToggle>(LOC("VEHS"), CMDNAMES("clearvehicles"), NOLABEL, true);
		peds = this->createChild<CommandToggle>(LOC("PEDS"), CMDNAMES("clearpeds"), NOLABEL, true);
		objects = this->createChild<CommandToggle>(LOC("OBJS"), CMDNAMES("clearobjects"), NOLABEL, true);
		cages = this->createChild<CommandToggle>(LOC("CGES"), CMDNAMES("clearcages"), NOLABEL, true);
		pickups = this->createChild<CommandToggle>(LOC("PUPS"), CMDNAMES("clearpickups"), NOLABEL, true);
		ignore_mission_entities = this->createChild<CommandToggle>(LOC("IGMISSENT"), CMDNAMES("clearnomission"));
		no_delay = this->createChild<CommandToggle>(LOC("NDLY"), CMDNAMES("clearnodelay"));
	}

	bool CommandClearArea::hasWorkToDo() const noexcept
	{
		return !to_delete.empty()
			|| !deleting.empty()
			;
	}

	std::vector<AbstractEntity> CommandClearArea::getEntitiesMatchingFilters() const noexcept
	{
		std::vector<AbstractEntity> entities{};
		const v3 player_pos = g_player_ped.getPos();
		auto ent_consumer = [&entities, this, player_pos](AbstractEntity&& ent)
		{
			if (ent.getPos().distance(player_pos) < distance->getFloatValue()
				&& (!ignore_mission_entities->m_on || !ENTITY::IS_ENTITY_A_MISSION_ENTITY(ent))
				)
			{
				entities.emplace_back(ent);
			}
			CONSUMER_CONTINUE;
		};
		if (vehicles->m_on)
		{
			AbstractEntity::getAllVehicles([&](AbstractEntity&& ent)
			{
				if (ent != g_player_veh)
				{
					ent_consumer(std::move(ent));
				}
				CONSUMER_CONTINUE;
			});
		}
		if (peds->m_on)
		{
			AbstractEntity::getAllPeds([&](AbstractEntity&& ent)
			{
				if (!ent.isAPlayer())
				{
					ent_consumer(std::move(ent));
				}
				CONSUMER_CONTINUE;
			});
		}
		if (objects->m_on)
		{
			AbstractEntity::getAllObjects(ent_consumer);
		}
		if (cages->m_on)
		{
			AbstractEntity::getAllObjects([&](AbstractEntity&& ent)
			{
				if (ent.getModel().isCageObject())
				{
					ent_consumer(std::move(ent));
				}
				CONSUMER_CONTINUE;
			});
		}
		if (pickups->m_on)
		{
			AbstractEntity::getAllPickups(ent_consumer);
		}
		return entities;
	}

	void CommandClearArea::onTickAsActiveList()
	{
		if (!hasWorkToDo())
		{
			for (auto& ent : getEntitiesMatchingFilters())
			{
				ent.getBox().drawLines(0, 255, 0, 255);
			}
		}
	}
}
