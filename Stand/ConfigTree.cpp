#include "ConfigTree.hpp"

#include <soup/cat.hpp>

#include "CommandLuaScript.hpp"
#include "Exceptional.hpp"
#include "Gui.hpp"
#include "Util.hpp"

namespace Stand
{
	void ConfigTree::loadImpl(std::unordered_map<std::string, std::string>&& data)
	{
		const bool need_save = Util::upgrade_menu_names_from_colons_and_tabs_file(data);
		loadImplTree(std::move(data));
		if (need_save)
		{
			save();
		}
	}

	bool ConfigTree::canSaveNow() const
	{
		return g_gui.isRootStateFull()
			&& !g_gui.root_update_in_progress
			&& !CommandLuaScript::hold_up_save
			;
	}

	void ConfigTree::saveToFileNow()
	{
		if (!canSaveNow() || g_gui.unload_state > UnloadState::SAVE_PENDING_CONFIGS)
		{
			return;
		}
		auto buf = std::string();
		EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
		buf.append("Tree Compatibility Version: ");
		buf.append(fmt::to_string(Util::menu_names_version));
		buf.push_back('\n');
		saveImpl(buf, {}, {}, g_gui.root_list.get(), &nullsub<>);
		EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
		std::ofstream file_out(getPath());
		file_out << std::move(buf);
		file_out.close();
	}

	void ConfigTree::saveImpl(std::string& buf, const std::string& path_prefix, const std::string& file_prefix, const CommandList* list, std::function<void()>&& first_entry_func) const
	{
		bool any_entry_in_list = false;
		for (const auto& _command : list->children)
		{
			if (!_command->isPhysical()
				|| !_command->canBeResolved()
				)
			{
				continue;
			}
			auto* command = (const CommandPhysical*)_command.get();
			if ((command->flags & CMDFLAG_STATE_AND_CHECK_FINISHLIST)
				&& (command->flags & CMDFLAG_HOTKEYS_FINISHLIST)
				)
			{
				break;
			}
			std::string name = command->getNameForConfig();
			std::string path = std::move(std::string(path_prefix).append(name));
			bool had_entry = false;
			if (std::string value; getCommandValue(command, path, value))
			{
				if (!any_entry_in_list)
				{
					first_entry_func();
					any_entry_in_list = true;
				}
				buf.append(file_prefix);
				{
					std::string name = command->getNameForConfig();
					soup::cat::encodeName(name);
					buf.append(std::move(name));
				}
				buf.append(": ");
				soup::cat::encodeValue(value);
				buf.append(std::move(value));
				buf.push_back('\n');
				had_entry = true;
			}
			if (command->isList())
			{
				path.push_back('>');
				std::string file_prefix_{ file_prefix };
				file_prefix_.push_back('\t');
				saveImpl(buf, path, std::move(file_prefix_), (const CommandList*)command, [&any_entry_in_list, first_entry_func, had_entry, &buf, file_prefix, name{ std::move(name) }]
				{
					if (!any_entry_in_list)
					{
						first_entry_func();
						any_entry_in_list = true;
					}
					if (!had_entry)
					{
						buf.append(file_prefix);
						buf.append(name);
						buf.push_back('\n');
					}
				});
			}
		}
	}
}
