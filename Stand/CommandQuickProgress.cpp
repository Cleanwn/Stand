#include "CommandQuickProgress.hpp"

#include <soup/ObfusString.hpp>

#include "CommandLambdaAction.hpp"
#include "CommandLambdaActionScript.hpp"
#include "CommandListActionItem.hpp"
#include "CommandListActionLambda.hpp"
#include "CommandReadonlyLink.hpp"

#include "CommandListUnlocks.hpp"
#include "CommandListStatEditor.hpp"

#include "CommandSetCarClubRepLevel.hpp"

#include "CommandCasinoWheel.hpp"
#include "CommandCasinoSlots.hpp"
#include "CommandCasinoBlackjack.hpp"
#include "CommandCasinoRoulette.hpp"

#include "CommandFinishDoomsdayHeistSetups.hpp"
#include "CommandFinishApartmentHeistSetups.hpp"
#include "CommandSetRank.hpp"
#include "CommandClubPop.hpp"
#include "CommandClubPopLock.hpp"

namespace Stand
{
	CommandQuickProgress::CommandQuickProgress(CommandList* const parent)
		: CommandList(parent, LOC("QWQPROG"), {}, LOC("QWQPROG_H"))
	{
		this->createChild<CommandListUnlocks>();
		this->createChild<CommandListStatEditor>();

		// LS Car Meet
		{
			auto ls_car_meet = this->createChild<CommandList>(LOC("LSCM"));

			ls_car_meet->createChild<CommandSetCarClubRepLevel>();

			ls_car_meet->createChild<CommandLambdaActionScript>(LOC("UNLKPRIZRID"), CMDNAMES("unlockprizeride"), NOLABEL, [](const Click& click)
			{
				const auto char_slot = Util::getCharSlot(click);
				if (char_slot != -1)
				{
					STATS::STAT_SET_BOOL(CHAR_STAT(char_slot, "CARMEET_PV_CHLLGE_CMPLT"), true, true);
					click.setResponse(LOC("UNLKPRIZRID_T"));
				}
			});
		}

		// Cayo Perico Heist
		{
			auto cayo = this->createChild<CommandList>(LOC("H4"), { CMDNAME("cayoheist"), CMDNAME("cayopericoheist") });

			cayo->createChild<CommandLambdaActionScript>(LOC("H_FNSH"), { CMDNAME("setupcayoheist"), CMDNAME("setupcayopericoheist") }, NOLABEL, [](const Click& click)
			{
				const auto char_slot = Util::getCharSlot(click);
				if (char_slot != -1)
				{
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4CNF_BS_GEN"), 131071, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4CNF_WEAPONS"), 1, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4CNF_WEP_DISRP"), 3, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4CNF_ARM_DISRP"), 3, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4CNF_HEL_DISRP"), 3, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4CNF_BOLTCUT"), 4424, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4CNF_UNIFORM"), 5256, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4CNF_GRAPPEL"), 5156, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4CNF_TROJAN"), 1, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4LOOT_CASH_I"), 0, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4LOOT_CASH_I_SCOPED"), 0, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4LOOT_CASH_C"), 0, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4LOOT_CASH_C_SCOPED"), 0, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4LOOT_COKE_I"), 0, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4LOOT_COKE_I_SCOPED"), 0, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4LOOT_COKE_C"), 0, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4LOOT_COKE_C_SCOPED"), 0, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4LOOT_GOLD_I"), 16777215, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4LOOT_GOLD_I_SCOPED"), 16777215, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4LOOT_GOLD_C"), 255, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4LOOT_GOLD_C_SCOPED"), 255, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4LOOT_WEED_I"), 0, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4LOOT_WEED_I_SCOPED"), 0, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4LOOT_WEED_C"), 0, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4LOOT_WEED_C_SCOPED"), 0, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4LOOT_PAINT"), 127, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4LOOT_PAINT_SCOPED"), 127, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4LOOT_GOLD_V"), 598484, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4LOOT_PAINT_V"), 448863, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4_PROGRESS"), 124271, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4_MISSIONS"), 65535, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4_PLAYTHROUGH_STATUS"), 40000, TRUE);
					click.setResponse(LOC("H4_FNSH_T"));
				}
			});
			cayo->createChild<CommandListActionLambda>(LOC("H_TRGT"), { CMDNAME("cayotarget") }, NOLABEL, std::vector<CommandListActionItemData>{
				{ 5, LOC("H4_TRGT_5"), { CMDNAME("panther") } },
				{ 4, LOC("H4_TRGT_4"), { CMDNAME("files") } },
				{ 3, LOC("H4_TRGT_3"), { CMDNAME("diamond") } },
				{ 2, LOC("H4_TRGT_2"), { CMDNAME("bonds") } },
				{ 1, LOC("H4_TRGT_1"), { CMDNAME("necklace") } },
				{ 0, LOC("H4_TRGT_0"), { CMDNAME("tequila") } },
			}, [](Click& click, CommandListActionItem* item)
				{
					const auto char_slot = Util::getCharSlot(click);
					if (char_slot != -1)
					{
						const auto value = int(item->value);
						click.ensureScriptThread([char_slot, value]
						{
							STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H4CNF_TARGET"), value, TRUE);
						});
					}
				});
		}

		// Casino Heist
		{
			auto casino_heist = this->createChild<CommandList>(LOC("H3"), { CMDNAME("casinoheist") });
			{
				casino_heist->children.emplace_back(std::make_unique<CommandLambdaActionScript>(casino_heist, LOC("H3_RPT"), CMDNAMES("repeatcasinoheist"), LOC("H3_RPT_H"), [](const Click& click)
				{
					const auto char_slot = Util::getCharSlot(click);
					if (char_slot != -1)
					{
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H3_COMPLETEDPOSIX"), -1, true);
						click.setResponse(LOC("H3_RPT_T"));
					}
				}));

				// P.O.I. & Access Points
				{
					auto scoping = casino_heist->createChild<CommandList>(LOC("H3_POIACC"));

					scoping->createChild<CommandLambdaActionScript>(LOC("SCOP"), CMDNAMES("scopecasino"), LOC("H3_SCOP_H"), [](const Click& click)
					{
						const auto char_slot = Util::getCharSlot(click);
						if (char_slot != -1)
						{
							STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H3OPT_POI"), 1023, true);
							STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H3OPT_ACCESSPOINTS"), 2047, true);
						}
					});
					scoping->createChild<CommandLambdaActionScript>(LOC("USCOP"), CMDNAMES("unscopecasino"), NOLABEL, [](const Click& click)
					{
						const auto char_slot = Util::getCharSlot(click);
						if (char_slot != -1)
						{
							STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H3OPT_POI"), 0, true);
							STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H3OPT_ACCESSPOINTS"), 0, true);
						}
					});
				}

				casino_heist->createChild<CommandListActionLambda>(LOC("H3_VLT"), { CMDNAME("casinovault") }, LOC("H3_VLT_H"), std::vector<CommandListActionItemData>{
					{3, LOC("H3_VLT_3"), { CMDNAME("diamonds") }},
					{ 2, LOC("H3_VLT_2"), CMDNAMES("art") },
					{ 1, LOC("H3_VLT_1"), CMDNAMES("gold") },
					{ 0, LOC("H3_VLT_0"), CMDNAMES("cash") },
				}, [](Click& click, CommandListActionItem* item)
					{
						const auto char_slot = Util::getCharSlot(click);
						if (char_slot != -1)
						{
							const auto value = int(item->value);
							click.ensureScriptThread([char_slot, value]
							{
								STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H3OPT_TARGET"), value, true);
							});
							click.setResponse(LOC("H3_VLT_T"));
						}
					});
				casino_heist->createChild<CommandListActionLambda>(LOC("H3_FNSH1"), { CMDNAME("casinoapproach") }, LOC("H3_FNSH1_H"), std::vector<CommandListActionItemData>{
					{1, LOC("H3_FNSH1_1"), { CMDNAME("silentandsneaky") }},
					{ 2, LOC("H3_FNSH1_2"), CMDNAMES("bigcon") },
					{ 3, LOC("H3_FNSH1_3"), CMDNAMES("aggressive") },
				}, [](Click& click, CommandListActionItem* item)
					{
						const auto char_slot = Util::getCharSlot(click);
						if (char_slot != -1)
						{
							const auto value = int(item->value);
							click.ensureScriptThread([char_slot, value]
							{
								STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H3OPT_APPROACH"), value, true);
								STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H3OPT_BITSET1"), -1, true);
							});
							click.setResponse(LOC("H3_FNSH1_T"));
						}
					});
				casino_heist->children.emplace_back(std::make_unique<CommandLambdaActionScript>(casino_heist, LOC("H3_FNSH2"), CMDNAMES("casinosecondboard"), NOLABEL, [](const Click& click)
				{
					const auto char_slot = Util::getCharSlot(click);
					if (char_slot != -1)
					{
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H3OPT_DISRUPTSHIP"), 3, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H3OPT_BODYARMORLVL"), 3, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H3OPT_KEYLEVELS"), 2, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H3OPT_BITSET0"), -1, true);
						click.setResponse(LOC("H3_FNSH2_T"));
					}
				}));
				casino_heist->children.emplace_back(std::make_unique<CommandLambdaActionScript>(casino_heist, LOC("H3_RSTCRW"), CMDNAMES("casinoresetcrew"), LOC("H3_RSTCRW_H"), [](const Click& click)
				{
					const auto char_slot = Util::getCharSlot(click);
					if (char_slot != -1)
					{
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H3OPT_CREWWEAP"), 0, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H3OPT_CREWDRIVER"), 0, true);
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "H3OPT_CREWHACKER"), 0, true);
						click.setResponse(LOC("H3_RSTCRW_T"));
					}
				}));
			}
		}

		// Casino
		{
			auto casino = this->createChild<CommandList>(LOC("CSNO"));

			casino->createChild<CommandCasinoWheel>();
			casino->createChild<CommandCasinoSlots>();
			casino->createChild<CommandCasinoBlackjack>();
			casino->createChild<CommandCasinoRoulette>();
		}

		this->createChild<CommandReadonlyLink>(LOC("MONEYLINK2"), soup::ObfusString("https://stand.sh/help/money"));
		this->createChild<CommandLambdaAction>(LOC("SNKSNARMR"), { CMDNAME("fillinventory") }, NOLABEL, [](Click& click)
		{
			const auto char_slot = Util::getCharSlot(click);
			if (char_slot != -1)
			{
				STATS::STAT_SET_INT(CHAR_STAT(char_slot, "NO_BOUGHT_YUM_SNACKS"), 30, true);
				STATS::STAT_SET_INT(CHAR_STAT(char_slot, "NO_BOUGHT_HEALTH_SNACKS"), 15, true);
				STATS::STAT_SET_INT(CHAR_STAT(char_slot, "NO_BOUGHT_EPIC_SNACKS"), 5, true);
				STATS::STAT_SET_INT(CHAR_STAT(char_slot, "NUMBER_OF_ORANGE_BOUGHT"), 10, true);
				STATS::STAT_SET_INT(CHAR_STAT(char_slot, "NUMBER_OF_BOURGE_BOUGHT"), 10, true);
				STATS::STAT_SET_INT(CHAR_STAT(char_slot, "CIGARETTES_BOUGHT"), 20, true);
				STATS::STAT_SET_INT(CHAR_STAT(char_slot, "NUMBER_OF_SPRUNK_BOUGHT"), 10, true);
				STATS::STAT_SET_INT(CHAR_STAT(char_slot, "NUMBER_OF_CHAMP_BOUGHT"), 5, true);
				STATS::STAT_SET_INT(CHAR_STAT(char_slot, "MP_CHAR_ARMOUR_1_COUNT"), 10, true);
				STATS::STAT_SET_INT(CHAR_STAT(char_slot, "MP_CHAR_ARMOUR_2_COUNT"), 10, true);
				STATS::STAT_SET_INT(CHAR_STAT(char_slot, "MP_CHAR_ARMOUR_3_COUNT"), 10, true);
				STATS::STAT_SET_INT(CHAR_STAT(char_slot, "MP_CHAR_ARMOUR_4_COUNT"), 10, true);
				STATS::STAT_SET_INT(CHAR_STAT(char_slot, "MP_CHAR_ARMOUR_5_COUNT"), 10, true);
				click.setResponse(LOC("SNKSNARMR_T"));
			}
		});
		this->createChild<CommandFinishDoomsdayHeistSetups>();
		this->createChild<CommandFinishApartmentHeistSetups>();
		this->createChild<CommandSetRank>();

		// Nightclub Popularity
		{
			auto p = this->createChild<CommandList>(LOC("CLUBPOP_P"));

			p->createChild<CommandClubPop>();
			p->createChild<CommandClubPopLock>();
		}

		// Fast Run Ability
		{
			auto fast_run = this->createChild<CommandList>(LOC("FSTRUN"));

			fast_run->createChild<CommandLambdaActionScript>(LOC("ABILOPT"), CMDNAMES(), NOLABEL, [](const Click& click)
			{
				const auto char_slot = Util::getCharSlot(click);
				if (char_slot != -1)
				{
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "CHAR_ABILITY_1_UNLCK"), -1, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "CHAR_ABILITY_2_UNLCK"), -1, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "CHAR_ABILITY_3_UNLCK"), -1, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "CHAR_FM_ABILITY_1_UNLCK"), -1, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "CHAR_FM_ABILITY_2_UNLCK"), -1, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "CHAR_FM_ABILITY_3_UNLCK"), -1, TRUE);
					click.setResponse(LOC("GEN_T_NEWSESS"));
				}
			});
			fast_run->createChild<CommandLambdaActionScript>(LOC("ABILUNL"), CMDNAMES(), NOLABEL, [](const Click& click)
			{
				const auto char_slot = Util::getCharSlot(click);
				if (char_slot != -1)
				{
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "CHAR_ABILITY_1_UNLCK"), 0, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "CHAR_ABILITY_2_UNLCK"), 0, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "CHAR_ABILITY_3_UNLCK"), 0, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "CHAR_FM_ABILITY_1_UNLCK"), 0, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "CHAR_FM_ABILITY_2_UNLCK"), 0, TRUE);
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "CHAR_FM_ABILITY_3_UNLCK"), 0, TRUE);
					click.setResponse(LOC("GEN_T_NEWSESS"));
				}
			});
		}

		// Bad Sport
		{
			auto bad_sport = this->createChild<CommandList>(LOC("BADSPORT"));

			bad_sport->createChild<CommandLambdaActionScript>(LOC("ABILOPT"), CMDNAMES(), NOLABEL, [](const Click& click)
			{
				if (click.inOnline())
				{
					STATS::STAT_SET_FLOAT(ATSTRINGHASH("MPPLY_OVERALL_BADSPORT"), 60000.0f, TRUE);
					STATS::STAT_SET_BOOL(ATSTRINGHASH("MPPLY_CHAR_IS_BADSPORT"), TRUE, TRUE);
					STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_BADSPORT_MESSAGE"), -1, TRUE);
					STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_BECAME_BADSPORT_NUM"), -1, TRUE);
				}
			});
			bad_sport->createChild<CommandLambdaActionScript>(LOC("RELINQ"), CMDNAMES(), NOLABEL, [](const Click& click)
			{
				if (click.inOnline())
				{
					STATS::STAT_SET_FLOAT(ATSTRINGHASH("MPPLY_OVERALL_BADSPORT"), 0.0f, TRUE);
					STATS::STAT_SET_BOOL(ATSTRINGHASH("MPPLY_CHAR_IS_BADSPORT"), FALSE, TRUE);
					STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_BADSPORT_MESSAGE"), 0, TRUE);
					STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_BECAME_BADSPORT_NUM"), 0, TRUE);
				}
			});
		}
	}
}
