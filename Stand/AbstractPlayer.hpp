#pragma once

#include <functional>
#include <optional>

#include <soup/Optional.hpp>
#include <soup/ShortString.hpp>

#include "fwddecl.hpp"
#include "gta_fwddecl.hpp"
#include "gta_player.hpp"
#include "typedecl.hpp"

#include "atStringHash.hpp"
#include "BiString.hpp"
#include "netIpAddress.hpp"
#include "PlayerScope.hpp"

#define HAVE_BREAKUP_KICK false

namespace Stand
{
#pragma pack(push, 1)
	class AbstractPlayer
	{
	public:
		inline static bool is_freemode = false;
		inline static compactplayer_t host = MAX_PLAYERS;
		inline static compactplayer_t prev_host = MAX_PLAYERS;
		inline static time_t last_host_change = 0;
		inline static player_t script_host = MAX_PLAYERS;
		inline static time_t next_orgasm_kick = 0;
		inline static bool performing_love_letter_kick = false;

		inline static std::array<soup::ShortString<16>, MAX_PLAYERS> redundant_names = {};

		[[nodiscard]] static CNetworkPlayerMgr* getNetworkPlayerMgrIfApplicable();
		static void onInit();
		static void doWork();
		static void updateLocalPlayer();

		[[nodiscard]] static int getPlayerCount();
		[[nodiscard]] static std::vector<AbstractPlayer> listAll();
		[[nodiscard]] static std::vector<AbstractPlayer> listOnly(const bool include_self = false, const bool include_friends = false, const bool include_crew_members = false, const bool include_stand_users = false, const bool include_org_members = false, const bool include_modders = false, const bool include_likely_modders = false);
		[[nodiscard]] static std::vector<AbstractPlayer> listExcept(const bool exclude_self = false, const bool exclude_friends = false, const bool exclude_crew_members = false, const bool exclude_stand_users = false, const bool exclude_org_members = false, const bool exclude_modders = false, const bool exclude_likely_modders = false);
		[[nodiscard]] static AbstractPlayer getRandom();
		[[nodiscard]] static AbstractPlayer getHost();
		[[nodiscard]] static AbstractPlayer getScriptHost();

		[[nodiscard]] static constexpr AbstractPlayer invalid() noexcept
		{
			return MAX_PLAYERS;
		}

		[[nodiscard]] static AbstractPlayer fromRockstarId(int64_t rid);
		[[nodiscard]] static AbstractPlayer fromStartupId(const rage::netPeerId peerId);
		[[nodiscard]] static AbstractPlayer fromPeerId(const rage::netPeerId peerId);
		[[nodiscard]] static AbstractPlayer fromEndpointId(uint32_t id);
		[[nodiscard]] static AbstractPlayer fromEndpointAddress(const rage::netSocketAddress& addr);
		[[nodiscard]] static AbstractPlayer getNextHost();

		player_t p;

		constexpr AbstractPlayer(player_t p = MAX_PLAYERS) noexcept
			: p(p)
		{
		}

		operator player_t() const noexcept { return p; }

		[[nodiscard]] bool isValid() const noexcept { return p < MAX_PLAYERS; }
		[[nodiscard]] bool exists() const;

		inline static uint8_t streamer_spoof = 0;
		inline static playerscope_t hide_unfiltered_name = PlayerScope::NOONE;
		[[nodiscard]] static const char* getUserName();

		[[nodiscard]] std::string getName() const; // filtered
		[[nodiscard]] std::string getNameWithFlags() const;
		[[nodiscard]] std::wstring getNameW() const;
		[[nodiscard]] std::wstring getNameW(bool with_flags) const;
		[[nodiscard]] std::u32string getFlagsStrings() const;
		[[nodiscard]] const char* getRedundantName() const; // filtered
		[[nodiscard]] uint32_t getNameHash() const;

		[[nodiscard]] static const char* getMaskName(const std::string& name);
		void spoofName() const;
		void unspoofName() const;
		void forceCommandRecreationIfNotUser() const;
		void updateNameOnBlip() const;
		[[nodiscard]] Vector3 getBlipPos() const;

