#include "Worker.hpp"

#include <soup/os.hpp>

#include "AbstractPlayer.hpp"
#include "Exceptional.hpp"
#include "ExecCtx.hpp"
#include "Gui.hpp"
#include "PlayerHistory.hpp"
#include "pointers.hpp"
#include "regular_event.hpp"

namespace Stand
{
	void Worker::thread_func()
	{
		THREAD_NAME("Worker");
		ExecCtx::get().tc = TC_WORKER;

		while (g_gui.unload_state < UnloadState::HAND_OVER_TO_MAIN)
		{
			__try
			{
				if (*pointers::CLoadingScreens_ms_Context == 0)
				{
					AbstractPlayer::doWork();
					if (root_update_mtx.tryLock())
					{
						__try
						{
							if (g_gui.doesRootStateAllowPlayerList())
							{
								g_gui.managePlayerList(TC_WORKER);
								if (PlayerHistory::needs_to_be_saved)
								{
									PlayerHistory::saveNow();
								}
							}
						}
						__EXCEPTIONAL()
						{
						}
						root_update_mtx.unlock();
					}
				}

				std::queue<std::function<void()>> jobs;
				EXCEPTIONAL_LOCK(jobs_mtx)
				jobs = std::move(next_jobs);
				EXCEPTIONAL_UNLOCK(jobs_mtx)
				while (!jobs.empty())
				{
					auto job = std::move(jobs.front());
					jobs.pop();
					__try
					{
						job();
					}
					__EXCEPTIONAL()
					{
					}
				}

				soup::os::sleep(30);
			}
			__EXCEPTIONAL()
			{
			}
		}
		g_gui.worker_thread_running = false;
	}

	void Worker::queueJob(std::function<void()>&& func)
	{
		EXCEPTIONAL_LOCK(jobs_mtx)
		next_jobs.emplace(std::move(func));
		EXCEPTIONAL_UNLOCK(jobs_mtx)
	}
}
