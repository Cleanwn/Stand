#pragma once

#include "Grid.hpp"

namespace Stand
{
	class TutorialGrid : public Grid
	{
	public:
		TutorialGrid();

		void populate(std::vector<std::unique_ptr<GridItem>>& items_draft) final;
	};
	inline TutorialGrid g_tutorial_grid{};
}