		[[nodiscard]] std::string getUnoverriddenName() const;
		[[nodiscard]] const char* getOverrideName() const;
		void setOverrideName(const std::string& name) const;
		void clearOverrideName() const;

		[[nodiscard]] std::string getSessionName() const;
		[[nodiscard]] std::wstring getSessionNameForUser() const;

		inline static playerscope_t hide_rid = PlayerScope::NOONE;
		[[nodiscard]] const rage::rlGamerHandle& getGamerHandle() const;
		[[nodiscard]] int64_t getRockstarId() const;
		template <typename T>
		[[nodiscard]] T getRockstarIdForUser() const
		{
			if (isInScope(hide_rid))
			{
				return BiString<T>::fromLang(ATSTRINGHASH("RDCTD"));
			}
			return BiString<T>::fromValue(getRockstarId());
		}

		inline static playerscope_t hide_ip = PlayerScope::NOONE;
		[[nodiscard]] rage::netSocketAddress getAddress() const;
		[[nodiscard]] rage::netSocketAddress getConnectAddress() const;
		[[nodiscard]] rage::netSocketAddress getLanAddress() const;

		[[nodiscard]] std::string getIpAddressForUserA() const;
		[[nodiscard]] std::wstring getIpAddressForUserW() const;

		[[nodiscard]] std::string getConnectedViaForPlayerHelpText() const;
		[[nodiscard]] std::wstring getConnectedViaForPlayerInfoValue() const;

		inline static playerscope_t hide_geoip = PlayerScope::NOONE;
		[[nodiscard]] std::string getCountry() const;
		[[nodiscard]] Label getLanguageName() const;
		[[nodiscard]] int getLanguage() const;

		[[nodiscard]] bool isUsingController() const;

		template <typename T>
		[[nodiscard]] T get() const;
		[[nodiscard]] rage::netPeerId getPeerId() const;

		using host_queue_sort_algo_t = bool(*)(const AbstractPlayer& a, const AbstractPlayer& b);
		static host_queue_sort_algo_t host_queue_sort_algos[4];
		[[nodiscard]] compactplayer_t getHostQueuePosition() const;

		[[nodiscard]] rage::netPeerId getStartupId() const;
		[[nodiscard]] int64_t getAccountId() const;
		[[nodiscard]] uint32_t getMacId() const;
		[[nodiscard]] CNetGamePlayer* getCNetGamePlayer() const;
		[[nodiscard]] CPlayerInfo* getPlayerInfoNoFallback() const;
		[[nodiscard]] CPlayerInfo* getPlayerInfo() const;
		[[nodiscard]] rage::rlGamerInfo* getGamerInfoNoFallback() const;
		[[nodiscard]] rage::rlGamerInfo* getGamerInfo() const;
		[[nodiscard]] rage::netPeerAddress* getPeer() const;
		[[nodiscard]] rage::netEndpoint* getEndpoint() const;
		[[nodiscard]] uint32_t getEndpointId() const;
		[[nodiscard]] int32_t getCxnId(const uint32_t channel_id) const;
		[[nodiscard]] rage::rlClanMembershipData* getActiveClanMembership() const;
		[[nodiscard]] CPed* getCPedPtr() const;
		[[nodiscard]] bool hasPed() const;
		[[nodiscard]] AbstractEntity getPed() const;
		[[nodiscard]] AbstractEntity getVehicle() const;
		[[nodiscard]] AbstractEntity getVehicle(Click& click) const;
		[[nodiscard]] int32_t getWantedLevel() const;
	private:
		void incRemoteWantedLevel(int32_t wanted_level) const;
		void clearWantedLevel() const;
	public:
		void setWantedLevel(int32_t wanted_level) const;
		[[nodiscard]] v3 getPos() const;
		void tickAutoHeal() const;
		void givePickupReward(hash_t reward) const;
		void sendFreeze() const;

