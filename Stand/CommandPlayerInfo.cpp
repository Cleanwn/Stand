#include "CommandPlayerInfo.hpp"

#include <fmt/xchar.h>

#include "AbstractEntity.hpp"
#include "AbstractModel.hpp"
#include "AbstractPlayer.hpp"
#include "rlclancommon.hpp"
#include "CommandPlayer.hpp"
#include "CommandDivider.hpp"
#include "CommandLambdaAction.hpp"
#include "CommandListPlayer.hpp"
#include "CommandPlayerIntelLambda.hpp"
#include "CommandSpoofClan.hpp"
#include "format_duration.hpp"
#include "GamerIdentifier.hpp"
#include "get_current_time_millis.hpp"
#include "Hooking.hpp"
#include "is_rid.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "netConnection.hpp"
#include "netEndpoint.hpp"
#include "netPeerId.hpp"
#include "netSocketAddress.hpp"
#include "PlayerProvider.hpp"
#include "ScAccount.hpp"
#include "ScriptGlobal.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"
#include "Vector2Plus.hpp"
#include "weapons.hpp"

#ifdef STAND_DEBUG
#include "CNetGamePlayer.hpp"
#include "netPeerAddress.hpp"
#include "peercert.hpp"
#include "pointers.hpp"
#include "rlGamerId.hpp"
#endif

