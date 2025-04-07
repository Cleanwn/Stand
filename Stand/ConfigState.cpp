#include "ConfigState.hpp"

namespace Stand
{
	bool ConfigState::getCommandValue(const CommandPhysical* command, const std::string& path, std::string& outValue) const
	{
		if (ConfigTreeSimple::getCommandValue(command, path, outValue))
		{
			if (outValue != command->getDefaultState())
			{
				return true;
			}
		}
		return false;
	}
}
