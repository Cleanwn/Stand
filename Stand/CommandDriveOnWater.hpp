#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"

namespace Stand
{
	class CommandDriveOnWater : public CommandToggle
	{
	public:
		static inline AbstractEntity object = AbstractEntity::invalid();

	private:
		static inline constexpr Hash object_hash = ATSTRINGHASH("ar_prop_ar_bblock_huge_01");

	public:
		explicit CommandDriveOnWater(CommandList* const parent);

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;

	private:
		void createOrUpdateObject(const v3& pos, float water_height);

		[[nodiscard]] static std::optional<float> getWaterHeight(const v3& pos);

		void reset();
	};
}