		[[nodiscard]] CommandPlayer* getCommand() const noexcept;
		[[nodiscard]] CommandList* getCommandAsList() const noexcept;
		[[nodiscard]] bool satisfiesAnyOf(playerflag_t flags) const noexcept;
		[[nodiscard]] bool satisfiesAnyOf(bool is_friend, bool is_crew_member = false, bool is_stand_user = false, bool is_org_member = false, bool is_modder = false, bool is_likely_modder = false) const;
		[[nodiscard]] bool isFrozen() const;
		void freeze(const bool toggle = true) const;
		void unfreeze() const;
		void kill() const;
		playertype_t getType() const;
		
		void onFlowEvent(flowevent_t event, playertype_t playertype) const;
		void setAttacked() const;
		[[nodiscard]] floweventreaction_t getReactions(flowevent_t event, bool event_is_being_dispatched = true, bool alternative_self_handling = false) const;
		[[nodiscard]] floweventreaction_t getReactions(flowevent_t event, playertype_t playertype, bool event_is_being_dispatched = true, bool alternative_self_handling = false) const;
		void addReactions(floweventreaction_t& reactions, flowevent_t event) const; // prefer to use EventAggregation
		[[nodiscard]] static floweventreaction_t getReactionsRaw(flowevent_t event, playertype_t playertype);
		floweventreaction_t triggerDetection(flowevent_t type, std::string&& extra_data = {}, uint8_t severity = 100) const;
		floweventreaction_t getAndApplyReactionsIn(flowevent_t type, const std::string& extra_data = {}) const;
		void applyReactions(floweventreaction_t reactions) const;
		void applyReactions(floweventreaction_t reactions, flowevent_t event) const;
		void applyReactions(floweventreaction_t reactions, flowevent_t event, const Label& name) const;
		void applyReactions(floweventreaction_t reactions, std::vector<flowevent_t>&& types, Label&& name) const;
		void applyReactions(floweventreaction_t reactions, EventAggregation&& event, uint8_t severity = 100) const;
		void applyReactions(floweventreaction_t reactions, NuancedEventWithExtraData&& event) const;
		void applyReactions(EventLabel&& label, std::vector<flowevent_t>&& types, floweventreaction_t reactions, hash_t format_string_key, uint8_t severity = 100) const;
		void applyReactionsIn(EventLabel&& label, std::vector<flowevent_t>&& types, floweventreaction_t reactions, uint8_t severity = 100) const;
		void applyReactionsIn(floweventreaction_t reactions, flowevent_t type, const std::string& extra_data = {}) const;
		void applyReactionsOut(EventLabel&& label, std::vector<flowevent_t>&& types, floweventreaction_t reactions) const;
		static void augmentReactionToast(std::string& toast, floweventreaction_t reactions);

		[[nodiscard]] bool hasFlag(playerflag_t flag) const;
		void blockIncomingNetEvents(ThreadContext thread_context) const;
		[[nodiscard]] floweventreaction_t getOutgoingSyncReactions() const;
		[[nodiscard]] bool blockingOutgoingSyncs() const;

		[[nodiscard]] bool isInTimeout() const noexcept;
		void putInTimeout(ThreadContext thread_context) const;

		[[nodiscard]] bool areStatsReady() const;
		[[nodiscard]] bool areStatsBusted() const;
		[[nodiscard]] int getRp() const;
		[[nodiscard]] int getRank() const;
		[[nodiscard]] unsigned int getMoney() const;
		[[nodiscard]] unsigned int getWallet() const;
		[[nodiscard]] unsigned int getBank() const;
		[[nodiscard]] float getKd() const;
		[[nodiscard]] int getKills() const;
		[[nodiscard]] int getDeaths() const;
		[[nodiscard]] bool isOtr() const;
		[[nodiscard]] bool isOutOfSight() const;
		[[nodiscard]] bool isInInterior() const;
		[[nodiscard]] bool isHost() const noexcept;
		[[nodiscard]] bool canBecomeHost() const noexcept;
		[[nodiscard]] bool isNextHost() const;
		[[nodiscard]] bool isScriptHost() const;
		[[nodiscard]] bool isFriend() const;
		[[nodiscard]] bool isCrewMember() const;
		[[nodiscard]] bool isStandUser() const;
		[[nodiscard]] bool isStandDevDbg() const;
		[[nodiscard]] bool isRockstarAdmin() const;
		[[nodiscard]] static bool isAnyRockstarAdminInSession();
		[[nodiscard]] bool isTyping() const;
		[[nodiscard]] float* mentalState() const;
		[[nodiscard]] float getMentalState() const;
		[[nodiscard]] int getInteriorId() const;
		[[nodiscard]] int getShopId() const;
		[[nodiscard]] int getApartmentId() const;
		[[nodiscard]] AbstractModel getCurrentVehicleModel() const;
		[[nodiscard]] bool isInVehicle() const;
		[[nodiscard]] bool isUsingRcVehicle() const;
		[[nodiscard]] bool isBlockingRequestControl() const;
		[[nodiscard]] bool isInStore() const; // Shop IDs do not serve for this purpose. Shop ID 45 includes things like the Bunker or LSCM. Likely, wherever the player can "shop". Some stores do not have Shop IDs. 