namespace Stand
{
	CommandPlayerInfo::CommandPlayerInfo(CommandList* const parent)
		: CommandOnPlayer(parent, LOC("INFO"), CMDNAMES_OBF("info", "pinfo", "pi"), NOLABEL, CMDFLAGS_LIST_READONLY | CMDFLAG_TEMPORARY | CMDFLAG_SEARCH_FINISHLIST)
	{
		PC_PTR->registerCommandNames(this);

		{
			auto* status = createChild<CommandListReadonly>(LOC("STUS"), {}, NOLABEL, CMDFLAGS_LIST_READONLY | CMDFLAG_TEMPORARY);

			status->createChild<CommandPlayerIntelLambda>(LOC("DIST"), [](const AbstractPlayer p) -> std::wstring
			{
				if (!p.hasPed())
				{
					return LANG_GET_W("NA");
				}
				if (g_player == p)
				{
					return L"0";
				}
				return fmt::to_wstring((long long)p.getPos().distance(g_player_ped.getPos()));
			});
			status->createChild<CommandPlayerIntelLambda>(LOC("HLTH"), [](const AbstractPlayer p) -> std::wstring
			{
				if (!p.hasPed())
				{
					return LANG_GET_W("NA");
				}
				AbstractEntity ped = p.getPed();
				auto str = fmt::to_wstring((int)ped.getHealth());
				str.push_back('/');
				str.append(fmt::to_wstring((int)ped.getMaxHealth()));
				return str;
			});
			status->createChild<CommandPlayerIntelLambda>(LOC("ARMR"), [](const AbstractPlayer p) -> std::wstring
			{
				if (!p.hasPed())
				{
					return LANG_GET_W("NA");
				}
				return fmt::to_wstring((long long)p.getPed().getArmour());
			});
			status->createChild<CommandPlayerIntelLambda>(LOC("WPN_SINGULAR"), [](const AbstractPlayer p) -> std::wstring
			{
				auto hash = p.getPed().getSelectedWeapon();
				if (auto wpn = Weapon::find(hash))
				{
					return StringUtils::utf8_to_utf16(wpn->getDisplayName());
				}
				else if (hash == ATSTRINGHASH("WEAPON_UNARMED"))
				{
					return StringUtils::utf8_to_utf16(Weapon::weapon_unarmed.getDisplayName());
				}
				return LANG_GET_W("NA");
			});
			status->createChild<CommandPlayerIntelLambda>(LOC("VEH"), [](const AbstractPlayer p) -> std::wstring
			{
				auto veh = p.getCurrentVehicleModel();
				if (veh != 0)
				{
					return veh.getLabel().getLiteralUtf16();
				}
				return LANG_GET_W("NA");
			});
			status->createChild<CommandPlayerIntelLambda>(LOC("LANG"), [](const AbstractPlayer p)
			{
				return p.getLanguageName().getLocalisedUtf16();
			});
			status->createChild<CommandPlayerIntelLambda>(LOC("CTRLLR"), [](const AbstractPlayer p)
			{
				return p.isUsingController() ? LANG_GET_W("YES") : LANG_GET_W("NO");
			});
			status->createChild<CommandPlayerIntelLambda>(LOC("WNTDLVL"), [](const AbstractPlayer p)
			{
				return fmt::to_wstring(p.getWantedLevel());
			});
			status->createChild<CommandPlayerIntelLambda>(LOC("MNTLS"), [](const AbstractPlayer p)
			{
				return fmt::to_wstring((int)*p.mentalState());
			});
			status->createChild<CommandPlayerIntelLambda>(LOC("DMG_WEAP"), [](const AbstractPlayer p)
			{
				return fmt::to_wstring(p.getWeaponDamageModifier());
			});
			status->createChild<CommandPlayerIntelLambda>(LOC("DMG_MELEE"), [](const AbstractPlayer p)
			{
				return fmt::to_wstring(p.getMeleeWeaponDamageModifier());
			});

			status->createChild<CommandPlayerIntelLambda>(LOC("APTID"), [](const AbstractPlayer p)
			{
				return fmt::to_wstring(p.getApartmentId());
			});
			status->createChild<CommandPlayerIntelLambda>(LOC("INTID"), [](const AbstractPlayer p)
			{
				return fmt::to_wstring(p.getInteriorId());
			});
			status->createChild<CommandPlayerIntelLambda>(LOC("SPID2"), [](const AbstractPlayer p)
			{
				return fmt::to_wstring(p.p);
			});
#ifdef STAND_DEBUG
			status->createChild<CommandPlayerIntelLambda>(LIT("[Debug Build] Roaming Bubble ID"), [](const AbstractPlayer p)
			{
				if (auto cngp = p.getCNetGamePlayer())
				{
					return fmt::to_wstring(cngp->m_RoamingBubbleMemberInfo.m_BubbleID);
				}
				return LANG_GET_W("NA");
			}, CMDFLAGS_READONLY_VALUE_COPY | CMDFLAG_FEATURELIST_SKIP);
			status->createChild<CommandPlayerIntelLambda>(LIT("[Debug Build] Roaming Bubble Player ID"), [](const AbstractPlayer p)
			{
				if (auto cngp = p.getCNetGamePlayer())
				{
					return fmt::to_wstring(cngp->m_RoamingBubbleMemberInfo.m_PlayerID);
				}
				return LANG_GET_W("NA");
			}, CMDFLAGS_READONLY_VALUE_COPY | CMDFLAG_FEATURELIST_SKIP);
#endif

			status->createChild<CommandDivider>(LOC("ORG"));
			status->createChild<CommandPlayerIntelLambda>(LOC("TYP"), [](const AbstractPlayer p)
			{
				if (!p.isInAnOrg())
				{
					return LANG_GET_W("NA");
				}
				return p.isInMc() ? LANG_GET_W("ORGT_MC") : LANG_GET_W("ORGT_SV");
			});
			status->createChild<CommandPlayerIntelLambda>(LOC("BOSS"), [](const AbstractPlayer p)
			{
				if (!p.isInAnOrg())
				{
					return LANG_GET_W("NA");
				}
				return p.getBoss().getNameW();
			});
			status->createChild<CommandPlayerIntelLambda>(LOC("NME"), [](const AbstractPlayer p)
			{
				if (!p.isInAnOrg())
				{
					return LANG_GET_W("NA");
				}
				return StringUtils::utf8_to_utf16(p.getOrgName());
			});
		}

		{
			auto* connection = this->createChild<CommandList>(LOC("CNNTN"));

			connection->createChild<CommandPlayerIntelLambda>(LOC("PING"), [](const AbstractPlayer p)
			{
				if (p == g_player)
				{
					return LANG_GET_W("NA");
				}
				return fmt::format(L"{} {}", NETWORK::NETWORK_GET_AVERAGE_LATENCY(p), LANG_GET_W("MS"));
			});

			connection->createChild<CommandPlayerIntelLambda>(LOC("CONVIA"), [](const AbstractPlayer p)
			{
				if (p == g_player)
				{
					return LANG_GET_W("NA");
				}
				return p.getConnectedViaForPlayerInfoValue();
			});
			connection->createChild<CommandPlayerIntelLambda>(LOC("RLYIP"), [](const AbstractPlayer p)
			{
				if (auto endpoint = p.getEndpoint())
				{
					return endpoint->relay_address.proxy_sock_addr.ip.toString<std::wstring>();
				}
				return LANG_GET_W("NA");
			});
			connection->createChild<CommandPlayerIntelLambda>(LOC("RLYPRT"), [](const AbstractPlayer p)
			{
				if (auto endpoint = p.getEndpoint())
				{
					return fmt::to_wstring(endpoint->relay_address.proxy_sock_addr.port);
				}
				return LANG_GET_W("NA");
			});
			connection->createChild<CommandPlayerIntelLambda>(LOC("LANIP"), [](const AbstractPlayer p)
			{
				auto addr = p.getLanAddress();
				if (addr.ip != -1)
				{
					return addr.ip.toString<std::wstring>();
				}
				return LANG_GET_W("NA");
			});

			connection->createChild<CommandPlayerIntelLambda>(LOC("IP"), [](const AbstractPlayer p)
			{
				return p.getIpAddressForUserW();
			});
			connection->createChild<CommandPlayerIntelLambda>(LOC("PRT"), [](const AbstractPlayer p)
			{
				if (p != g_player)
				{
					auto addr = p.getAddress();
					if (addr.ip != -1)
					{
						return fmt::to_wstring(addr.port);
					}
				}
				return LANG_GET_W("NA");
			});
			connection->createChild<CommandDivider>(LIT("GeoIP"));
			connection->createChild<CommandPlayerIntelLambda>(LOC("CT"), [](const AbstractPlayer p) -> std::wstring
			{
				return StringUtils::utf8_to_utf16(p.getAddress().ip.getGeoIpField(GEOIP_CITY));
			});
			connection->createChild<CommandPlayerIntelLambda>(LOC("RGION"), [](const AbstractPlayer p) -> std::wstring
			{
				return StringUtils::utf8_to_utf16(p.getAddress().ip.getGeoIpField(GEOIP_STATE));
			});
			connection->createChild<CommandPlayerIntelLambda>(LOC("CTRY"), [](const AbstractPlayer p) -> std::wstring
			{
				return StringUtils::utf8_to_utf16(p.getAddress().ip.getGeoIpField(GEOIP_COUNTRY));
			});
			connection->createChild<CommandPlayerIntelLambda>(LIT("ISP"), [](const AbstractPlayer p) -> std::wstring
			{
				return StringUtils::utf8_to_utf16(p.getAddress().ip.getGeoIpField(GEOIP_ISP));
			});
		}

		{
			auto* stats = createChild<CommandListReadonly>(LOC("STS"), {}, NOLABEL, CMDFLAGS_LIST_READONLY | CMDFLAG_TEMPORARY);

			stats->createChild<CommandPlayerIntelLambda>(LOC("RNK"), [](const AbstractPlayer p)
			{
				const int current_rank = p.getRank();
				std::wstring str = fmt::to_wstring(current_rank);
				const auto rp_required_for_current_rank = (double)Util::get_rp_required_for_rank(current_rank);
				const auto perc = (long long)((((double)p.getRp() - rp_required_for_current_rank) / ((double)Util::get_rp_required_for_rank(current_rank + 1) - rp_required_for_current_rank)) * 100.0);
				if (perc >= 0 && perc <= 100)
				{
					str.append(L" + ").append(fmt::to_wstring(perc)).push_back(L'%');
				}
				return str;
			});
			stats->createChild<CommandPlayerIntelLambda>(LOC("MNY_W"), [](const AbstractPlayer p)
			{
				return StringUtils::toStringWithThousandsSeparatorW(p.getWallet());
			});
			stats->createChild<CommandPlayerIntelLambda>(LOC("MNY_B"), [](const AbstractPlayer p)
			{
				return StringUtils::toStringWithThousandsSeparatorW(p.getBank());
			});
			stats->createChild<CommandPlayerIntelLambda>(LOC("MNY_A"), [](const AbstractPlayer p)
			{
				return StringUtils::toStringWithThousandsSeparatorW(p.getMoney());
			});
			stats->createChild<CommandPlayerIntelLambda>(LOC("KLLS"), [](const AbstractPlayer p)
			{
				return StringUtils::toStringWithThousandsSeparatorW(p.getKills());
			});
			stats->createChild<CommandPlayerIntelLambda>(LOC("DTHS"), [](const AbstractPlayer p)
			{
				return StringUtils::toStringWithThousandsSeparatorW(p.getDeaths());
			});
			stats->createChild<CommandPlayerIntelLambda>(LOC("KD"), [](const AbstractPlayer p)
			{
				return fmt::to_wstring(p.getKd());
			});
			stats->createChild<CommandPlayerIntelLambda>(LOC("FAVVEH"), [](const AbstractPlayer p) -> std::wstring
			{
				if (auto hash = ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(p, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_FAVOURITE_VEHICLE).get<Hash>())
				{
					return AbstractModel(hash).getLabel().getLocalisedUtf16();
				}
				return LANG_GET_W("NA");
			});
			stats->createChild<CommandPlayerIntelLambda>(LOC("FAVWEAP"), [](const AbstractPlayer p) -> std::wstring
			{
				if (auto w = Weapon::find(ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(p, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_FAVOURITE_WEAPON).get<Hash>()))
				{
					return StringUtils::utf8_to_utf16(w->getDisplayName());
				}
				return LANG_GET_W("NA");
			});
			stats->createChild<CommandPlayerIntelLambda>(LOC("FAVRDSTAT"), [](const AbstractPlayer p) -> std::wstring
			{
				if (auto station = ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(p, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_FAVOURITE_STATION).get<Hash>())
				{
					return StringUtils::utf8_to_utf16(Util::GET_LABEL_TEXT(AUDIO::GET_RADIO_STATION_NAME(AUDIO::FIND_RADIO_STATION_INDEX(station)), false));
				}
				return LANG_GET_W("NA");
			});

			stats->createChild<CommandPlayerIntelLambda>(LOC("OWNBKR"), [](const AbstractPlayer p)
			{
				return ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(p, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_BUNKER_ID_BASE).at(GLOBAL_PLAYER_STAT_BUNKER_ID_OFFSET, GLOBAL_PLAYER_STAT_BUNKER_ID_SIZE).get<int>() != 0 ? LANG_GET_W("YES") : LANG_GET_W("NO");
			});
			stats->createChild<CommandPlayerIntelLambda>(LOC("OWNFAC"), [](const AbstractPlayer p)
			{
				return ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(p, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_FACILITY_ID).get<int>() != 0 ? LANG_GET_W("YES") : LANG_GET_W("NO");
			});
			stats->createChild<CommandPlayerIntelLambda>(LOC("OWNCLB"), [](const AbstractPlayer p)
			{
				return ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(p, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_NIGHTCLUB_ID).get<int>() != 0 ? LANG_GET_W("YES") : LANG_GET_W("NO");
			});
		}

		{
			auto clan = this->createChild<CommandList>(LOC("CRW"));

			clan->createChild<CommandPlayerIntelLambda>(LIT("ID"), [](const AbstractPlayer p) -> std::wstring
			{
				return fmt::to_wstring(p.getActiveClanMembership()->clan.id);
			});
			clan->createChild<CommandPlayerIntelLambda>(LOC("NME"), [](const AbstractPlayer p) -> std::wstring
			{
				return StringUtils::utf8_to_utf16(p.getActiveClanMembership()->clan.name);
			});
			clan->createChild<CommandPlayerIntelLambda>(LOC("CRWTG"), [](const AbstractPlayer p) -> std::wstring
			{
				return StringUtils::utf8_to_utf16(p.getActiveClanMembership()->clan.tag);
			});
			clan->createChild<CommandPlayerIntelLambda>(LOC("MOTTO"), [](const AbstractPlayer p) -> std::wstring
			{
				return StringUtils::utf8_to_utf16(p.getActiveClanMembership()->clan.motto);
			});
			clan->createChild<CommandPlayerIntelLambda>(LOC("ALTBDG"), [](const AbstractPlayer p) -> std::wstring
			{
				return p.getActiveClanMembership()->clan.alt_badge ? LANG_GET_W("YES") : LANG_GET_W("NO");
			});
			clan->createChild<CommandLambdaAction>(LOC("CPY2SPFCRW"), {}, NOLABEL, [this](Click& click)
			{
				memcpy(&g_hooking.spoofed_clan_membership, PP_PTR->getPlayer().getActiveClanMembership(), sizeof(rage::rlClanMembershipData));
				CommandSpoofClan::instance->setValuesFromHooking(click);
			});
		}

		this->createChild<CommandPlayerIntelLambda>(LOC("NME_S"), [](const AbstractPlayer p)
		{
			return StringUtils::utf8_to_utf16(p.getName());
		});
		this->createChild<CommandPlayerIntelLambda>(LOC("NME_H"), [](const AbstractPlayer p)
		{
			return p.getSessionNameForUser();
		});
		this->createChild<CommandPlayerIntelLambda>(LOC("NME_R"), [](const AbstractPlayer p)
		{
			if (p.isInScope(AbstractPlayer::hide_unfiltered_name))
			{
				return LANG_GET_W("RDCTD");
			}
			auto rid = p.getRockstarId();
			if (is_rid_valid(rid))
			{
				auto* a = ScAccount::fromRID(rid);
				if (!a->isComplete())
				{
					a->requestCompletion();
					return LANG_GET_W("GIP_Q");
				}
				if (a->hasName())
				{
					return StringUtils::utf8_to_utf16(a->name);
				}
			}
			return LANG_GET_W("NA");
		});
		this->createChild<CommandPlayerIntelLambda>(LIT("Rockstar ID"), [](const AbstractPlayer p)
		{
			return p.getRockstarIdForUser<std::wstring>();
		});
		this->createChild<CommandPlayerIntelLambda>(LOC("DSCVAGO"), [](const AbstractPlayer p) -> std::wstring
		{
			if (auto cmd = p.getCommand())
			{
				return StringUtils::utf8_to_utf16(LANG_FMT("DUR_AGO", format_duration(GET_MILLIS_SINCE(cmd->discovery))));
			}
			return {};
		});
		this->createChild<CommandPlayerIntelLambda>(LOC("GODTIME"), [](const AbstractPlayer p) -> std::wstring
		{
			if (auto cmd = p.getCommand(); cmd && cmd->invulnerable_since != 0)
			{
				return StringUtils::utf8_to_utf16(format_duration(GET_MILLIS_SINCE(cmd->invulnerable_since)));
			}
			return LANG_GET_W("NA");
		});
		/*this->createChild<CommandPlayerIntelLambda>(LOC("HOSTKN"), [](const AbstractPlayer p)
		{
			return StringUtils::utf8_to_utf16(Util::to_padded_hex_string(p.getPeerId()));
		});*/
		this->createChild<CommandPlayerIntelLambda>(LOC("HOSTQPOS"), [](const AbstractPlayer p) -> std::wstring
		{
			if (auto pos = p.getHostQueuePosition(); pos != 0)
			{
				return fmt::to_wstring(pos);
			}
			return LANG_GET_W("NA");
		});
		this->createChild<CommandPlayerIntelLambda>(LOC("HQSA_NH"), [](const AbstractPlayer p)
		{
			return StringUtils::utf8_to_utf16(Util::to_padded_hex_string(p.getNameHash()));
		});

#ifdef STAND_DEBUG
		{
			auto secret = this->createChild<CommandList>(LIT("Debug Build Shit"), {}, NOLABEL, CMDFLAGS_LIST | CMDFLAG_FEATURELIST_SKIP);

			secret->createChild<CommandPlayerIntelLambda>(LIT("GIv4"), [](const AbstractPlayer p)
			{
				return StringUtils::utf8_to_utf16(GamerIdentifierV4::fromGamerInfo(*p.getGamerInfo()).toString());
			});
			/*secret->createChild<CommandPlayerIntelLambda>(LIT("Gamer ID"), [](const AbstractPlayer p)
			{
				return StringUtils::utf8_to_utf16(Util::to_padded_hex_string(p.getGamerId()));
			});*/
			secret->createChild<CommandPlayerIntelLambda>(LIT("Account ID"), [](const AbstractPlayer p)
			{
				return fmt::to_wstring(p.getAccountId());
			});
			secret->createChild<CommandPlayerIntelLambda>(LIT("MAC ID"), [](const AbstractPlayer p)
			{
				return StringUtils::utf8_to_utf16(Util::to_padded_hex_string(p.getMacId()));
			});
			secret->createChild<CommandPlayerIntelLambda>(LIT("Startup ID"), [](const AbstractPlayer p)
			{
				return StringUtils::utf8_to_utf16(Util::to_padded_hex_string(p.getStartupId()));
			});
			secret->createChild<CommandPlayerIntelLambda>(LIT("CNetGamePlayer*"), [](const AbstractPlayer p)
			{
				return StringUtils::utf8_to_utf16(Util::to_padded_hex_string((uint64_t)p.getCNetGamePlayer()));
			});
			secret->createChild<CommandPlayerIntelLambda>(LIT("rage::netEndpoint*"), [](const AbstractPlayer p)
			{
				return StringUtils::utf8_to_utf16(Util::to_padded_hex_string((uint64_t)p.getEndpoint()));
			});
			/*secret->createChild<CommandPlayerIntelLambda>(LIT("rage::netPeerCertificate*"), [](const AbstractPlayer p)
			{
				if (auto endpoint = p.getEndpoint())
				{
					return StringUtils::utf8_to_utf16(Util::to_padded_hex_string((uint64_t)rage::findCertByKey(endpoint->getCertKey())));
				}
				return LANG_GET_W("NA");
			});*/
			secret->createChild<CommandPlayerIntelLambda>(LIT("Endpoint ID"), [](const AbstractPlayer p)
			{
				if (auto endpoint_id = p.getEndpointId(); endpoint_id != -1)
				{
					return fmt::format(L"{} / 0x{:x}", endpoint_id, endpoint_id);
				}
				return LANG_GET_W("NA");
			});
			secret->createChild<CommandPlayerIntelLambda>(LIT("Channel 7 Con ID"), [](const AbstractPlayer p)
			{
				if (auto endpoint = p.getEndpoint(); endpoint && endpoint->channel_cxns[7])
				{
					return fmt::format(L"{} / 0x{:x}", endpoint->channel_cxns[7]->id, endpoint->channel_cxns[7]->id);
				}
				return LANG_GET_W("NA");
			});
			secret->createChild<CommandPlayerIntelLambda>(LIT("Channel 13 Con ID"), [](const AbstractPlayer p)
			{
				if (auto endpoint = p.getEndpoint(); endpoint && endpoint->channel_cxns[13])
				{
					return fmt::format(L"{} / 0x{:x}", endpoint->channel_cxns[13]->id, endpoint->channel_cxns[13]->id);
				}
				return LANG_GET_W("NA");
			});
		}
#endif
	}
}
