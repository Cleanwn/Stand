#pragma once

#include <functional>
#include <unordered_map>

#include "fwddecl.hpp"
#include "gta_fwddecl.hpp"
#include "natives_decl.hpp"

#include "nullsub.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class AbstractEntity
	{
	public:
		inline static bool player_ped_was_on_foot = true;
		inline static bool player_ped_was_aiming = true;

		inline static std::unordered_map<Entity, std::pair<time_t, std::unordered_map<Hash, std::pair<std::function<void(AbstractEntity&)>, std::function<void(AbstractEntity&)>>>>> control_requests = {};

		static void updatePlayerPed();
		static void updatePlayerVehAndEnt();
		static void updatePlayerEnt();
		static void updatePlayerPedVehAndEnt();
		static void updateCamEnt();
		static void onTick();

		static AbstractEntity get(AbstractEntity) = delete;
		[[nodiscard]] static AbstractEntity get(Entity handle) { return AbstractEntity(handle); }
		[[nodiscard]] static AbstractEntity get(CPhysical* ptr) { return AbstractEntity(ptr); }
		static void get(AbstractEntity&) = delete;
		[[nodiscard]] static AbstractEntity getSmart(int64_t handle_or_ptr);
		[[nodiscard]] static AbstractEntity getNetwork(uint16_t network_id);

		[[nodiscard]] static v3 getEffectiveUserPos();

		[[nodiscard]] static AbstractEntity getPersonalVehicle();

		[[nodiscard]] static AbstractEntity getNearestVisible(std::vector<AbstractEntity>& entites, bool los_check = true);

		[[nodiscard]] static AbstractEntity invalid() { return AbstractEntity(); }

	private:
		CPhysical* ptr;
		Entity handle;

	public:
		constexpr AbstractEntity()
			: ptr(nullptr), handle(INVALID_GUID)
		{
		}

		constexpr AbstractEntity(const AbstractEntity& ent)
			: ptr(ent.ptr), handle(ent.handle)
		{
		}

	private:
		explicit constexpr AbstractEntity(Entity handle)
			: ptr(nullptr), handle(handle)
		{
		}

		explicit constexpr AbstractEntity(CPhysical* ptr)
			: ptr(ptr), handle(INVALID_GUID)
		{
		}

	public:
		static void getAll(const std::function<bool(AbstractEntity&&)>& collector);
		static bool getAllPeds(const std::function<bool(AbstractEntity&&)>& collector);
		static bool getAllVehicles(const std::function<bool(AbstractEntity&&)>& collector);
		static bool getAllObjects(const std::function<bool(AbstractEntity&&)>& collector);
		static void getAllPickups(const std::function<bool(AbstractEntity&&)>& collector);
		static void getAllObjectsIncludingPickups(const std::function<bool(AbstractEntity&&)>& collector);

		static void getAllPeds(std::vector<AbstractEntity>& vec);
		static void getAllVehicles(std::vector<AbstractEntity>& vec);
		static void getAllObjects(std::vector<AbstractEntity>& vec);
		static void getAllPickups(std::vector<AbstractEntity>& vec);

		[[nodiscard]] bool hasHandle() const;
		bool ensureHandle();
	private:
		bool findHandle();
	public:
		[[nodiscard]] bool isHandleEquals(Entity handle);
		// Experimental stuff, not recommended to be used.
		void releaseHandle();
		bool setHandle(Entity desired_handle);
		void setHandleReference(uint8_t reference);

		bool ensurePointer();
	private:
		bool findPointer();
	public:
		bool ensureHandleAndPointer();

		[[nodiscard]] Entity getHandle();
		[[nodiscard]] CPhysical* getPointer();

		[[nodiscard]] CPed* getCPed()
		{
			return reinterpret_cast<CPed*>(getPointer());
		}

		[[nodiscard]] CVehicle* getCVehicle()
		{
			return reinterpret_cast<CVehicle*>(getPointer());
		}

		[[nodiscard]] CObject* getCObject()
		{
			return reinterpret_cast<CObject*>(getPointer());
		}

		[[nodiscard]] CPhysical* getPointerRaw() const noexcept
		{
			return ptr;
		}

		[[nodiscard]] CPed* getCPedRaw() const noexcept
		{
			return reinterpret_cast<CPed*>(ptr);
		}

		[[nodiscard]] void* getNetObjectImpl();

		template <typename T = rage::netObject>
		[[nodiscard]] T* getNetObject()
		{
			return reinterpret_cast<T*>(getNetObjectImpl());
		}

		operator bool() = delete;
		operator Entity();
		operator CPed* ();
		operator CVehicle* ();
		bool operator==(AbstractEntity& b);
		bool operator==(AbstractEntity&& b);
		bool operator!=(AbstractEntity& b);
		void operator=(const AbstractEntity& b);
		[[nodiscard]] std::size_t hash() const noexcept;
		void reset() noexcept;

		// General
		[[nodiscard]] bool isValid() const noexcept;
		[[nodiscard]] bool isFullyUsable();
		[[nodiscard]] bool exists();
		[[nodiscard]] eEntityType getType();
		[[nodiscard]] bool isPed();
		[[nodiscard]] bool isVehicle();
		[[nodiscard]] bool isObject();
		[[nodiscard]] bool isPickup();
		[[nodiscard]] std::string getName(name_t gender);
		[[nodiscard]] std::string getName(bool include_owner = true);
		[[nodiscard]] v3 getPos();
		[[nodiscard]] v3 getVisualPos();
		void setPos(const v3& pos);
		void setOrigin(const v3& pos);
		[[nodiscard]] v3 getRot();
		void setRot(const v3& rot);
		[[nodiscard]] bool isInLineOfSight();
		[[nodiscard]] bool isInLineOfSight(const v3& observer_pos);
		[[nodiscard]] bool isInLineOfSight(const v3& observer_pos, const v3& pos);
		[[nodiscard]] AbstractModel getModel();
		[[nodiscard]] CBaseModelInfo* getModelInfo();
		[[nodiscard]] Hash getModelHash();
		[[nodiscard]] AbstractPlayer getOwner();
		[[nodiscard]] bool isOwner();
		void requestControl(Hash reason, std::function<void(AbstractEntity&)>&& callback, std::function<void(AbstractEntity&)>&& = &nullsub<AbstractEntity&>);

		[[nodiscard]] bool canBeDeleted();
		void removeFromPlaneOfExistence();
		void removeFromPlaneOfExistenceNocheck();
	private:
		void removeFromPlaneOfExistenceRemoveRelationships();
		void removeFromPlaneOfExistenceAfterControlRequest();
	public:
		bool removeFromPlaneOfExistenceForce();
	private:
		bool removeFromPlaneOfExistenceInner();

	public:
		[[nodiscard]] bool isInvulnerable();
		[[nodiscard]] float getHealth();
		void setHealth(float health);
		[[nodiscard]] bool isDead();
		[[nodiscard]] bool isFrozen();
		void freeze();
		void unfreeze();
		[[nodiscard]] Vector3 getDimensions();
		[[nodiscard]] Vector3 getOrigin();
		[[nodiscard]] v3 getOriginOffset();
		[[nodiscard]] Box getBox();
		[[nodiscard]] float airDistance(AbstractEntity& b);
		[[nodiscard]] bool canTarget(const TargetingData& data, bool ped_to_vehicle, bool consider_veh_as_player = false);
		[[nodiscard]] rage::fwExtension* getExtension(uint32_t id);
	private:
		[[nodiscard]] rage::fwExtension* getExtensionInner(uint32_t id) const;
	public:
		void setCanMigrate(bool can_migrate);
		[[nodiscard]] bool getCanMigrate();
		void setAlwaysSynch(bool always_synch);
		[[nodiscard]] uint16_t getNetObjId();
		[[nodiscard]] bool isNetObjId(uint16_t id);
		void giveControl(AbstractPlayer p);
		[[nodiscard]] bool isInWater();
		[[nodiscard]] float getSpeed();

		// Ped Specific
		[[nodiscard]] Vector3 getBonePos(PedBoneId boneId);
		[[nodiscard]] Vector3 getHeadPos();
		[[nodiscard]] bool isAiming();
		[[nodiscard]] bool isAPlayer();
		[[nodiscard]] bool couldBePlayer();
		[[nodiscard]] CPlayerInfo* getPlayerInfo();
		[[nodiscard]] compactplayer_t getPlayerId();
		[[nodiscard]] AbstractPlayer getPlayer();
		[[nodiscard]] float getMaxHealth();
		[[nodiscard]] float getArmour();
		void setMaxHealth(float max_health);
		void setArmour(float armor);
		[[nodiscard]] static float getMaxArmour();
		[[nodiscard]] bool isFriend();
		[[nodiscard]] bool isInAnyVehicle();
		[[nodiscard]] int getSeat(AbstractEntity& veh);
		void giveWeapons(std::vector<hash_t>&& weapons, const bool force = false);
		void giveAllWeapons();
		void removeWeapons(std::vector<hash_t>&& weapons);
		void removeAllWeapons();
		[[nodiscard]] AbstractEntity getCurrentWeaponEntity();
		[[nodiscard]] v3 getCurrentWeaponPos();
		[[nodiscard]] CPedHeadBlendData* getHeadBlendData();
		[[nodiscard]] int getComponentTexture(int component_id);
		[[nodiscard]] int getComponentMaxDrawable(int component_id);
		void ensureHasParachute();
		void giveParachute();
		void randomiseOutfit();
		void godmodeEnable(const bool no_ragdoll = true);
		void godmodeDisable();
		void revive();
		[[nodiscard]] bool isInBubble();
		[[nodiscard]] AbstractEntity getVehicle(bool include_last = false);
		[[nodiscard]] bool isOnGround();
		[[nodiscard]] bool hasHeadroom();
		void setConfigFlag(int flag, bool value);
		void setCombatAttr(int flag, bool value);
		void cage(Hash cage_model = 0);
		void ragdoll();
		[[nodiscard]] Hash getSelectedWeapon();
		void sendMagicBullet(Hash weapon, float damage, bool nullify);
		[[nodiscard]] AbstractEntity getWeapon();
		[[nodiscard]] v3 getWeaponMuzzlePos();
		[[nodiscard]] bool isPassiveMode();
		[[nodiscard]] bool isParachuting();

		// Vehicle Specific
		[[nodiscard]] int getMaxPassengers();
		[[nodiscard]] int getFreeSeat();
		void makePassengersLeave();
		[[nodiscard]] AbstractEntity getPedInSeat(int seat);
		[[nodiscard]] AbstractEntity getDriver();
		[[nodiscard]] bool isAPlayerInAnySeat();
		[[nodiscard]] bool isOwnerOfVehicleAndDriver();
		void setVehicleCanBeDamaged(const bool toggle);
		void repair();
		void wash();
		[[nodiscard]] int getUpgradeValue(int modType);
		[[nodiscard]] int getUpgradeMaxValue(int modType);
		[[nodiscard]] bool isUpgradeApplicable(int modType);
		[[nodiscard]] bool hasUpgradeMax(int modType);
		void applyUpgrade(int modType, int value);
		void applyUpgradeMax(int modType);
		void fullyUpgrade();
		bool applyPerformanceUpgrade();
		bool applyNeonPerformanceUpgrade();
		bool applyPerformanceUpgradeWithSpoiler();
		void downgrade();
		void startEngine();
		[[nodiscard]] bool isVtolOn();
		void toggleVtolOff();
		void destroy();
		void call();
		[[nodiscard]] int getHeadlightsVariation();
		void setHeadlightsVariation(int value);
		[[nodiscard]] int getPrimaryColour();
		void setPrimaryColour(int value);
		[[nodiscard]] int getSecondaryColour();
		void setSecondaryColour(int value);
		[[nodiscard]] bool isUserPersonalVehicle();
		void slingshot();
		void slingshot(Direction direction);
		[[nodiscard]] std::optional<std::vector<uint8_t>> getNextGenRemapping(int modType);
		void makeIndestructible();
		void detachWheel(int wheelIndex);
		void setPlateText(const std::string& text);
		void setPlateText(const char* text);
		void setPlateBasedOnSeed();
		void randomiseSeedAndUpdatePlate();
		[[nodiscard]] bool isOnRoad();
		[[nodiscard]] AbstractPlayer getPersonalVehicleOwner();
		[[nodiscard]] std::pair<Vector3, Vector3> getBombBayPos();
	};
#pragma pack(pop)

	inline AbstractEntity g_player_ped;
	inline AbstractEntity g_player_veh;
	inline AbstractEntity g_player_ent;
	inline bool g_overwriting_cam_ent = false;
	inline AbstractEntity g_cam_ped;
	inline AbstractEntity g_cam_veh;
	inline AbstractEntity g_cam_ent;
	inline AbstractEntity g_preview_ent;
}
