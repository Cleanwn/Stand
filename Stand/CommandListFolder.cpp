#include "CommandListFolder.hpp"

#include <soup/filesystem.hpp>

#include "alphanum.hpp"
#include "CommandLambdaAction.hpp"
#include "CommandListFolderItemData.hpp"
#include "get_appdata_path.hpp"
#include "Gui.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandListFolder::CommandListFolder(CommandList* const parent, std::wstring&& folder_name, std::vector<std::wstring>&& ext, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text)
		: CommandListRefreshable(parent, std::move(menu_name), std::move(command_names), std::move(help_text)), folder_name(std::move(folder_name)), ext(std::move(ext))
	{
	}

	std::wstring CommandListFolder::getFolderPath() const
	{
		std::wstring dir = folder_name;
		dir.push_back(L'\\');
		return get_appdata_path(dir);
	}

	std::wstring CommandListFolder::getFilePath(const CommandListFolderItemData* item_data) const
	{
		return std::move(getFolderPath().append(item_data->rel_path));
	}

	void CommandListFolder::populateHead()
	{
		createOpenFolderAction();
	}

	void CommandListFolder::createOpenFolderAction(Label&& help_text)
	{
		createChild<CommandLambdaAction>(LOC("OPNFLD"), CMDNAMES(), std::move(help_text), [this](Click&)
		{
			g_gui.shellExecute(getFolderPath().c_str());
		});
	}

	std::unordered_set<std::wstring> CommandListFolder::getListedItems() const
	{
		std::unordered_set<std::wstring> listed_items;
		for (auto i = children.cbegin() + getFullBodyOffset(); i != children.cend(); ++i)
		{
			if (isSubFolder(i->get()))
			{
				recursivelyGetListedItems(listed_items, (*i)->as<CommandList>());
			}
			else
			{
				listed_items.emplace(getItemData(i->get())->rel_path);
			}
		}
		return listed_items;
	}

	void CommandListFolder::recursivelyGetListedItems(std::unordered_set<std::wstring>& listed_items, const CommandList* list) const
	{
		for (const auto& cmd : list->children)
		{
			if (isSubFolder(cmd.get()))
			{
				recursivelyGetListedItems(listed_items, cmd->as<CommandList>());
			}
			else
			{
				listed_items.emplace(getItemData(cmd.get())->rel_path);
			}
		}
	}

	CommandList* CommandListFolder::findFolder(const std::string& name) const
	{
		for (const auto& cmd : children)
		{
			if (isSubFolder(cmd.get()))
			{
				if (cmd->as<CommandList>()->menu_name.literal_str == name)
				{
					return cmd->as<CommandList>();
				}
			}
		}
		return nullptr;
	}

	void CommandListFolder::populateBody()
	{
		const auto folder_path = getFolderPath();
		if (!std::filesystem::exists(folder_path))
		{
			std::filesystem::create_directory(folder_path);
		}
		std::unordered_set<std::wstring> listed_items = getListedItems();
		recursivelyPopulateBody(this, listed_items, folder_path, {});
		postPopulateBody();
	}

	void CommandListFolder::recursivelyPopulateBody(CommandList* list, const std::unordered_set<std::wstring>& listed_items, const std::wstring& path, const std::wstring& rel_path) const
	{
		std::error_code ec{};
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path, ec))
		{
			if (entry.is_regular_file())
			{
				auto filename = entry.path().wstring().substr(path.length());
				for (const auto& supported_ext : this->ext)
				{
					if (filename.length() > supported_ext.size())
					{
						auto file_rel_path = std::move(std::wstring(rel_path).append(filename));
						if (!listed_items.count(file_rel_path))
						{
							auto ext = filename.substr(filename.length() - supported_ext.size());
							StringUtils::to_lower(ext);
							if (ext == supported_ext)
							{
								// std::string file_path = std::move(std::string(path).append(filename));
								createItem(list, filename.substr(0, filename.length() - supported_ext.size()), CommandListFolderItemData{ std::move(file_rel_path) });
							}
						}
					}
				}
			}
			else if (entry.is_directory())
			{
				auto filename = entry.path().wstring().substr(path.length());
				auto folder_rel_path = std::move(std::wstring(rel_path).append(filename));
				if (ext.empty()) // Folder mode?
				{
					if (!listed_items.count(folder_rel_path))
					{
						createItem(list, std::move(filename), CommandListFolderItemData{ std::move(folder_rel_path) });
					}
					continue;
				}
				if (canShowFolder(folder_rel_path))
				{
					Command* found = list->resolveChildByMenuNameLiteral(StringUtils::utf16_to_utf8(filename));
					if (found != nullptr && isSubFolder(found))
					{
						auto dir = (CommandList*)found;
						std::wstring path_{ path };
						path_.append(filename).push_back(L'\\');
						folder_rel_path.push_back(L'\\');
						recursivelyPopulateBody(dir, listed_items, std::move(path_), folder_rel_path);
						if (dir->children.empty())
						{
							dir->beGone();
						}
					}
					else
					{
						auto dir = list->makeChild<CommandList>(LIT(filename), {}, NOLABEL, CMDFLAGS_LIST | CMDFLAG_SEARCH_CHILDRENONLY | CMDFLAG_FEATURELIST_SKIP);
						filename.push_back(L'\\');
						std::wstring path_{ path };
						path_.append(filename).push_back(L'\\');
						folder_rel_path.push_back(L'\\');
						recursivelyPopulateBody(dir.get(), listed_items, std::move(path_), folder_rel_path);
						if (!dir->children.empty())
						{
							list->children.emplace_back(std::move(dir));
						}
					}
				}
			}
		}
	}

	void CommandListFolder::postPopulateBody() const
	{
	}

	void CommandListFolder::sortImpl()
	{
		return sortBodyRecursively([](const std::unique_ptr<Command>& a, const std::unique_ptr<Command>& b)
		{
			if (isSubFolder(a.get()) && !isSubFolder(b.get()))
			{
				return true;
			}
			if (isSubFolder(b.get()) && !isSubFolder(a.get()))
			{
				return false;
			}
			std::string a_name = a->getMenuName().getLocalisedUtf8();
			std::string b_name = b->getMenuName().getLocalisedUtf8();
			StringUtils::simplify(a_name);
			StringUtils::simplify(b_name);
			return alphanum_less(a_name, b_name);
		});
	}

	bool CommandListFolder::shouldDelete(Command* cmd) const
	{
		if (isSubFolder(cmd))
		{
			auto* list = cmd->as<CommandList>();
			for (auto i = list->children.begin(); i != list->children.end(); )
			{
				processChild(list, i);
			}
			return list->children.empty();
		}
		return !soup::filesystem::exists_case_sensitive(getFilePath(getItemData(cmd)));
	}

	bool CommandListFolder::canShowFolder(const std::wstring& folder_rel_path) const
	{
		return true;
	}
}
