#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListQrcode : public CommandList
	{
	public:
		using CommandList::CommandList;

		void onActiveListUpdate() final;

		[[nodiscard]] virtual std::string getText() const = 0;
	};
}
