#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "CommandListLockWeapons.hpp"
#include "WeaponLoadout.hpp"

namespace Stand
{
	class CommandToggleLockWeapons : public CommandToggle
	{
	public:
		WeaponLoadout saved_loadout{};

		explicit CommandToggleLockWeapons(CommandList* const parent)
			: CommandToggle(parent, LOC("WEAPLOCK"), CMDNAMES_OBF("lockweapons"))
		{
		}

		[[nodiscard]] CommandListLockWeapons* getHost() const noexcept
		{
			return parent->as<CommandListLockWeapons>();
		}

		void onChange(Click& click) final
		{
			ensureScriptThread(click, [this]
			{
				if (m_on)
				{
					saveLoadout();

					registerScriptTickEventHandlerInContext([this]
					{
						HANDLER_CHECK(m_on);
						getHost()->onTickEnabled();
						HANDLER_END;
					});
				}
				else
				{
					restoreLoadout();
				}
			});
		}

		void saveLoadout()
		{
			saved_loadout = WeaponLoadout::fromPed(g_player_ped);
		}

		void restoreLoadout(bool clear = true)
		{
			saved_loadout.apply(g_player_ped);

			if (clear)
			{
				saved_loadout.clear();
			}
		}
	};
}