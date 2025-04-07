#include "CommandEditLabels.hpp"

#include "CommandReadonlyLink.hpp"
#include "Hooking.hpp"
#include "Label.hpp"

#include "CommandAddLabel.hpp"
#include "CommandLabel.hpp"
#include "CommandRevealLabels.hpp"

namespace Stand
{
	CommandEditLabels::CommandEditLabels(CommandList* const parent)
		: CommandListUserkv(parent, LOC("EDTLB"), CMDNAMES("editlabels", "labeleditor"))
	{
	}

	uint32_t CommandEditLabels::keyToHash(const std::string& key)
	{
		uint32_t hash = rage::atStringHash(key);
		if (key.substr(0, 2) == "0X")
		{
			try
			{
				hash = (uint32_t)std::stoull(key.c_str() + 2, nullptr, 16);
			}
			catch (const std::exception&)
			{
			}
		}
		else
		{
			if (!g_hooking.doesTextLabelExist(hash))
			{
				try
				{
					hash = (uint32_t)std::stoull(key, nullptr, 10);
				}
				catch (const std::exception&)
				{
				}
			}
		}
		return hash;
	}

	void CommandEditLabels::populateHead()
	{
		this->createChild<CommandAddLabel>();
		this->createChild<CommandRevealLabels>();
		this->createChild<CommandTogglePointer>(&g_hooking.owo_labels, LOC("OWOLB"), CMDNAMES("owolabels"));
		this->createChild<CommandReadonlyLink>(LOC("LBLST"), soup::ObfusString("https://gist.github.com/aaronlink127/afc889be7d52146a76bab72ede0512c7/raw"));
	}

	std::vector<std::pair<std::string, CommandPhysical*>>::iterator CommandEditLabels::find(const std::string& key)
	{
		uint32_t hash = keyToHash(key);

		auto i = entries.begin();
		while (i != entries.end() && keyToHash(i->first) != hash)
		{
			++i;
		}
		return i;
	}

	CommandPhysical* CommandEditLabels::emplaceItem(const std::string& key, std::string&& value)
	{
		return createChild<CommandLabel>(key, std::move(value));
	}

	void CommandEditLabels::focusItem(ThreadContext thread_context, CommandPhysical* item) const
	{
		item->as<CommandLabel>()->open(thread_context);
	}

	std::string CommandEditLabels::getValue(const std::string& key, CommandPhysical* item) const
	{
		return g_hooking.label_replacements.at(keyToHash(key));
	}

	void CommandEditLabels::onRemoveItem(CommandPhysical* item)
	{
		g_hooking.label_replacements.erase(item->as<CommandLabel>()->label_hash);
	}
}
