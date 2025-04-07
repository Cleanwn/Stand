#pragma once

#include "CommandAction.hpp"

#include "FiberPool.hpp"
#include "SessionScriptMgr.hpp"

namespace Stand
{
	class CommandStartSessionScript : public CommandAction
	{
	private:
		const hash_t script_hash;

	public:
		inline static std::unordered_set<hash_t> categorised{};

		explicit CommandStartSessionScript(CommandList* const parent, const hash_t script_hash, const commandflags_t flags = 0)
			: CommandAction(parent, rage::scrThread::getLabel(script_hash), {}, NOLABEL, flags), script_hash(script_hash)
		{
			//menu_name.literal_str.append(fmt::format(" ({})", SessionScriptMgr::hash_to_id(script_hash)));
			/*if (CommandListSessionScripts::hash_to_id(script_hash) == 0)
			{
				Util::toast(fmt::format("{} not launchable", menu_name.getEnglishUtf8()), TC_SCRIPT_NOYIELD);
			}*/
			categorised.emplace(script_hash);
		}

		void onClick(Click& click) final
		{
			if (!SessionScriptMgr::startPrecheck(click))
			{
				return;
			}
			/*static bool _debug = false;
			if (!_debug)
			{
				_debug = true;
				FiberPool::queueJob([]
				{
					while (true)
					{
						drawDebugText(script_global(GLOBAL_AM_LAUNCHER_HOST_DATA + 2).get<int>());
						if (GtaThread::fromHash(ATSTRINGHASH("am_launcher")))
						{
							for (auto& p : AbstractPlayer::listExcept())
							{
								drawDebugText(*script_local(GtaThread::fromHash(ATSTRINGHASH("am_launcher")), 128).at(p, 3).at(2).as<int*>());
								//drawDebugText(*script_local(GtaThread::fromHash(ATSTRINGHASH("am_launcher")), 128).at(p, 3).at(1).as<int*>() & (1 << 2));
							}
						}
						script::get_current()->yield();
					}
				});
			}*/
			FiberPool::queueJob([this]
			{
				SessionScriptMgr::start(script_hash);
			});
		}
	};
}
