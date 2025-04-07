#include "CommandGeoGuessr.hpp"

#include "Camgr.hpp"
#include "get_current_time_millis.hpp"
#include "tbScreenshotMode.hpp"

#include "CommandGeoGuessrMain.hpp"
#include "CommandGeoGuessrScout.hpp"
#include "CommandGeoGuessrSubmit.hpp"

namespace Stand
{
	CommandGeoGuessr::CommandGeoGuessr(CommandList* const parent)
		: CommandListConcealer(parent, LOC("GGSR"), {}, LOC("GGSR_H"))
	{
		this->createChild<CommandGeoGuessrMain>();
		scout_toggle = this->createChild<CommandGeoGuessrScout>();
		submit_action = this->createChild<CommandGeoGuessrSubmit>();
	}

	void CommandGeoGuessr::startScouting()
	{
		Camgr::startCamCommand();

		scouting = true;
		scout_toggle->setOnIndication(true);

		Camgr::user_fov_allowed = false;
		g_tb_screenshot_mode.enable();
	}

	void CommandGeoGuessr::stopScouting()
	{
		if (scouting)
		{
			scouting = false;
			scout_toggle->setOnIndication(false);

			cleanupCam();
			Camgr::user_fov_allowed = true;
			g_tb_screenshot_mode.disable();
		}
	}

	void CommandGeoGuessr::updateGameCommandsVisiblity(const bool toggle)
	{
		setVisible(scout_toggle, toggle);
		setVisible(submit_action, toggle);
	}

	void CommandGeoGuessr::cleanup()
	{
		updateGameCommandsVisiblity(false);

		stopScouting();

		if (guessed_at != 0)
		{
			guessed_at = 0;
			cleanupGuess();
		}
	}

	void CommandGeoGuessr::cleanupCam()
	{
		Camgr::unlock_focus();
		Camgr::unlock_pos();
		Camgr::unlock_rot();
		Camgr::unlock_fov();
		Camgr::endCamCommand();
	}

	void CommandGeoGuessr::cleanupGuess()
	{
		cleanupCam();
		DrawUtil3d::exclusive_ar = false;
		g_tb_screenshot_mode.disable();
		g_tb_screenshot_mode.allow_centred_text = false;
	}
}
