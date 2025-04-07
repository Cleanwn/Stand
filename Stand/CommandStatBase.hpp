#pragma once

#include "CommandInputText.hpp"
#include "CommandSlider.hpp"
#include "CommandSliderFloat.hpp"

#include "ExecCtx.hpp"
#include "FiberPool.hpp"
#include "Script.hpp"
#include "Stats.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	static inline bool stats_loaded = false;

	template <class Base, typename ValueType>
	class CommandStatBase : public Base
	{
	private:
		Label old_help_tx = NOLABEL;
		bool busy_loading = false;
		bool did_focus_update = false;

	public:
		Stat& stat;

		explicit CommandStatBase(CommandList* const parent, Stat& stat, int min_value, const int max_value) requires std::is_base_of_v<CommandSlider, CommandStatBase>
			: Base(parent, stat.getLabel(), { utf8ToCmdName(StringUtils::simplify(stat.name)) }, NOLABEL, min_value, max_value, 0, 1, (CMDFLAGS_SLIDER & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS) | CMDFLAG_STATE_AND_CHECK_FINISHLIST | CMDFLAG_FEATURELIST_FINISHLIST), stat(stat)
		{
		}

		explicit CommandStatBase(CommandList* const parent, Stat& stat) requires std::is_base_of_v<CommandInput, CommandStatBase>
			: Base(parent, stat.getLabel(), { utf8ToCmdName(StringUtils::simplify(stat.name)) }, NOLABEL, {}, (CMDFLAGS_ACTION_VALUE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS) | CMDFLAG_STATE_AND_CHECK_FINISHLIST | CMDFLAG_FEATURELIST_FINISHLIST), stat(stat)
		{
			if (stat.type == Stats::LABEL)
			{
				Base::setHelpText(LOC("STAT_STR_LABEL"));
			}
			else if (stat.type == Stats::USERID || stat.type == Stats::PLAYERID)
			{
				Base::setHelpText(LOC("STAT_STR_RID"));
			}
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (!stats_loaded)
			{
				FiberPool::queueJob([=]() mutable
				{
					while (!isLoaded())
					{
						Script::current()->yield();
					}

					onCommandInner(click, args);
				});
			}
			else
			{
				onCommandInner(click, args);
			}

			args.clear();
		}

		void onTickInGameViewport() final
		{
			if (ExecCtx::get().isScript() 
				&& (!Base::isFocused() || !did_focus_update)
				)
			{
				if (stats_loaded)
				{
					did_focus_update = true;
				}

				ValueType value = {};
				getStat(getStatHash(), &value);
				onUpdate(false);

				if constexpr (std::is_base_of_v<CommandSlider, CommandStatBase>)
				{
					Click click(CLICK_BULK, TC_SCRIPT_NOYIELD);
					Base::setValueIndicator(click, value);
				}
				else if constexpr (std::is_base_of_v<CommandInput, CommandStatBase>)
				{
					Base::setValue(std::move(value));
					Base::update_state(CLICK_BULK);
				}
				else
				{
					SOUP_ASSERT(false);
				}
			}
		}

		void onTickInWebViewport() final
		{
			return Base::onTickInWebViewportImplRedirect();
		}

		[[nodiscard]] Hash getStatHash() const
		{
			return rage::atStringHash(stat.name);
		}

		void onUpdate(bool save_stats = true)
		{
			if (!isLoaded())
			{
				return;
			}

			if (Base::value != Base::default_value)
			{
				if (save_stats && !STATS::STAT_SAVE_PENDING_OR_REQUESTED())
				{
					STATS::STAT_SAVE(0, 0, 3, 0); // Some stats don't require this, some do. This also increments MPX_MANUAL_SAVED twice, not a bug.
				}

				Base::default_value = Base::value;
			}
		}

		[[nodiscard]] bool isLoaded()
		{
			SOUP_ASSERT(ExecCtx::get().isScript());

			if (!STATS::STAT_LOAD_PENDING(0) && !STATS::STAT_SLOT_IS_LOADED(0))
			{
				STATS::STAT_LOAD(0);

				if (!busy_loading)
				{
					old_help_tx = Base::help_text;
					Base::setHelpText(LOC("STAT_WAIT"));
				}

				busy_loading = true;
			}

			stats_loaded = STATS::STAT_SLOT_IS_LOADED(0);

			if (stats_loaded)
			{
				busy_loading = false;
				Base::setHelpText(old_help_tx);
			}

			return stats_loaded;
		}

	protected:
		virtual void setStat(Hash stat) = 0;
		virtual void getStat(Hash stat, ValueType* value) = 0;

		virtual void onCommandInner(Click& click, std::wstring& args)
		{
			return Base::onCommand(click, args);
		}
	};
}