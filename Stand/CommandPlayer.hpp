#pragma once

#include "CommandListPlayer.hpp"
#include "CommandPlayerPong.hpp"
#include "dx_common.hpp"
#include "natives_decl.hpp"
#include "PlayerClassifier.hpp"
#include "PlayerProviderSingle.hpp"
#include "player_tags.hpp"
#include "RateLimit.hpp"
#include "RecursiveSpinlock.hpp"
#include "vector.hpp"

#define ASSUME_NO_RID_SPOOFING true

namespace Stand
{
	enum DirectPacketFlags : uint8_t
	{
		DPFLAG_COLOADING  = 1 << 0,
		DPFLAG_SUION      = 1 << 1,
		DPFLAG_DEVDBG     = 1 << 2,
		DPFLAG_DETASDEV   = 1 << 3,
	};

#pragma pack(push, 1)
	class CommandPlayer : public CommandListPlayer
	{
	public:
		static char32_t flag_chars[FLAG_COUNT];
		inline static bool force_flag_update = false;
		inline static const wchar_t* flag_prefix = L" [";
		inline static const wchar_t* flag_suffix = L"]";
		inline static bool classification_flags_on_self = true;

		inline static int64_t trampoline_rid = 0;

		inline static bool blip_names = false;

		explicit CommandPlayer(CommandList* const parent, const compactplayer_t p, bool notify_join);
		void fastTrackRidSessionMismatch();
		void onLeave();

	private:
		const PlayerProviderSingle _player_provider;
	public:
		std::string name; // filtered

		// "joined" is a bit of a heuristic for when we should be fine to interact with this player, e.g. send SEs to them.
		// There's also a global in freemode.c which is set to 99 when they're done joining. ">= 2 && CAM::IS_SCREEN_FADED_OUT())"
		// Don't know how reliable such an approach is, tho.
		bool joined;

		time_t discovery;
		bool streamer_spoof_is_off = false;
		uint64_t peerid;
		int64_t rid;
#if ASSUME_NO_RID_SPOOFING
		bool session_spoofing = false;
#else
		int64_t real_rid = 0;
#endif
		HistoricPlayer* historic_player = nullptr;
#if !ASSUME_NO_RID_SPOOFING
		HistoricPlayer* historic_player_rid = nullptr;
		bool processed_rediscovery = false;
#endif
		bool force_recreate = false;
		bool changed_info = false;
		bool did_rid_spoof_lookup = false;
		bool is_friend = false;
		bool is_crew_member = false;
		time_t last_attack = 0;
		playerflag_t flags = 0;
		compactplayer_t spectating = MAX_PLAYERS;
		time_t invulnerable_since = 0;
		time_t latest_death_at = 0;
		time_t latest_vehicle_entry_at = 0;
		time_t rank_incongruence_since = 0;
		hash_t syncing_as_model = ATSTRINGHASH("mp_m_freemode_01");
		time_t otr_deadline = 0;
		time_t given_otr_since = 0;
		time_t last_typing_at = 0;
		time_t ptfx_timeout = 0;
		time_t cayo_check_timer = 0;
		time_t sound_timeout = 0;
		time_t stun_timeout = 0;
		compactplayer_t love_letter_src = MAX_PLAYERS;
		bool has_left_script = false;
		bool queued_for_deletion = false;
		bool has_silentchat_chance;
		Vector3 last_outofsight_pos;

		std::unordered_set<int64_t> vote_kick_history{};
		union
		{
			bool did_kick_reaction = false;
			bool did_custom_vote_kick /*= false*/;
		};
		bool did_crash_reaction = false;

		time_t last_had_bounty_at = 0;
		int last_bounty_value;

		PlayerClassifier classifier;
		bool aggressive_action_warranted = false;
		[[nodiscard]] flowevent_t getAvailableCustomFlowEvent() const noexcept;

		bool syncing_as_visible = true;

		bool has_waypoint = false;
		//bool is_owner_of_waypoint;
		float waypoint_x;
		float waypoint_y;

		bool attempted_stand_user_handshake = false;
		time_t sent_stand_user_handshake_at = 0;
		uint8_t dp_flags = 0;
		DirectX::SimpleMath::Color minigameColour = { 1.0f, 0.0f, 1.0f, 1.0f };
		[[nodiscard]] bool isStandUser() const noexcept;
		[[nodiscard]] bool isMarkedAsStandUser() const noexcept;
		[[nodiscard]] bool hasStandUserHandshakeFinished() const noexcept;

		bool got_pong_invite = false;
		CommandPlayerPong* pong_command = nullptr;
		size_t my_pong_wins = 0;
		size_t their_pong_wins = 0;

		inline static bool desync_kick_karma = false;
		time_t last_packet = 0;
		[[nodiscard]] bool isTimingOut(time_t delay = 5000) const noexcept;

		std::wstring last_message{};
		time_t last_message_time = 0;
		bool last_message_had_call_to_action = false;
		bool last_message_had_promise_of_good_things = false;
		time_t last_duplicate_message_timestamp = 0;
		int duplicate_message_count = 0;

