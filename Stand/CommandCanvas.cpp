#include "CommandCanvas.hpp"

#include "FeatureCanvasDirect.hpp"
#include "FeatureCanvasWindow.hpp"

namespace Stand
{
	CommandCanvas::CommandCanvas(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text)
		: CommandListSelect(parent, std::move(menu_name), std::move(command_names), std::move(help_text), {
			{0, LOC("DOFF")},
			{1, LIT("Window")},
			{2, LIT("Direct")},
		}, 0)
	{
	}

	void CommandCanvas::onChange(Click& click, long long prev_value)
	{
		if (prev_value == 1)
		{
			canvas_window->shutdown();
		}
		else if (prev_value == 2)
		{
			canvas_direct->shutdown();
		}

		if (value == 1)
		{
			if (!canvas_window)
			{
				canvas_window = soup::make_unique<FeatureCanvasWindow>();
			}
			init(*canvas_window);
		}
		else if (value == 2)
		{
			if (!canvas_direct)
			{
				canvas_direct = soup::make_unique<FeatureCanvasDirect>();
			}
			init(*canvas_direct);
		}
	}

	void CommandCanvas::init(FeatureCanvas& canvas)
	{
		canvas.init(800, 300, &drawFunc, this);
	}

	void CommandCanvas::drawFunc(void* userdata, soup::RenderTarget& rt)
	{
		reinterpret_cast<CommandCanvas*>(userdata)->draw(rt);
	}
}
