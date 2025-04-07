#pragma once

#include "netGameEventId.hpp"

#include <string>

#include "conf_classifier.hpp"

namespace Stand
{
	struct FlowEvent
	{
		enum _ : flowevent_t
		{
			NE_ANY = NETWORK_INCREMENT_STAT_EVENT,
			NE_VEHTAKEOVER = NET_EVENTS_END,
			NE_INFRING,
			NE_CAMSHAKE,
			NE_EXPSPAM,
			NE_RAGDOLL,
			NE_SUDDENDEATH,
			NE_DISARM,
			_SE_BEGIN,
			SE_CRASH = _SE_BEGIN,
			SE_KICK,
			SE_PVKICK,
			SE_SENDTOJOB,
			SE_CEOKICK,
			SE_STARTFMMISSION,
			SE_STARTFMMISSION_NCEO,
			SE_INTERIORWARP,
			SE_INTERIORWARP_NCEO,
			SE_GIVECOLLE,
			SE_GIVECOLLE_NCEO,
			SE_INVALID,
			SE_UNUSUAL,
			SE_APTINV,
			SE_INFLOAD,
			SE_DISABLEDRIVINGVEHICLES,
			SE_CAYO_INV,
			SE_FORCECAMFRONT,
			SE_FREEZE,
			SE_NOTIFY_EMPTY,
			SE_INTERIORKICK,
			SE_TRIGGERDEFEND,
			_SE_END,
			_PUP_BEGIN = _SE_END,
			PUP_ANY = _PUP_BEGIN,
			PUP_INVALID,
			PUP_RP,
			PUP_CASH,
			_PUP_END,
			_SYNCIN_BEGIN = _PUP_END,
			SYNCIN_ANY = _SYNCIN_BEGIN,
			SYNCIN_IMS,
			SYNCIN_WOS,
			SYNCIN_CAGE,
			SYNCIN_ATTACH,
			SYNCIN_ACK_CLONE_CREATE,
			SYNCIN_ACK_CLONE_UPDATE,
			SYNCIN_ACK_CLONE_DELETE,
			SYNCIN_CLONE_CREATE,
			SYNCIN_CLONE_UPDATE,
			SYNCIN_CLONE_DELETE,
			SYNCIN_CONTROL,
			_SYNCIN_END,
			_SYNCOUT_BEGIN = _SYNCIN_END,
			SYNCOUT_CLONE_CREATE = _SYNCOUT_BEGIN,
			SYNCOUT_CLONE_UPDATE,
			SYNCOUT_CLONE_DELETE,
			_SYNCOUT_END,
			_MOD_BEGIN = _SYNCOUT_END,
			MOD_ANY = _MOD_BEGIN,
			MOD_ID_STAND,
			MOD_ID_STAND_COLOADING,
			MOD_ID_OTHER,
			MOD_RAC,
			MOD_HEALTH,
			MOD_LONG_DEATH,
			MOD_LONG_OTR,
			MOD_ARMOUR,
			MOD_PRESENCESPOOFING,
			MOD_HOSTKN_HANDICAP,
			MOD_HOSTKN_SWEET,
			MOD_HOSTKN_AGGRESSIVE,
			MOD_SPFDHOSTKN,
			MOD_GODATK,
			MOD_FROG,
			MOD_RSDEV,
			MOD_RSQA,
			MOD_CHEAT,
			MOD_RP2,
			MOD_RP3,
			MOD_RP4,
			MOD_T2IP,
			MOD_ADMINRID,
			MOD_MODEL,
			MOD_DMGMUL,
			MOD_NONET,
			MOD_DRIVEB,
			MOD_SILENTCHAT,
			MOD_BOUNTYSPAM,
			MOD_SUSSYBOUNTY,
			MOD_EXPBLAME,
			MOD_BADEXPLOSION,
			MOD_ORGNAME,
			MOD_CAYO,
			MOD_LLLUBE,
			MOD_MISC,
#ifdef STAND_DEBUG
			MOD_INCONGRUENCE_1,
			MOD_INCONGRUENCE_2,
#endif
			_MOD_END,
			_CHAT_BEGIN = _MOD_END,
			CHAT_ANY = _CHAT_BEGIN,
			CHAT_AD,
			CHAT_PROFANITYBYPASS,
			CHAT_SPAM,
			_CHAT_END,
			_SMS_BEGIN = _CHAT_END,
			SMS_ANY = _SMS_BEGIN,
			SMS_AD,
			SMS_PROFANITYBYPASS,
			_SMS_END,
			_CLSFN_BEGIN = _SMS_END,
			CLSFN_ANY = _CLSFN_BEGIN,
			CLSFN_PM,
			CLSFN_LM,
			CLSFN_MOD,
#if HAS_MODAMN
			CLSFN_MODAMN,
#endif
			CLSFN_AMN,
			_CLSFN_END,
			_SCRLAUNCH_BEGIN = _CLSFN_END,
			SCRLAUNCH_ANY = _SCRLAUNCH_BEGIN,
			SCRLAUNCH_NOCAT,
			SCRLAUNCH_FE,
			SCRLAUNCH_AG,
			SCRLAUNCH_OFM,
			SCRLAUNCH_GRIEF,
			SCRLAUNCH_SERV,
			SCRLAUNCH_CUT,
			SCRLAUNCH_FOIM,
			SCRLAUNCH_PS,
			SCRLAUNCH_STRIKE,
			SCRLAUNCH_DPSSV,
			SCRLAUNCH_DARTS,
			SCRLAUNCH_IMPDM,
			SCRLAUNCH_SLASHER,
			_SCRLAUNCH_END,
			_CUSTOM_BEGIN = _SCRLAUNCH_END,
			CUSTOM_1 = _CUSTOM_BEGIN,
			CUSTOM_2,
			CUSTOM_3,
			CUSTOM_4,
			CUSTOM_5,
			CUSTOM_6,
			CUSTOM_7,
			CUSTOM_8,
			CUSTOM_9,
			CUSTOM_10,
			CUSTOM_11,
			CUSTOM_12,
			CUSTOM_13,
			CUSTOM_14,
			CUSTOM_15,
			CUSTOM_16,
			CUSTOM_17,
			CUSTOM_18,
			CUSTOM_19,
			CUSTOM_20,
			_CUSTOM_END,
			MISC_JOIN = _CUSTOM_END,
			MISC_HOSTCHANGE,
			MISC_SCRIPTHOSTCHANGE,
			MISC_VOTEKICK_ANY,
			MISC_VOTEKICK_ME,
			MISC_LOVELETTER,
			MISC_BOUNTY,
			MISC_PTFX,
			SIZE,
			MISC_HISTORYJOIN,
			MISC_MODELBLACKLIST,
		};

