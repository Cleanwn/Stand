#include "CommandModelBlacklist.hpp"

#include "CommandModelBlacklistAdd.hpp"
#include "CommandModelBlacklistItem.hpp"
#include "Hooking.hpp"

namespace Stand
{
	CommandModelBlacklist::CommandModelBlacklist(CommandList* parent)
		: CommandListUserkv(parent, LOC("PTX_CSB"), {}, NOLABEL, "B12314E0<3,3>E5A2D6C6<3,3>0D7114C9<3,3>9B22DBAF<3,3>EFE5AFE6<3,3>BDBB4922<3,3>")
	{
	}

	void CommandModelBlacklist::populateHead()
	{
		this->createChild<CommandModelBlacklistAdd>();
	}

	CommandPhysical* CommandModelBlacklist::emplaceItem(const std::string& key, std::string&& value)
	{
		const hash_t hash = std::stoul(key, nullptr, 16);
		g_hooking.custom_model_sync_reactions.emplace(hash, FlowEventReactionData::fromString(value));
		return this->createChildWithBuilder<CommandModelBlacklistItem>(hash);
	}

	void CommandModelBlacklist::focusItem(ThreadContext thread_context, CommandPhysical* item) const
	{
		item->as<CommandModelBlacklistItem>()->open(thread_context);
	}

	std::string CommandModelBlacklist::getValue(const std::string& key, CommandPhysical* item) const
	{
		return g_hooking.custom_model_sync_reactions.at(std::stoul(key, nullptr, 16)).toString();
	}

	void CommandModelBlacklist::onRemoveItem(CommandPhysical* item)
	{
		g_hooking.custom_model_sync_reactions.erase(item->as<CommandModelBlacklistItem>()->hash);
	}
}
