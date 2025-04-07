#pragma once

#include "CommandToggle.hpp"

#include <array>
#include <map>
#include <unordered_map>

#include "natives_decl.hpp"

#include "outfit_data.hpp"

namespace Stand
{
	class CommandLockOutfit final : public CommandToggleNoCorrelation
	{
	public:
		static inline CommandLockOutfit* instance = nullptr;
	private:
		static inline Hash player_prev_model = 0;
	public:
		static inline std::map<Hash, std::array<std::int16_t, COMPACT_OUTFIT_SIZE>> model_outfits = {};
		static inline std::unordered_map<int, std::pair<CommandOutfitComponent*, CommandOutfitComponentVariation*>> ped_component_commands = {};
		static inline CommandSlider* hair_highlight = nullptr;
		static inline std::unordered_map<int, std::pair<CommandOutfitProp*, CommandOutfitPropVariation*>> ped_prop_commands = {};

		explicit CommandLockOutfit(CommandList* const parent);
		~CommandLockOutfit() final;

		static void onTick();
		static void updateOutfitCommands();

		void onClick(Click& click) final;

		std::string getState() const final;
		std::string getDefaultState() const final;
		void setState(Click& click, const std::string& state) final;
		void applyDefaultState() final;

		void updateOutfit(void(*cb)());

		static void setHairColourFromCommands();
	};
}