		inline static Label custom_event_names[_CUSTOM_END - _CUSTOM_BEGIN];

		[[nodiscard]] static Label getName(flowevent_t id) noexcept;
		[[nodiscard]] static Label getName(flowevent_t id, const std::string& extra_data) noexcept;
		[[nodiscard]] static Label getName(Label name, const std::string& extra_data) noexcept;

		[[nodiscard]] static constexpr bool is_net_or_script_event(flowevent_t event) noexcept
		{
			return event < _SE_END;
		}

		[[nodiscard]] static constexpr bool is_script_event(flowevent_t event) noexcept
		{
			return event >= _SE_BEGIN && event < _SE_END;
		}

		[[nodiscard]] static constexpr bool is_pickup(flowevent_t event) noexcept
		{
			return event >= _PUP_BEGIN && event < _PUP_END;
		}

		[[nodiscard]] static constexpr bool is_sync_in(flowevent_t event) noexcept
		{
			return event >= _SYNCIN_BEGIN && event < _SYNCIN_END;
		}

		[[nodiscard]] static constexpr bool is_sync_out(flowevent_t event) noexcept
		{
			return event >= _SYNCOUT_BEGIN && event < _SYNCOUT_END;
		}

		[[nodiscard]] static constexpr bool is_modder_detection(flowevent_t event) noexcept
		{
			return event >= _MOD_BEGIN && event < _MOD_END;
		}

		[[nodiscard]] static constexpr bool is_chat_message(flowevent_t event) noexcept
		{
			return event >= _CHAT_BEGIN && event < _CHAT_END;
		}

