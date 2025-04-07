#include "ConsoleLogger.hpp"

#include <Windows.h>

#include <soup/console.hpp>
#include <soup/ObfusString.hpp>

#include "conf.hpp"
#include "Exceptional.hpp"
#include "Renderer.hpp"
#include "ThreadContext.hpp"
#include "Util.hpp"

namespace Stand
{
	ConsoleLogger::ConsoleLogger()
		: Logger()
	{
		ignore_repetitions = true;
	}

	bool ConsoleLogger::isInited() const
	{
		return out && out.is_open();
	}

	bool ConsoleLogger::gameHasAnyConsole()
	{
		return AttachConsole(GetCurrentProcessId());
	}

	void ConsoleLogger::init()
	{
		g_renderer.stop_input();
		AllocConsole();
		if (GetStdHandle(STD_OUTPUT_HANDLE) != nullptr)
		{
			// Disable "X" button
			EnableMenuItem(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

			// Prevent Ctrl+C terminating the program
			soup::console.overrideCtrlC([] {});

			SetConsoleTitleA(soup::ObfusString(STAND_NAMEVERSION).c_str());
			SetConsoleOutputCP(CP_UTF8);
			out.open("CONOUT$", std::ios_base::out | std::ios_base::app);

			enterBlockMode();
			log_no_nl(
				"           *******                                           **\n"
				"          *       ***      *                                   **\n"
				"         *         **     **                                   **\n"
				"         **        *      **                                   **\n"
				"          ***           ********                               **\n"
				"         ** ***        ********     ****    ***  ****      *** **\n"
				"          *** ***         **       * ***  *  **** **** *  *********\n"
				"            *** ***       **      *   ****    **   ****  **   ****\n"
				"              *** ***     **     **    **     **    **   **    **\n"
				"                ** ***    **     **    **     **    **   **    **\n"
				"                 ** **    **     **    **     **    **   **    **\n"
				"                  * *     **     **    **     **    **   **    **\n"
				"        ***        *      **     **    **     **    **   **    **\n"
				"       *  *********        **     ***** **    ***   ***   *****\n"
				"      *     *****                  ***   **    ***   ***   ***\n"
				"      *\n"
				"       **\n");
			leaveBlockMode();
		}
		else
		{
			Util::toast("Failed to allocate console.", TOAST_ALL);
		}
	}

	void ConsoleLogger::deinit()
	{
		FreeConsole();
		out.close();
	}

	struct ConsoleWriteTask : public soup::Task
	{
		std::string message;

		ConsoleWriteTask(std::string&& message)
			: message(std::move(message))
		{
		}

		void onTick() final
		{
			g_console.out << std::move(message) << std::flush;
			setWorkDone();
		}
	};

	void ConsoleLogger::write(std::string&& message)
	{
		if (isInited())
		{
			// Moving into DetachedScheduler so an active selection in the console won't block the caller.
			EXCEPTIONAL_LOCK(sched_mtx)
			sched.add<ConsoleWriteTask>(std::move(message));
			EXCEPTIONAL_UNLOCK(sched_mtx)
		}
	}
}
