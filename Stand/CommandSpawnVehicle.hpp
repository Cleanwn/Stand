#pragma once

#include "CommandActionModel.hpp"

#include "CommandInput.hpp"
#include "CommandListSelect.hpp"
#include "CommandToggle.hpp"

namespace Stand
{
	class CommandSpawnVehicle : public CommandActionModel
	{
	private:
		CommandListSelect* const spawntuned;
		CommandToggleNoCorrelation* const onfoot_spawnin;
		CommandToggleNoCorrelation* const inveh_spawnin;
		CommandToggleNoCorrelation* const onfoot_airinair;
		CommandToggleNoCorrelation* const inveh_airinair;
		CommandToggleNoCorrelation* const onfoot_spawnfront;
		CommandToggleNoCorrelation* const inveh_spawnfront;
		CommandToggleNoCorrelation* const onfoot_spawndel;
		CommandToggleNoCorrelation* const inveh_spawndel;
		CommandToggleNoCorrelation* const spawngod;
		CommandToggleNoCorrelation* const inveh_spawnvel;
		CommandToggleNoCorrelation* const spawncoloured;
		CommandColour* const spawncolourprimary;
		CommandColour* const spawncoloursecondary;
		CommandToggleNoCorrelation* const onfoot_likepv;
		CommandToggleNoCorrelation* const inveh_likepv;
		CommandInput* const spawn_plate;

	public:
		inline static CommandSpawnVehicle* instance;

		explicit CommandSpawnVehicle(CommandList* const parent, CommandListSelect* spawntuned, CommandToggleNoCorrelation* onfoot_spawnin, CommandToggleNoCorrelation* inveh_spawnin, CommandToggleNoCorrelation* onfoot_airinair, CommandToggleNoCorrelation* inveh_airinair, CommandToggleNoCorrelation* onfoot_spawnfront, CommandToggleNoCorrelation* inveh_spawnfront, CommandToggleNoCorrelation* onfoot_spawndel, CommandToggleNoCorrelation* inveh_spawndel, CommandToggleNoCorrelation* spawngod, CommandToggleNoCorrelation* inveh_spawnvel, CommandToggleNoCorrelation* spawncoloured, CommandColour* spawncolourprimary, CommandColour* spawncoloursecondary, CommandToggleNoCorrelation* onfoot_likepv, CommandToggleNoCorrelation* inveh_likepv, CommandInput* spawn_plate);

		bool spawnIfValid(const AbstractModel& model, Click& click, const Label& name) const final;
		hash_t getInvalidModelMessage() const final;

		[[nodiscard]] AbstractEntity createVehicle(Click& click, hash_t hash) const; // needs a yieldable script thread
		void applySpawnTuning(AbstractEntity& veh) const;
	};
}
