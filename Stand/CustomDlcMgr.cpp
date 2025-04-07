#include "CustomDlcMgr.hpp"

#include "CExtraContentManager.hpp"
#include "CommandListAssetHashes.hpp"
#include "Exceptional.hpp"
#include "pointers.hpp"

namespace Stand
{
	bool CustomDlcMgr::isLoadingADlc()
	{
		return currently_loading_dlc_rpf_path_hash != 0;
	}

	bool CustomDlcMgr::isDlcLoaded(std::string path)
	{
		path.append(soup::ObfusString("dlc.rpf").str());
		for (const auto& dlc : (*pointers::extra_content_manager)->m_content)
		{
			if (dlc.m_filename.data
				&& path == dlc.m_filename.data
				)
			{
				return true;
			}
		}
		return false;
	}

	void CustomDlcMgr::loadDlc(const std::string& path)
	{
		std::string dlc_rpf_path = path;
		dlc_rpf_path.append(soup::ObfusString("dlc.rpf").str());
		currently_loading_dlc_rpf_path_hash = rage::atStringHash(dlc_rpf_path);
		__try
		{
			pointers::CExtraContentManager_AddContentFolder(*pointers::extra_content_manager, path.c_str());
			pointers::CExtraContentManager_LoadContent(*pointers::extra_content_manager, true, false);
			(*pointers::extra_content_manager)->m_everHadBadPackOrder = false;
			CommandListAssetHashes::instance->processCrcUpdate();
		}
		__EXCEPTIONAL()
		{
		}
		currently_loading_dlc_rpf_path_hash = 0;
	}

	std::vector<CustomVehicleModel*> CustomDlcMgr::getDlcModels(std::string path)
	{
		std::vector<CustomVehicleModel*> models;
		{
			path.append(soup::ObfusString("dlc.rpf").str());
			const auto dlc_rpf_hash_name = rage::atStringHash(path);
			for (auto node = CrossState::get().custom_vehicle_models_head; node; node = node->next)
			{
				if (node->dlc_rpf_hash_name == dlc_rpf_hash_name)
				{
					models.emplace_back(node);
				}
			}
		}
		return models;
	}

	bool CustomDlcMgr::isCustomModel(hash_t hash)
	{
		return getCustomModelInfo(hash) != nullptr;
	}

	CVehicleModelInfo* CustomDlcMgr::getCustomModelInfo(hash_t hash)
	{
		{
			for (auto node = CrossState::get().custom_vehicle_models_head; node; node = node->next)
			{
				if (node->hash == hash)
				{
					return node;
				}
			}
		}
		return nullptr;
	}

	void CustomDlcMgr::registerModel(CustomVehicleModel* info)
	{
		info->dlc_rpf_hash_name = currently_loading_dlc_rpf_path_hash;

		CrossState& cs = CrossState::get();
#if true
		info->next = cs.custom_vehicle_models_head;
		cs.custom_vehicle_models_head = info;
#else
		info->next = nullptr;
		cs.getCustomVehicleModelsTail() = info;
#endif
	}
}
