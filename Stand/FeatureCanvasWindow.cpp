#include "FeatureCanvasWindow.hpp"

#include <soup/ObfusString.hpp>
#include <soup/Rgb.hpp>
#include <soup/RenderTarget.hpp>

#include "CommandPhysical.hpp" // HANDLER_CHECK, HANDLER_END
#include "Exceptional.hpp"
#include "regular_event.hpp"
#include "Renderer.hpp"
#include "Util.hpp"

namespace Stand
{
	struct FeatureCanvasWindowCustomData
	{
		FeatureCanvas::draw_t draw_func;
		void* userdata;
	};

	void FeatureCanvasWindow::init(int width, int height, draw_t draw_func, void* userdata)
	{
		Exceptional::createManagedExceptionalThread(__FUNCTION__, [=]
		{
			g_renderer.stop_input();

			w = soup::Window::create(soup::ObfusString("A Stand Gadget"), width, height);
			w->customData() = FeatureCanvasWindowCustomData{ draw_func, userdata };
			w->setResizable(true);
			w->setDrawFunc([](soup::Window w, soup::RenderTarget& rt)
			{
				rt.fill(soup::Rgb::BLACK);

				auto& cd = w.customData().get<FeatureCanvasWindowCustomData>();
				cd.draw_func(cd.userdata, rt);
			});
			w->onClose([](soup::Window)
			{
				Util::toast(soup::ObfusString("Disable the respective feature if you don't want this window."));
			});

			reScriptTickEvent::registerHandler([this]
			{
				HANDLER_CHECK(this->w.has_value());
				this->w->redraw();
				HANDLER_END;
			}, soup::ObfusString("A Stand Gadget"));

			w->runMessageLoop();
		});
	}

	void FeatureCanvasWindow::shutdown()
	{
		if (w.has_value())
		{
			w->close();
			w.reset();
		}
	}
}