		[[nodiscard]] bool inSameInterior(AbstractPlayer& other, std::optional<int> id = std::nullopt) const noexcept;

		[[nodiscard]] std::optional<int> getBounty() const;
		[[nodiscard]] bool hasBounty() const;
		[[nodiscard]] int getBountyValue() const;

		[[nodiscard]] bool isInAnOrg() const noexcept;
		[[nodiscard]] AbstractPlayer getBoss() const noexcept;
		[[nodiscard]] bool isBoss() const noexcept;
		[[nodiscard]] int getOrgType() const noexcept;
		[[nodiscard]] bool inOurOrg() const noexcept;
		[[nodiscard]] bool isInMc() const noexcept;
		[[nodiscard]] const char* getOrgName() const noexcept;
		void setOrgName(const char* name) const noexcept;
		void setOrgName(bool is_mc, const char* name) const noexcept;
		[[nodiscard]] int getColourSlot() const noexcept;
		[[nodiscard]] bool isBeast() const;
		[[nodiscard]] bool inCayo() const;
		[[nodiscard]] bool isVisible() const;

		void giveOtr() const;

		void openThirdEye(std::function<void(const AbstractPlayer&, AbstractEntity&, bool)>&& callback) const; // needs a yieldable script thread
		[[nodiscard]] bool openThirdEyeWouldBeUseful() const;
		bool openThirdEyeBegin() const;
		static void openThirdEyeTick(bool needs_cleanup);
		void openThirdEyeCleanup(bool needs_cleanup) const;

		void fiberTeleport(Vector2Plus coords) const;
		void fiberTeleport(const v3& pos) const;
		[[nodiscard]] bool fiberTeleportVehicleTick(const v3& pos) const;
		void waypoint() const;

		[[nodiscard]] soup::Optional<Vector2Plus> getWaypoint() const;

		[[nodiscard]] bool canBeVoteKicked() const;
		[[nodiscard]] bool canBeVoteKickedNaive() const;
		[[nodiscard]] compactplayer_t getNumKickVoters() const;
		[[nodiscard]] static compactplayer_t getVoteKickThreshold();
		[[nodiscard]] static compactplayer_t getVoteKickThresholdUnclamped();
		[[nodiscard]] bool hasEnoughVotesToKick() const;

		// A host kick generally a) sends MsgKickPlayer, b) adds to blacklist, c) removes from session. All diversions from these rules are documented below.
		enum eHostKickType : uint32_t
		{
			VOTE_KICK = 0,
			UNKNOWN_NETWORK_ERROR = 1, // doesn't add to blacklist
			DESYNC = 2, // doesn't send MsgKickPlayer, doesn't add to blacklist
			ADMIN = 4, // doesn't add to blacklist
			ADMIN_BLACKLIST = 5,
			UNKNOWN = 6,
			BATTLEYE_KICK = 7,
			BATTLEYE_BAN = 8, // Temporarily bans them from GTA Online
		};

		[[nodiscard]] static Label getHkReasonString(uint32_t reason);