		[[nodiscard]] static constexpr bool is_sms(flowevent_t event) noexcept
		{
			return event >= _SMS_BEGIN && event < _SMS_END;
		}

		[[nodiscard]] static constexpr bool is_classification(flowevent_t event) noexcept
		{
			return event >= _CLSFN_BEGIN && event < _CLSFN_END;
		}

		[[nodiscard]] static constexpr bool is_script_launch(flowevent_t event) noexcept
		{
			return event >= _SCRLAUNCH_BEGIN && event < _SCRLAUNCH_END;
		}

		[[nodiscard]] static constexpr bool is_custom(flowevent_t event) noexcept
		{
			return event >= _CUSTOM_BEGIN && event < _CUSTOM_END;
		}


		[[nodiscard]] static constexpr bool is_host_token_detection(flowevent_t event) noexcept
		{
			switch (event)
			{
			case MOD_HOSTKN_HANDICAP:
			case MOD_HOSTKN_SWEET:
			case MOD_HOSTKN_AGGRESSIVE:
			case MOD_SPFDHOSTKN:
				return true;
			}
			return false;
		}

		[[nodiscard]] static constexpr bool has_player(flowevent_t event) noexcept
		{
			return !is_pickup(event) && !is_sync_out(event);
		}

		[[nodiscard]] static constexpr bool can_discriminate(flowevent_t event) noexcept
		{
			return event != MISC_HISTORYJOIN && has_player(event);
		}

		[[nodiscard]] static constexpr bool can_be_self(flowevent_t event) noexcept
		{
			if (is_modder_detection(event))
			{
				switch (event)
				{
				case MOD_BADEXPLOSION:
				case MOD_EXPBLAME:
				case MOD_SUSSYBOUNTY:
				case MOD_BOUNTYSPAM:
				case MOD_SILENTCHAT:
				case MOD_ID_STAND:
				case MOD_ID_STAND_COLOADING:
				case MOD_ID_OTHER:
				case MOD_RAC:
					return false;
				}
				return true;
			}
			switch (event)
			{
			case MISC_HOSTCHANGE:
			case MISC_SCRIPTHOSTCHANGE:
				return true;
			}
			return is_script_event(event) || is_classification(event) || is_script_launch(event);
		}

		[[nodiscard]] static constexpr bool is_join(flowevent_t event) noexcept
		{
			return event == MISC_JOIN || event == MISC_HISTORYJOIN;
		}

		[[nodiscard]] static constexpr bool is_ac_event(flowevent_t event) noexcept
		{
			switch (event)
			{
			case REPORT_MYSELF_EVENT:
			case NETWORK_CHECK_CODE_CRCS_EVENT:
				return true;
			}
			return false;
		}

		[[nodiscard]] static constexpr bool can_be_blocked(flowevent_t event) noexcept
		{
			if (is_modder_detection(event))
			{
				return event == MOD_EXPBLAME || event == MOD_BADEXPLOSION || event == MOD_ORGNAME;
			}
			if (is_ac_event(event) || is_classification(event) || is_join(event))
			{
				return false;
			}
			switch (event)
			{
			case MISC_HOSTCHANGE:
			case MISC_SCRIPTHOSTCHANGE:
			case MISC_VOTEKICK_ANY:
			case MISC_VOTEKICK_ME:
			case MISC_BOUNTY:
				return false;
			}
			return true;
		}

		[[nodiscard]] static constexpr bool can_be_blocked_from_self(flowevent_t event) noexcept
		{
			return is_script_event(event) || is_script_launch(event);
		}

		[[nodiscard]] static constexpr bool can_notify(flowevent_t event) noexcept
		{
			return event != CHAT_ANY;
		}

		[[nodiscard]] static constexpr bool kick_is_block_join(flowevent_t event) noexcept
		{
			switch (event)
			{
			case MOD_T2IP:
			case MISC_HISTORYJOIN:
				return true;
			}
			return false;
		}

