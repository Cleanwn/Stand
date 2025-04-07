#include "CommandSavedVehicle.hpp"

#include <iostream>

#include "ColonsAndTabs.hpp"
#include "ExecCtx.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandSavedVehicle::CommandSavedVehicle(CommandList* parent, std::wstring&& name, CommandListFolderItemData&& folder_item_data)
		: CommandWithEntityPreview(parent, LIT(std::move(name)), CMDNAMES("vehicle"), NOLABEL, CMDFLAGS_ACTION | CMDFLAG_SEARCH_FINISHLIST | CMDFLAG_FEATURELIST_SKIP), folder_item_data(std::move(folder_item_data))
	{
		StringUtils::simplify(command_names.at(0).append(this->menu_name.getLiteralForCommandName()));
	}

	bool CommandSavedVehicle::ensureData()
	{
		if (data.data.empty())
		{
			std::ifstream file_in(getFilePath());
			ColonsAndTabs::read(file_in, data.data);

			if (data.data.find("Headlights") == data.data.end()) // < 0.62.1
			{
				auto old = data.data.find("Xenon Lights");
				if (old != data.data.end())
				{
					auto val = std::move(old->second);
					data.data.erase(old);
					data.data.emplace("Headlights", std::move(val));
				}

				auto primary = data.data.find("Primary Colour");
				if (primary != data.data.end()
					&& primary->second.at(0) == '#'
					)
				{
					auto val = std::move(primary->second);
					data.data.erase(primary);
					data.data.emplace("Custom Primary Colour", std::move(val));
				}

				auto secondary = data.data.find("Secondary Colour");
				if (secondary != data.data.end()
					&& secondary->second.at(0) == '#'
					)
				{
					auto val = std::move(secondary->second);
					data.data.erase(secondary);
					data.data.emplace("Custom Secondary Colour", std::move(val));
				}
			}

			if (auto e = data.data.find("Unk17"); e != data.data.end()) // 0.88
			{
				auto val = std::move(e->second);
				data.data.erase(e);
				data.data.emplace("Nitrous", std::move(val));
			}

			if (auto e = data.data.find("Unk19"); e != data.data.end()) // 0.88
			{
				auto val = std::move(e->second);
				data.data.erase(e);
				data.data.emplace("Subwoofer", std::move(val));
			}

			if (auto e = data.data.find("Windows"); e != data.data.end()) // 0.88
			{
				auto val = std::move(e->second);
				data.data.erase(e);
				data.data.emplace("Left Door", std::move(val));
			}

			if (auto e = data.data.find("Unk47"); e != data.data.end()) // 0.88
			{
				auto val = std::move(e->second);
				data.data.erase(e);
				data.data.emplace("Right Door", std::move(val));
			}

			if (auto e = data.data.find("Unk49"); e != data.data.end()) // 0.88
			{
				auto val = std::move(e->second);
				data.data.erase(e);
				data.data.emplace("Lightbar", std::move(val));
			}
		}
		return !data.data.empty();
	}

	void CommandSavedVehicle::onClick(Click& click)
	{
		this->data.data.clear();
		if (!ensureData())
		{
			getCommandListFolder()->refresh();
		}
		else
		{
			ensureScriptThread(click, [this, click]() mutable
			{
				click.thread_context = ExecCtx::get().tc;
				data.spawnClone(click);
			});
		}
	}

	Hash CommandSavedVehicle::getModelHash()
	{
		if (ensureData())
		{
			return data.read(VehicleCustomisationIo::MODEL);
		}
		return 0;
	}

	AbstractEntity CommandSavedVehicle::createPreviewEntity()
	{
		return data.createEntityNow(false);
	}
}