		RateLimit ptfx_ratelimit{ 1000, 15 };
		RateLimit very_close_ptfx_ratelimit{ 4000, 3 };
		RateLimit sound_ratelimit{ 200, 9 };
		RateLimit uncommon_sound_ratelimit{ 3000, 3 };
		RateLimit explosion_nearby_ratelimit{ 7000, 10 };
		RateLimit notification_ratelimit{ 200, 7 };
		RateLimit sound_se_ratelimit{ 5000, 4 };
		RateLimit int_invite_ratelimit{ 4000, 3 };
		RateLimit ragdoll_request_ratelimit{ 300, 5 };
		RateLimit stun_ratelimit{ 5000, 20 };
		time_t last_htbidlehelp = 0;

		RateLimit create_automobile_ratelimit{ 4000, 20 };
		RateLimit create_ped_ratelimit{ 4000, 30 };
		RateLimit create_ped_ratelimit_upclose{ 20000, 10 };
		RateLimit create_bike_ratelimit{ 4000, 10 };
		RateLimit create_aircraft_ratelimit{ 4000, 9 };
		RateLimit create_object_ratelimit{ 4000, 30 };
		RateLimit create_boat_ratelimit{ 4000, 7 };
		RateLimit uncommon_sync_ratelimit{ 4000, 5 };
		rage::ObjectId last_boat_create_id = NETWORK_INVALID_OBJECT_ID;
		v3 last_boat_create_pos{};
		RateLimit hiresped_ratelimit{ 20000, 10 };
		RateLimit sus_attachment_ratelimit{ 1000, 4 };
		rage::ObjectId last_veh_appearance_update = NETWORK_INVALID_OBJECT_ID;
		RateLimit veh_appearance_ratelimit{ 5000, 60 };

		RateLimit bounty_ratelimit{ 5000, 3 };

		RateLimit chat_message_ratelimit{ 2000, 5 };

		std::wstring getCommandSyntax() const final;

		inline static long long on_click_enable_spectate = 0;
		void onClick(Click& click) final;

		CommandToggleNoCorrelation* getSpectateNinjaCommand() const;
		
		inline static bool on_back_disable_spectate = false;
		void onBack(ThreadContext tc) final;

		[[nodiscard]] bool isSelf() const noexcept;
		[[nodiscard]] AbstractPlayer getPlayer() const;
		[[nodiscard]] std::string getPlayerName() const final;
		[[nodiscard]] CommandName getCommandNamesSuffix() const final;

	private:
		void concludeRidChecks(int64_t real_rid);
		void checkRockstarAdmin(int64_t real_rid) const;
		void processRediscovery(AbstractPlayer p) const;

		void updateCachedVars();
	public:
		void doWork(ThreadContext thread_context);

		[[nodiscard]] CommandPlayerClassifier* getDetectionsList() const noexcept;
		void updateClassification(CommandPlayerClassifier* const detections);
		void updateClassification();
		[[nodiscard]] floweventreaction_t checkClassificationUpdate(PlayerClassifier::Classification& classification, const PlayerClassifier::Classification prev_classification) const;
		void notifyClassificationUpdate(PlayerClassifier::Classification classification, toast_t toast_flags, floweventreaction_t reactions) const;

		[[nodiscard]] bool getFlag(playerflag_t id) const noexcept;
		void setFlag(playerflag_t id, bool val) noexcept;

		void updateFlags();
		void updateFlags(AbstractPlayer p);
	private:
		std::string help_text_connected_via;
		std::string help_text_note;
		[[nodiscard]] std::string getHelpText(AbstractPlayer p);
		[[nodiscard]] std::string getNote() const;

	public:
		[[nodiscard]] floweventreaction_t getReactions(flowevent_t event) const;
		void applyReactions(floweventreaction_t reactions, const NuancedEvent& event, bool batched = false);
		void applyReactions(floweventreaction_t reactions, const NuancedEventWithExtraData& event, bool batched = false);
		void applyReactions(floweventreaction_t reactions, const NuancedEvent& event, Label event_name, const std::string& extra_data = {}, bool batched = false);
		void applyReactions(floweventreaction_t reactions);
		void getAndApplyReactions(AbstractPlayer ap, flowevent_t event, uint8_t severity = 100);
		void getAndApplyReactions(AbstractPlayer ap, flowevent_t event, std::string&& extra_data, uint8_t severity = 100);
		void getAndApplyReactions(AbstractPlayer ap, flowevent_t event, const Label& name);

		void putInTimeout(ThreadContext thread_context) const;
		void setTimeoutState(ThreadContext thread_context, bool state) const;

		void blockIncomingNetEvents(ThreadContext thread_context) const;
		void blockIncomingSyncs(ThreadContext thread_context, bool state = true) const;
		void blockOutgoingSyncs(ThreadContext thread_context) const;

		bool addOrUpdateDetection(const NuancedEvent& event, Label name);
		void removeDetection(const flowevent_t type);

		void directPacketReceived(DirectPacket a0, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint64_t a6);
		void sendPongInfo() const;
		[[nodiscard]] bool eligibleForHostShare() const noexcept;
		inline static bool hostshare_enabled = true;
		inline static bool hostshare_strangers = true;
		inline static bool hostshare_friends = true;
		inline static bool hostshare_crew = true;
	};
#pragma pack(pop)
}
