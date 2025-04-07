#include "Box.hpp"

#include "AbstractRenderer.hpp"
#include "DrawUtil3d.hpp"

namespace Stand
{
	void Box::drawLines(int r, int g, int b, int a) const
	{
		DrawUtil3d::draw_line(back_lower_left, edge2, r, g, b, a);
		DrawUtil3d::draw_line(back_lower_left, edge4, r, g, b, a);
		DrawUtil3d::draw_line(edge2, edge3, r, g, b, a);
		DrawUtil3d::draw_line(edge3, edge4, r, g, b, a);
		DrawUtil3d::draw_line(front_upper_right, edge6, r, g, b, a);
		DrawUtil3d::draw_line(front_upper_right, edge8, r, g, b, a);
		DrawUtil3d::draw_line(edge6, edge7, r, g, b, a);
		DrawUtil3d::draw_line(edge7, edge8, r, g, b, a);
		DrawUtil3d::draw_line(back_lower_left, edge7, r, g, b, a);
		DrawUtil3d::draw_line(edge2, edge8, r, g, b, a);
		DrawUtil3d::draw_line(edge3, front_upper_right, r, g, b, a);
		DrawUtil3d::draw_line(edge4, edge6, r, g, b, a);
	}

	void Box::drawLines(const AbstractRenderer& renderer, DirectX::SimpleMath::Color& colour) const
	{
		renderer.drawLine(back_lower_left, edge2, colour);
		renderer.drawLine(back_lower_left, edge4, colour);
		renderer.drawLine(edge2, edge3, colour);
		renderer.drawLine(edge3, edge4, colour);
		renderer.drawLine(front_upper_right, edge6, colour);
		renderer.drawLine(front_upper_right, edge8, colour);
		renderer.drawLine(edge6, edge7, colour);
		renderer.drawLine(edge7, edge8, colour);
		renderer.drawLine(back_lower_left, edge7, colour);
		renderer.drawLine(edge2, edge8, colour);
		renderer.drawLine(edge3, front_upper_right, colour);
		renderer.drawLine(edge4, edge6, colour);
	}
}