		void onPreRemove() const;
	private:
		[[nodiscard]] bool kickFlowWait(time_t ms = 1000) const;
	public:
		void kickBail() const;
		void kick(bool use_pre_remove = true, bool can_take_script_host = true) const; // needs a yieldable script thread
		void kickHost(eHostKickType type) const;
		void kickHostNoprekick(eHostKickType type) const;
		void kickScriptHost() const;
		void kickBeUpdate() const;
		void sendBePacket(bool client, const void* data, uint32_t size) const;
		void kickNonHost() const;
		[[nodiscard]] bool checkAggressiveAction() const;
		[[nodiscard]] bool canUseUnblockableKick(bool karma_ourselves = true) const;
		void kickLoveLetter(bool call_pre_remove) const;
		[[nodiscard]] static bool isOrgasmKickOnCooldown();
		void kickOrgasm() const;
		void kickBreakup(int remove_reason = -1) const;
#if HAVE_BREAKUP_KICK
		void kickConfusion() const;
#endif
		void sessionDesync(int remove_reason = -1) const;
		void sessionDesync(const rage::rlGamerHandle& gamer_handle, int remove_reason) const;
		void remoteDesync(const rage::rlGamerHandle& target, int remove_reason = -1) const;
		void desyncBealone() const;
		void desyncBreakup(int remove_reason = -1) const;
		void removeFromPlayerMgr() const;
		void removeFromSessionMgr(int remove_reason = -1) const;
		void kickPickup() const;

		[[nodiscard]] static bool canSendNetEvents(int count = 1);

		void crash() const; // needs script thread context
		void sendCloneDelete(uint16_t netobjid) const;

		[[nodiscard]] bool isInScope(playerscope_t scope) const;

		static void onComm(const std::string& _text, rage::netPeerId recipient_startup_id);
		[[nodiscard]] bool isKnownAdvertiser() const;
		[[nodiscard]] uint8_t getMessageAdLevel(const std::string& text) const;
		[[nodiscard]] uint8_t getMessageAdLevel(std::wstring text) const;
		[[nodiscard]] static void normaliseForAdFilters(std::wstring& str) noexcept;
		[[nodiscard]] static bool checkKnownAggressiveSpammer(const std::wstring& msg) noexcept;
		[[nodiscard]] static bool nameIsCallToAction(const std::wstring& name);
		[[nodiscard]] static bool messageHasCallToAction(const std::wstring& msg);
		[[nodiscard]] static bool messageHasPromiseOfGoodThings(const std::wstring& msg);
		[[nodiscard]] static bool messageIsEmptyOrMiscPromotion(const std::wstring& msg);

		void sendSms(const std::string& text) const; // Receipient must be a friend or member of the same non-Rockstar crew; doesn't work on self. Message must be 1-255 characters.

		void directPacketSend(DirectPacket a0, uint64_t a1 = 0, uint64_t a2 = 0, uint64_t a3 = 0, uint64_t a4 = 0, uint64_t a5 = 0, uint64_t a6 = 0) const;
		void directPacketSendIdentify(DirectPacket a0) const;
		[[nodiscard]] uint8_t getAuthInfo() const;

		[[nodiscard]] v3 getCamPos() const;
		[[nodiscard]] v3 getCamRot() const;
		[[nodiscard]] AbstractPlayer getSpectateTarget() const;
		[[nodiscard]] bool isThisPositionInCameraOrbit(const v3& pos) const;

		[[nodiscard]] float getWeaponDamageModifier() const;
		[[nodiscard]] float getMeleeWeaponDamageModifier() const;

		void giveVehicleRepair() const;
		void giveBounty(int value) const;
	};
#pragma pack(pop)
	static_assert(sizeof(AbstractPlayer) <= sizeof(uintptr_t), "AbstractPlayer is bigger than a pointer/reference");

	inline AbstractPlayer g_player = 0;

	template <>
	inline compactplayer_t AbstractPlayer::get() const
	{
		return *this;
	}
	
	template <>
	inline CNetGamePlayer* AbstractPlayer::get() const
	{
		return getCNetGamePlayer();
	}

	template <>
	inline const rage::netPlayer* AbstractPlayer::get() const
	{
		return reinterpret_cast<const rage::netPlayer*>(getCNetGamePlayer());
	}
}
