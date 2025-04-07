#include "CommandPlayerClassifier.hpp"

#include "CommandPlayerClassifierDetection.hpp"
#include "lang.hpp"

namespace Stand
{
	CommandPlayerClassifier::CommandPlayerClassifier(CommandList* const parent)
		: CommandList(parent, LIT(std::move(std::string(LANG_GET("CLSFN")).append(": ").append(LANG_GET("NONE")))), {}, LOC("CLSFN_N_H"), CMDFLAGS_LIST | CMDFLAG_TEMPORARY | CMDFLAG_FEATURELIST_SKIP, COMMAND_LIST_CUSTOM_SPECIAL_MEANING)
	{
		indicator_type = LISTINDICATOR_ARROW_IF_CHILDREN;
	}

	void CommandPlayerClassifier::onClick(Click& click)
	{
		if (!children.empty())
		{
			CommandList::onClick(click);
		}
	}

	CommandPlayerClassifierDetection* CommandPlayerClassifier::getDetection(const flowevent_t type) const
	{
		for (const auto& child : children)
		{
			if (child->as<CommandPlayerClassifierDetection>()->event_type == type)
			{
				return child->as<CommandPlayerClassifierDetection>();
			}
		}
		return nullptr;
	}
}
