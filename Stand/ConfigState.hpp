#pragma once

#include "ConfigTreeSimple.hpp"

namespace Stand
{
	class ConfigState : public ConfigTreeSimple
	{
	public:
		using ConfigTreeSimple::ConfigTreeSimple;

	protected:
		[[nodiscard]] bool getCommandValue(const CommandPhysical* command, const std::string& path, std::string& outValue) const final;
	};
}
