#include "CommandPersonalVehicles.hpp"

#include "CommandLambdaAction.hpp"
#include "CommandPersonalVehicle.hpp"
#include "CommandSearchLiterals.hpp"
#include "CommandPersonalVehiclesSort.hpp"
#include "CommandSaveAllPersonalVehicles.hpp"
#include "Gui.hpp"
#include "PersonalVehicle.hpp"
#include "Script.hpp"
#include "ScriptGlobal.hpp"

namespace Stand
{
	CommandPersonalVehicles::CommandPersonalVehicles(CommandList* parent)
		: CommandListRefreshable(parent, LOC("PVS"), CMDNAMES("pvs"))
	{
		instance = this;
	}

	CommandPersonalVehicles::~CommandPersonalVehicles()
	{
		if (instance == this)
		{
			instance = nullptr;
		}
	}

	void CommandPersonalVehicles::onActiveListUpdate()
	{
		if (isThisOrSublistCurrentMenuListInTab())
		{
			if (!is_active
				&& isCurrentMenuListInTab()
				)
			{
				is_active = true;
				refresh();
			}
		}
		else
		{
			is_active = false;
		}
	}

	void CommandPersonalVehicles::onClick(Click& click)
	{
		if (click.inOnline())
		{
			CommandListRefreshable::onClick(click);
		}
		else
		{
			onClickImplUnavailable(click);
		}
	}

	void CommandPersonalVehicles::forEachPersonalVehicle(const std::function<void(uint32_t, ScriptGlobal&&)>& consumer)
	{
		return ScriptGlobal(GLOBAL_PVS).iterate(GLOBAL_PVS_ELMSIZE, consumer);
	}

	void CommandPersonalVehicles::populateHead()
	{
		createChild<CommandSearchLiterals<false>>({ CMDNAME("findpv") });
		sort_algo_select = createChild<CommandPersonalVehiclesSort>();
		createChild<CommandSaveAllPersonalVehicles>();

#if false
		// All
		{
			auto all = createChild<CommandList>(LOC("ALL"));

			all->createChild<CommandLambdaAction>(LOC("INSURCLM"), { CMDNAME("claimallpvs") }, NOLABEL, [this](Click& click)
			{
				forEachPersonalVehicle([](uint32_t i, ScriptGlobal&& data)
				{
					PersonalVehicle{ i }.setInsuranceFlag(GLOBAL_PVS_INSURANCE_DESTROYED, false);
				});
				PersonalVehicle::saveStats();
				this->refresh();
			});
			all->createChild<CommandLambdaAction>(LOC("ADDINSUR"), { CMDNAME("insureallpvs") }, NOLABEL, [this](Click& click)
			{
				forEachPersonalVehicle([](uint32_t i, ScriptGlobal&& data)
				{
					PersonalVehicle{ i }.setInsuranceFlag(GLOBAL_PVS_INSURANCE_INSURED, true);
				});
				PersonalVehicle::saveStats();
				this->refresh();
			});
		}
#endif
	}

	void CommandPersonalVehicles::populateBody()
	{
		if (g_gui.user_was_in_online)
		{
			forEachPersonalVehicle([this](int i, ScriptGlobal&& data)
			{
				auto model = data.at(GLOBAL_PVS_MODEL).get<Hash>();
				if (model == 0)
				{
					return;
				}
				createChild<CommandPersonalVehicle>(i, model);
			});
		}
	}

	void CommandPersonalVehicles::sortImpl()
	{
		return sortBody(reinterpret_cast<CommandList::sort_algo_t>(sort_algo_select->value));
	}

	bool CommandPersonalVehicles::sortAlgoSlot(const std::unique_ptr<Command>& a, const std::unique_ptr<Command>& b)
	{
		return ((CommandPersonalVehicle*)a.get())->i < ((CommandPersonalVehicle*)b.get())->i;
	}
}
