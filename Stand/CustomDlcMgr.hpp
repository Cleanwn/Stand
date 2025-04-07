#pragma once

#include <soup/UniquePtr.hpp>

#include "CrossState.hpp"

namespace Stand
{
	class CustomDlcMgr
	{
	private:
		inline static hash_t currently_loading_dlc_rpf_path_hash = 0;

	public:
		[[nodiscard]] static bool isLoadingADlc();

		[[nodiscard]] static bool isDlcLoaded(std::string path);
		static void loadDlc(const std::string& path);
		[[nodiscard]] static std::vector<CustomVehicleModel*> getDlcModels(std::string path);

		[[nodiscard]] static bool isCustomModel(hash_t hash);
		[[nodiscard]] static CVehicleModelInfo* getCustomModelInfo(hash_t hash);
		static void registerModel(CustomVehicleModel* info);
	};
}
