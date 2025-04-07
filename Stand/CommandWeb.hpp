#pragma once

#include "CommandListQrcode.hpp"

namespace Stand
{
	class CommandWeb : public CommandListQrcode
	{
	public:
		explicit CommandWeb(CommandList* const parent);

		[[nodiscard]] std::string getText() const final;

		[[nodiscard]] static std::string getUri();
	};
}
