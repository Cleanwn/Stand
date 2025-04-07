#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandGallery : public CommandList
	{
	public:
		explicit CommandGallery(CommandList* const parent);	
	};
}
