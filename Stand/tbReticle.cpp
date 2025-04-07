#include "tbReticle.hpp"

#include "regular_event.hpp"
#include "Renderer.hpp"

namespace Stand
{
	void tbReticle::onPresent()
	{
		Circle::inst_50.setRadius(8.0f / 1920.0f / 2.0f);
		g_renderer.drawHollowCircleS(0.5f, 0.5f, Circle::inst_50, DirectX::SimpleMath::Color{ 1.0f, 1.0f, 1.0f, 0.5f });
	}

	void tbReticle::onEnable()
	{
		rePresentEvent::registerHandler(&onPresent);
	}

	void tbReticle::onDisable()
	{
		rePresentEvent::unregisterHandler(&onPresent);
	}
}
