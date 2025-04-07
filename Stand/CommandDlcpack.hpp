#pragma once

#include "CommandList.hpp"

#include <filesystem>

#include "CommandListFolderItemData.hpp"
#include "CustomDlcMgr.hpp"
#include "Util.hpp"

#include "CommandVehicleDlc.hpp"

namespace Stand
{
	class CommandDlcpack : public CommandListFolderItem<CommandList, CommandDlcpack>
	{
	private:
		bool loading = false;
	public:
		CommandListFolderItemData folder_item_data;

		explicit CommandDlcpack(CommandList* const parent, std::wstring&& filename_no_ext, CommandListFolderItemData&& data)
			: CommandListFolderItem(parent, LIT(std::move(filename_no_ext)), {}, NOLABEL, CMDFLAGS_LIST | CMDFLAG_FEATURELIST_SKIP), folder_item_data(std::move(data))
		{
			indicator_type = LISTINDICATOR_ARROW_IF_CHILDREN;
			try
			{
				if (CustomDlcMgr::isDlcLoaded(getDlcPath()))
				{
					populateDlcContent();
				}
			}
			catch (std::exception&)
			{
			}
		}

		[[nodiscard]] std::string getDlcPath() const
		{
			try
			{
				auto path = std::filesystem::path(getFilePath()).string();
				path.push_back('\\');
				return path;
			}
			catch (std::exception&)
			{
				throw std::exception("Path is not representable in native locale");
			}
		}

		void onClick(Click& click) final
		{
			if (!click.isRegularEditionOrSp())
			{
				return;
			}
			if (thread_context_has_game_tls(click.thread_context))
			{
				if (!loading)
				{
					// Loading a DLC causes the game to present again with stream-proof rendering, so we would process input again, and receive another onClick. Just using a dedup variable to make sure we don't process the click twice.
					loading = true;
					if (!CustomDlcMgr::isDlcLoaded(getDlcPath()))
					{
						CustomDlcMgr::loadDlc(getDlcPath());
					}
					loading = false;
					if (children.empty())
					{
						if (!populateDlcContent())
						{
							click.setResponse(LOC("CUSTDLC_ETY"));
							onClickImplUnavailable(click);
							return;
						}
					}
					CommandListFolderItem::onClick(click);
				}
			}
			else
			{
				CommandListFolderItem::onClick(click);
				FiberPool::queueJob([this]
				{
					if (!CustomDlcMgr::isDlcLoaded(getDlcPath()))
					{
						CustomDlcMgr::loadDlc(getDlcPath());
					}
					if (children.empty())
					{
						if (populateDlcContent())
						{
							processChildrenUpdate();
						}
						else
						{
							Util::toast(LOC("CUSTDLC_ETY"));
						}
					}
				});
			}
		}

	private:
		bool populateDlcContent()
		{
			const auto models = CustomDlcMgr::getDlcModels(getDlcPath());
			if (models.empty())
			{
				return false;
			}
			for (const auto& model : models)
			{
				createChild<CommandVehicleDlc>(model);
			}
			return true;
		}
	};
}
