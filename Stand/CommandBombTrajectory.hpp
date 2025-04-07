#pragma once

namespace Stand
{
	class CommandBombTrajectory : public CommandToggle
	{
	public:
		explicit CommandBombTrajectory(CommandList* const parent);

		void onChange(Click& click);
	};
}
