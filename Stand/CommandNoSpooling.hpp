#include "CommandToggle.hpp"

#include "WeaponMgr.hpp"

namespace Stand
{
	class CommandNoSpooling : public CommandToggle
	{
	public:
		explicit CommandNoSpooling(CommandList* const parent)
			: CommandToggle(parent, LOC("NOSPOL"), { CMDNAME("nospooling") }, LOC("NOSPOL_H"))
		{
		}

		void onChange(Click& click) final
		{
			WeaponMgr::weapon_spool_delay_multiplier = this->m_on ? 0.0f : 1.0f;
		}
	};
}