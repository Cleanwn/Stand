#pragma once

#include "CommandList.hpp"

#include "vector.hpp"

namespace Stand
{
	// Not to be used directly. Use an implementer such as CommandPosition3d so ::populate is available!
	class CommandVector3 : public CommandList
	{
	protected:
		using CommandList::CommandList;

	public:
		[[nodiscard]] virtual v3 getVec() = 0;
		virtual void setVec(v3&& vec) = 0;
	};
}