		[[nodiscard]] static constexpr bool is_broad(flowevent_t event) noexcept
		{
			switch (event)
			{
			case SCRIPTED_GAME_EVENT:
			case NE_ANY:
			case SYNCIN_ANY:
			case SYNCIN_ACK_CLONE_CREATE:
			case SYNCIN_ACK_CLONE_UPDATE:
			case SYNCIN_ACK_CLONE_DELETE:
			case SYNCIN_CLONE_CREATE:
			case SYNCIN_CLONE_UPDATE:
			case SYNCIN_CLONE_DELETE:
			case SYNCOUT_CLONE_CREATE:
			case SYNCOUT_CLONE_UPDATE:
			case SYNCOUT_CLONE_DELETE:
			case CHAT_ANY:
			case SCRLAUNCH_ANY:
			case SCRLAUNCH_NOCAT:
			case SCRLAUNCH_FE:
				return true;
			}
			return false;
		}

		[[nodiscard]] static constexpr bool can_react_aggressively(flowevent_t event) noexcept
		{
			switch (event)
			{
			case SE_UNUSUAL:
			case SE_TRIGGERDEFEND:
			case MISC_JOIN:
			case MISC_HOSTCHANGE:
			case MISC_SCRIPTHOSTCHANGE:
			case MISC_LOVELETTER:
				return false;
			}
			if (is_modder_detection(event))
			{
				return event == MOD_HOSTKN_AGGRESSIVE;
			}
			return !is_classification(event);
		}

		// Always implied to be false if is_broad is true.
		[[nodiscard]] static constexpr bool can_react_with_timeout(flowevent_t event) noexcept
		{
			// Some modder detections are applied every tick, so reacting with timeout would not be a timeout, just perma block.
			// Also doesn't make sense for most modder detections, so only allowing it for "that just happened" kind of detections.
			if (is_modder_detection(event))
			{
				return event == MOD_BOUNTYSPAM
					|| event == MOD_SUSSYBOUNTY
					|| event == MOD_BADEXPLOSION
					|| event == MOD_EXPBLAME
					;
			}
			return true;
		}


		[[nodiscard]] static constexpr bool is_classifier(flowevent_t event) noexcept
		{
			if (is_modder_detection(event) || is_custom(event))
			{
				return true;
			}
			if (is_script_launch(event))
			{
				switch (event)
				{
				case SCRLAUNCH_ANY:
				case SCRLAUNCH_NOCAT:
				case SCRLAUNCH_FE:
					return false;
				}
				return true;
			}
			switch (event)
			{
			case NE_INFRING:
			case NE_CAMSHAKE:
			case NE_EXPSPAM:
			case NE_RAGDOLL:
			case NE_SUDDENDEATH:
			case NE_DISARM:
			case SE_CRASH:
			case SE_KICK:
			case SE_INVALID:
			case SE_CEOKICK:
			case SE_STARTFMMISSION_NCEO:
			case SE_GIVECOLLE_NCEO:
			case SE_INFLOAD:
			case SE_DISABLEDRIVINGVEHICLES:
			case SE_FORCECAMFRONT:
			case SE_FREEZE:
			case CHAT_PROFANITYBYPASS:
			case SMS_PROFANITYBYPASS:
				return true;
			}
			return false;
		}

		[[nodiscard]] static constexpr bool is_preemptive(flowevent_t type, uint8_t severity) noexcept
		{
			return severity == 0 && !is_custom(type);
			/*switch (type)
			{
			case MOD_SPOOFEDRIDHARD:
				if (severity != 100)
				{
					return true;
				}
				break;
			}
			return false;*/
		}

		[[nodiscard]] static constexpr bool should_show_all_reactions_in_feature_list(flowevent_t event) noexcept
		{
			switch (event)
			{
			case SE_CRASH:
			case PUP_ANY:
			case SYNCIN_ANY:
			case MOD_ANY:
			case CHAT_AD:
			case SMS_ANY:
			case CLSFN_ANY:
			case SCRLAUNCH_AG:
				return true;
			}
			return false;
		}

		[[nodiscard]] static constexpr bool can_be_visible_to_script(flowevent_t event)
		{
			switch (event)
			{
			case MOD_ID_STAND:
			case MOD_ID_STAND_COLOADING:
				return false;
			}
			return true;
		}
	};
}
