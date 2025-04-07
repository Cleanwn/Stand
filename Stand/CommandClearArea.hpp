#pragma once

#include "CommandListWithOnTickAsActiveList.hpp"
#include "CommandList.hpp"

#include "AbstractEntity.hpp"
#include "Box.hpp"

namespace Stand
{
	class CommandClearArea : public CommandListWithOnTickAsActiveList<CommandList>
	{
	private:
		CommandSlider* distance;
		CommandToggleNoCorrelation* vehicles;
		CommandToggleNoCorrelation* peds;
		CommandToggleNoCorrelation* objects;
		CommandToggleNoCorrelation* cages;
		CommandToggleNoCorrelation* pickups;
		CommandToggleNoCorrelation* ignore_mission_entities;
		CommandToggleNoCorrelation* no_delay;

		struct WorkItem
		{
			AbstractEntity ent;
			Box box;
			time_t started_at;
		};

		std::vector<AbstractEntity> to_delete{};
		std::vector<WorkItem> deleting{};

	public:
		explicit CommandClearArea(CommandList* const parent);

		[[nodiscard]] bool hasWorkToDo() const noexcept;

		[[nodiscard]] std::vector<AbstractEntity> getEntitiesMatchingFilters() const noexcept;

		void onTickAsActiveList() final;
	};
}
