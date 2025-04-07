#include "CommandPlayerShowcam.hpp"

#include "CommandPlayer.hpp"
#include "DrawUtil3d.hpp"
#include "tbScreenshotMode.hpp"

namespace Stand
{
	CommandPlayerShowcam::CommandPlayerShowcam(CommandList* const parent)
		: CommandPlayerToggleBatch(parent, LOC("SHWCAM"), CMDNAMES_OBF("showcam"))
	{
	}

	void CommandPlayerShowcam::onTick(const std::vector<AbstractPlayer>& players) const
	{
		if (!g_tb_screenshot_mode.isEnabled())
		{
			for (const auto& p : players)
			{
				v3 cam_pos = p.getCamPos();
				DrawUtil3d::draw_line(cam_pos, p.getPos());
				DrawUtil3d::draw_camera(cam_pos, p.getCamRot());
			}
		}
	}
}
