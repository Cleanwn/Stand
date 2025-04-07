#pragma once

namespace Stand
{
	class CommandNoBeast : public CommandToggle
	{
	public:
		explicit CommandNoBeast(CommandList* const parent);

		void onChange(Click& click) final;
	};
}
