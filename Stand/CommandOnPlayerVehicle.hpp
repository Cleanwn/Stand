#pragma once

#include "CommandOnPlayer.hpp"

#include "AbstractEntity.hpp"
#include "AbstractPlayer.hpp"
#include "PlayerProvider.hpp"
#include "CommandListPlayer.hpp"

#define PV_CONTROL		0b1
#define PV_ALLOWSELF	0b10

namespace Stand
{
	template <class T>
	class CommandOnPlayerVehicle : public CommandOnPlayer<T>
	{
	public:
		uint8_t pv_flags = 0;

		using Base = CommandOnPlayer<T>;

		using Base::Base;

		void onClick(Click& click)
		{
			if (PP_PTR->isUser()
				&& !(pv_flags & PV_ALLOWSELF)
				)
			{
				click.uwotm8();
				return;
			}
			click.setNoResponse();
			click.ensureYieldableScriptThread([this, click]
			{
				for (const auto& ap : PP_PTR->getPlayers())
				{
					ap.openThirdEye([this, click](const AbstractPlayer& p, AbstractEntity& ped, bool needs_cleanup)
					{
						auto veh = ped.getVehicle(false);
						if (!veh.isValid())
						{
							click.setResponse(LIT(LANG_FMT("PLYNVEH", p.getName())));
							p.openThirdEyeCleanup(needs_cleanup);
							return;
						}
						if (!(pv_flags & PV_CONTROL))
						{
							onClickImpl(p, veh, click);
							p.openThirdEyeCleanup(needs_cleanup);
							return;
						}
						veh.requestControl(ATSTRINGHASH("CommandOnPlayerVehicle"), [this, click, p, needs_cleanup](AbstractEntity& veh)
						{
							onClickImpl(p, veh, click);
							p.openThirdEyeCleanup(needs_cleanup);
						}, [p, click, needs_cleanup](AbstractEntity& veh)
						{
							click.setResponse(LIT(LANG_FMT("PLYVEHNCTRL", p.getName())));
							p.openThirdEyeCleanup(needs_cleanup);
						});
					});
				}
			});
		}

	protected:
		virtual void onClickImpl(AbstractPlayer p, AbstractEntity& veh, const Click& click) const = 0;
	};
}
