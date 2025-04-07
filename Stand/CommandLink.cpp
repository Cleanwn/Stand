#include "CommandLink.hpp"

#include <soup/WeakRef.hpp>

#include "CommandPhysical.hpp"
#ifdef STAND_DEBUG
#include "Util.hpp"
#endif

namespace Stand
{
	CommandLink::CommandLink(CommandList* parent, Command* target, bool show_address_in_corner)
		: CommandLink(parent, (CommandPhysical*)target, show_address_in_corner)
	{
	}

	CommandLink::CommandLink(CommandList* parent, CommandPhysical* target, bool show_address_in_corner)
		: Command(COMMAND_LINK, parent), target(target->getWeakRef()), show_address_in_corner(show_address_in_corner)
	{
#ifdef STAND_DEBUG
		if (!target->isPhysical())
		{
			Util::toast(std::move(getPathEn().append(" target is non-physical?!")), TOAST_ALL);
		}
#endif
	}

	CommandPhysical* CommandLink::getTarget() const noexcept
	{
		return target.getPointer()->as<CommandPhysical>();
	}
}
