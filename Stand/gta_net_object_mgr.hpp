#pragma once

#include <cstdint>
#include <string>

#include "struct_base.hpp"
#include "rtti_shim.hpp"

#include <soup/Quaternion.hpp>

#include "gta_node_list.hpp"
#include "gta_player.hpp"
#include "NetworkObjectTypes.hpp"
#include "NodeCommonDataOperations.hpp"
#include "script_id.hpp"
#include "SyncType.hpp"
#include "vector.hpp"

using rage::ObjectId;

#pragma pack(push, 1)
namespace rage
{
	class CNetworkSyncDataULBase
	{
	public:
		virtual ~CNetworkSyncDataULBase() = default;

		inline virtual void m_8() { }
		inline virtual void m_10() { }
		inline virtual void m_18() { }
		inline virtual void m_20() { }
		inline virtual void m_28() { }
		inline virtual void m_30() { }
		inline virtual void m_38() { }
		inline virtual void m_40() { }
		inline virtual void m_48() { }
		inline virtual void m_50() { }
		inline virtual void SetCloningFrequency(int player, int frequency) { }

	public:
		uint8_t pad_10h[49];
		uint8_t ownerId;
		uint8_t nextOwnerId;
		uint8_t isRemote;
		uint8_t wantsToDelete : 1; // netobj+76
		uint8_t unk1 : 1;
		uint8_t shouldNotBeDeleted : 1;
		uint8_t pad_4Dh[3];
		uint8_t pad_50h[32];
		uint32_t creationAckedPlayers; // netobj+112
		uint32_t m64;
		uint32_t m68;
		uint32_t m6C;

	public:
		inline bool IsCreationAckedByPlayer(int index)
		{
			return (creationAckedPlayers & (1 << index)) != 0;
		}
	};

	class netBlender
	{
		PAD(0, 0x30);
	};

	class netLinInterpPredictionData
	{
	public:
		struct netSnapshot
		{
			rage::Vector3 m_Position;
			rage::Vector3 m_Velocity;
			uint32_t m_PositionTimestamp;
			uint32_t m_VelocityTimestamp;
			PAD(0x28, 0x30); // R*'s compiler is weird
		};
		static_assert(sizeof(netSnapshot) == 0x30);

		/* 0x00 */ netSnapshot m_SnapshotPast;
		/* 0x30 */ netSnapshot m_SnapshotPresent;
	};
	static_assert(offsetof(netLinInterpPredictionData, m_SnapshotPresent) == 0x30);

	class netBlenderLinInterp : public netBlender
	{
	public:
		/* 0x30 */ netLinInterpPredictionData m_PredictionData;
	};

	class netINodeDataAccessor
	{
	public:
		virtual ~netINodeDataAccessor() = default;
	};

	using PlayerFlags = playerbitset_t;
	using PhysicalPlayerIndex = compactplayer_t;

	struct netSyncDataUnitBase
	{
		virtual ~netSyncDataUnitBase() = default;

		/* 0x08 */ PlayerFlags m_UnsyncedPlayers;
	};

	struct netSyncDataBase
	{
		virtual ~netSyncDataBase() = default;
		virtual void _08() = 0;
		virtual void _10() = 0;
		virtual void _18() = 0;
		virtual void _20() = 0;
		virtual void _28() = 0;
		virtual netSyncDataUnitBase& GetSyncDataUnit(uint32_t i) = 0; // 0x30
		//virtual const netSyncDataUnitBase& GetSyncDataUnit(uint32_t i) const = 0;
	};

	class netSyncTreeTargetObject
	{
	public:
		RAGE_RTTI_SHIM
		virtual ~netSyncTreeTargetObject() = default; // 2802: 0x00 -> 0x30
		virtual netINodeDataAccessor* GetDataAccessor(unsigned dataAccessorType) = 0;
		virtual bool HasSyncData() const = 0;
		virtual netSyncDataBase* GetSyncData() = 0; // 3028: 0x50
		virtual netSyncTree* GetSyncTree() = 0;
		virtual const netSyncDataBase* GetSyncData() const = 0;
		virtual const netSyncTree* GetSyncTree() const = 0; // 2802: 0x30 -> 0x60 (+0x30)
		virtual bool HasGameObject() const = 0;
		virtual netSyncDataBase* CreateSyncData() = 0;
		virtual PlayerFlags GetClonedState() const = 0;
		virtual PlayerFlags GetLastPlayersSyncUpdated() const = 0;
		virtual uint8_t GetMinimumUpdateLevel() const = 0;
		virtual uint8_t GetUpdateLevel(compactplayer_t playerIndex) const = 0;
		virtual const char* GetLogName() const = 0;
		virtual bool CanApplyNodeData() const = 0; // 2802: 0x70 -> 0xA0 (+0x30)
		virtual bool HasNodeDependencies(const netSyncDataNode& dataNode) const = 0; // 2802: 0x78 -> 0xA8 (+0x30)
		virtual void GetAllNodeDependencies(netSyncDataNode& dataNode, netSyncDataNode** nodeDependencies, const unsigned arraySize, unsigned& numDependentNodes) = 0;
	};

	// Why CanPassControl may fail:
	enum
	{
		CPC_SUCCESS,                      // The object can be passed control to the target machine
		CPC_FAIL_NOT_CLONED,              // The object has not been cloned on the target machine
		CPC_FAIL_UNREGISTERING,           // The object is in the process of unregistering
		CPC_FAIL_BEING_REASSIGNED,        // The object is currently being reassigned
		CPC_FAIL_IS_CLONE,                // The object is not controlled by the local machine
		CPC_FAIL_IS_MIGRATING,            // The object is in the process of migrating to another machine
		CPC_FAIL_PERSISTANT_OWNER,        // The object has been flagged to remain owned by the local machine
		CPC_FAIL_PENDING_CLONE_OR_REMOVE, // The object is in the process of being cloned or removed on another machine
		CPC_FAIL_NO_SYNC_DATA,			  // The object has no sync data
		CPC_FAIL_USER                     // Start of any derived class result codes
	};
	enum
	{
		CPC_FAIL_NO_GAME_OBJECT = CPC_FAIL_USER, // The object has no game object
		CPC_FAIL_SPECTATING,                     // Can't pass control of script objects to spectating players
		CPC_FAIL_NOT_SCRIPT_PARTICIPANT,         // Target player is not a participant of the script that created the object
		CPC_FAIL_DEBUG_NO_PROXIMITY,             // Proximity migrations have been disabled in rag
		CPC_FAIL_DEBUG_NO_OWNERSHIP_CHANGE,      // All migrations have been disabled in rag
		CPC_FAIL_PLAYER_NO_ACCEPT,               // The player state is preventing control passing
		CPC_FAIL_PROXIMITY_TIMER,                // The proximity migration timer is active
		CPC_FAIL_SCRIPT_TIMER,                   // The script migration timer is active
		CPC_FAIL_DELETION_TIMER,                 // The object has no game object
		CPC_FAIL_OVERRIDING_CRITICAL_STATE,      // The object is having critical state overridden on remote machines
		CPC_FAIL_IN_CUTSCENE,                    // The object is in a cutscene
		CPC_FAIL_NOT_ADDED_TO_WORLD,             // The game object has not been added to the world yet
		CPC_FAIL_REMOVING_FROM_WORLD,            // The game object is being removed from the world
		CPC_FAIL_PICKUP_PLACEMENT,               // Pickup placement fail
		CPC_FAIL_PICKUP_PLACEMENT_DESTROYED,     // Pickup placement being destroyed
		CPC_FAIL_PHYSICAL_ATTACH,                // Physically attached object can only migrate with what they are attached to
		CPC_FAIL_PED_TASKS,                      // One or more ped tasks prevent the object migrating
		CPC_FAIL_IS_PLAYER,                      // Players cannot migrate to other machines
		CPC_FAIL_IS_TRAIN,                       // Trains do not migrate to other machines
		CPC_FAIL_VEHICLE_OCCUPANT,               // One of more vehicle occupants cannot be passed
		CPC_FAIL_VEHICLE_OCCUPANT_CLONE_STATE,   // The clone state is not equal between the vehicle and one or more occupants
		CPC_FAIL_VEHICLE_OCCUPANT_SEAT_STATE_UNSYNCED, // One or more peds last in one of the vehicle seats have not synced their vehicle state
		CPC_FAIL_VEHICLE_DRIVER_CANT_MIGRATE,	 // The driver of the vehicle cannot migrate
		CPC_FAIL_VEHICLE_RESPOT,                 // The vehicle is in the process of being respotted
		CPC_FAIL_VEHICLE_COMPONENT_IN_USE,       // One or more vehicle components are in use
		CPC_FAIL_VEHICLE_PLAYER_DRIVER,          // The vehicle is being driven by a player
		CPC_FAIL_VEHICLE_TASK,					 // The vehicle's AI task prevented migration
		CPC_FAIL_CRITICAL_STATE_UNSYNCED,        // The critical state of the object is not properly synced 
		CPC_FAIL_BEING_TELEPORTED,               // The object is being teleported
		CPC_FAIL_SCHEDULED_FOR_CREATION,		 // The vehicle is pending creation
		CPC_FAIL_RESPAWN_IN_PROGRESS,		     // The network respawn event is in progress
		CPC_FAIL_OWNERSHIP_TOKEN_UNSYNCED,	     // The ownership token has not been synced by all players
		CPC_FAIL_VEHICLE_HAS_SCHEDULED_OCCUPANTS,// The vehicle has occupants pending
		CPC_FAIL_VEHICLE_IN_ROAD_BLOCK,          // The vehicle is in a road block
		CPC_FAIL_VEHICLE_ATTACHED_TO_CARGOBOB,   // The vehicle is attached to the cargobob helicopter by rope
		CPC_FAIL_PHYSICAL_ATTACH_STATE_MISMATCH, // The attachment state of the entity does not match that dictated by network updates
		CPC_FAIL_WRECKED_VEHICLE_IN_AIR,		 // The vehicle is wrecked and in the air, prevent CPC until on the ground
		CPC_FAIL_VEHICLE_GADGET_OBJECT,			 // The object is a part of a vehicle gadget and so should remain under the ownership of the player controlling the parent vehicle.
		CPC_PICKUP_WARPING,						 // The pickup is warping to an accessible location
		CPC_PICKUP_HAS_PLACEMENT,				 // The pickup has a pickup placement
		CPC_PICKUP_COMING_TO_REST,			     // The pickup is coming to rest
		CPC_FAIL_PED_GROUP_UNSYNCED,             // The ped's group is not synced with the player being passed control to
		CPC_FAIL_REMOVE_FROM_WORLD_WHEN_INVISIBLE, // The object has the bRemoveFromWorldWhenNotVisible flag set so dont pass ownership
		CPC_PICKUP_ATTACHED_TO_LOCAL_PED,		 // The pickup is attached to a local ped
		CPC_FAIL_CONCEALED,						 // Physical object is concealed
		CPC_FAIL_ATTACHED_CHILD_NOT_CLONED,	 // If any of the networked child attachments are not cloned
		CPC_FAIL_TRAIN_ENGINE_LOCAL,			// Can't pass of train that has an engine that is locally owned
	};

	class netObject : public netSyncTreeTargetObject
	{
	public:
		virtual void m_88() = 0;
		virtual CEntity* GetEntity() = 0; // 2802: 0x90 -> 0xC0 (+0x30)
		virtual void m_98() = 0;
		virtual void m_A0() = 0;
		virtual void SetGlobalFlag(unsigned int flag, bool value) = 0; // 0xA8
		virtual int GetObjectFlags() = 0; // 0xA8
		virtual void m_B0() = 0;
		virtual void m_B8() = 0;
		virtual void m_C0() = 0;
		virtual void m_C8() = 0;
		virtual void m_D0() = 0;
		virtual int GetSyncFrequency() = 0; // 0xD8
		virtual void m_E0() = 0;
		virtual void m_E8_added_in_1_69() = 0;
		virtual void m_E8() = 0;
		virtual void m_F0() = 0;
		virtual void m_F8() = 0;
		virtual void m_100() = 0;
		virtual void Update() = 0;
		virtual void m_110() = 0;
		virtual void m_118() = 0;
		virtual void m_120() = 0;
		virtual void m_128() = 0;
		virtual void m_130() = 0;
		virtual void m_138() = 0;
		virtual void m_140() = 0;
		virtual void m_148() = 0;
		virtual void m_150() = 0;
		virtual void m_158() = 0;
		virtual void m_160() = 0;
		virtual bool CanPassControl(const netPlayer& player, int migrationType, unsigned* resultCode) = 0;
		virtual void m_170() = 0;
		virtual void m_178() = 0;
		virtual void m_180() = 0;
		virtual void m_188() = 0;
		virtual void m_190() = 0;
		virtual void m_198() = 0;
		virtual void m_1A0() = 0;
		virtual void m_1A8() = 0;
		virtual void m_1B0() = 0;
		virtual void m_1B8() = 0;
		virtual void m_1C0() = 0;
		virtual void m_1C8() = 0;
		virtual void m_1D0() = 0;
		virtual void m_1D8() = 0;
		virtual void m_1E0() = 0;
		virtual void m_1E8() = 0;
		virtual void m_1F0() = 0;
		virtual void m_1F8() = 0;
		virtual void m_200() = 0;
		virtual void m_208() = 0;
		virtual void m_210() = 0;
		virtual void m_218() = 0;
		virtual void m_220() = 0;
		virtual void m_228() = 0;
		virtual void m_230() = 0;
		virtual void m_238() = 0;
		virtual void m_240() = 0;
		virtual void m_248() = 0;
		virtual void m_250() = 0;
		virtual void m_258() = 0;
		virtual void m_260() = 0;
		virtual void m_268() = 0;
		virtual void m_270() = 0;
		virtual void m_278() = 0;
		virtual void m_280() = 0;
		virtual void m_288() = 0;
		virtual void m_290() = 0;
		virtual void m_298() = 0;
		virtual void m_2A0() = 0;
		virtual void m_2A8() = 0;
		virtual void m_2B0() = 0;
		virtual void m_2B8() = 0;
		virtual rage::Vector3 GetPosition() = 0;
		virtual void m_2C8() = 0;
		virtual void m_2D0() = 0;
		virtual void m_2D8() = 0;
		virtual void m_2E0() = 0;
		virtual void m_2E8() = 0;
		virtual void m_2F0() = 0;
		virtual void m_2F8() = 0;
		virtual void m_300() = 0;
		virtual void m_308() = 0;
		virtual void m_310() = 0;
		virtual void m_318() = 0;
		virtual void m_320() = 0;
		virtual void m_328() = 0;
		virtual void m_330() = 0;
		virtual void m_338() = 0;
		virtual void m_340() = 0;
		virtual void m_348() = 0;
		virtual void m_350() = 0;
		virtual void m_358() = 0;
		virtual void m_360() = 0;
		virtual void m_368() = 0;
		virtual void m_370() = 0;
		virtual void m_378() = 0;
		virtual void m_380() = 0;
		virtual void m_388() = 0;
		virtual void m_390() = 0;
		virtual void m_398() = 0;
		virtual void SetIsVisible(bool isVisible, const char* reason, bool bNetworkUpdate = false) = 0; // call to can be found with "SetPhysicalGameStateData"
		virtual void m_3A0() = 0;
		virtual void m_3A8() = 0;
		virtual void m_3B0() = 0;
		virtual void m_3B8() = 0;
		virtual void UpdateLocalVisibility() = 0; // function contains "Cached visibility"

		static const unsigned LOCALFLAG_UNREGISTERING = BIT(0); // if set this object has been unregistered and is cleaning itself up
		static const unsigned LOCALFLAG_NOCLONE = BIT(1); // if set this object is not cloned on other machines
		static const unsigned LOCALFLAG_BEINGREASSIGNED = BIT(2); // set when an object is ownerless and on the reassign list
		static const unsigned LOCALFLAG_NOREASSIGN = BIT(3); // if set this object will not be reassigned when a player leaves
		static const unsigned LOCALFLAG_USER = 4;

		// global flags used by network objects
		static const unsigned GLOBALFLAG_PERSISTENTOWNER = BIT(0); // disallows proximity or other common ownership changes to this object - ownership can only be altered via explicit means (usually script commands)
		static const unsigned GLOBALFLAG_CLONEALWAYS = BIT(1); // forces cloning of this object on all players
		static const unsigned GLOBALFLAG_USER = 2;

		/* 0x08 */ NetworkObjectType object_type;
		/* 0x0A */ ObjectId object_id;
	private:
		/* 0x0C */ char pad_0x0C[0x10 - 0x0C];
	public:
		/* 0x10 */ class CNetworkSyncDataULBase* ul;
	private:
		/* 0x18 */ char pad_0x18[0x0049 - 0x18];
	public:
		PhysicalPlayerIndex owner_id; //0x0049
		PhysicalPlayerIndex next_owner_id; //0x004A
		bool is_clone; //0x004B
		uint16_t local_flags; //0x004D
		uint16_t global_flags; //0x004E
		/* 0x50 */ fwEntity* game_obj; // prefer getEntity
		/* 0x58 */ netBlender* m_NetBlender;
		PAD(0x58 + 8, 0x70) uint32_t m_ClonedState;
		/* 0x74 */ uint32_t m_PendingCloningState;
		PAD(0x78, 0xA0);

		[[nodiscard]] const char* getTypeName() const
		{
			return NetworkObjectType_toString(object_type);
		}
	};
	static_assert(sizeof(netObject) == 0xA0);

	class netObjectMgrBase
	{
	public:
		class atDNetObjectNode : public atDNode<netObject*>
		{
		};

	public:
		virtual ~netObjectMgrBase() = default;

		virtual void initialize() = 0;
		virtual void shutdown() = 0;
		virtual void update(bool) = 0;

		virtual void addEntity(void*, void*) = 0;
		virtual void unregisterNetworkObject_(netObject* object, bool force) = 0;
		virtual void unregisterNetworkObject(netObject* object, int reason, bool force1, bool force2) = 0;

		virtual void changeOwner(netObject* object, CNetGamePlayer* player, int migrationType) = 0;

		virtual void handleJoiningPlayer(CNetGamePlayer* player) = 0;
		virtual void handleLeavingPlayer(CNetGamePlayer* player) = 0;

		virtual void _0x50(CNetGamePlayer* player) = 0;
		virtual void _0x58() = 0;

		virtual void registerNetworkObject(netObject* object) = 0;

		virtual void packCloneCreate(netObject* object, CNetGamePlayer* player, datBitBuffer* buffer) = 0;
		virtual bool packCloneRemove(netObject* object, CNetGamePlayer* player, bool) = 0;

		virtual void _0x78(netObject* object, void*) = 0;
		virtual void _0x80() = 0;
		virtual void _0x88() = 0;
		virtual const char* _0x90(int) = 0;

		/*virtual bool ProcessCloneCreateData(CNetGamePlayer *source, CNetGamePlayer *target, eNetObjType object_type, std::uint16_t object_id, eNetObjectFlags object_flags, void*, std::uint32_t timestamp) = 0;
		virtual void ProcessCloneCreateAckData(CNetGamePlayer *source, CNetGamePlayer *target, std::uint16_t object_flags, eAckCode ack_code) = 0;

		virtual int ProcessCloneSync(CNetGamePlayer *source, CNetGamePlayer *target, eNetObjType object_type, std::uint16_t object_id, void*, std::uint16_t, std::uint32_t timestamp) = 0;
		virtual void ProcessCloneAckData(CNetGamePlayer *source, CNetGamePlayer *target, void *, std::uint16_t objectId, eAckCode ack_code) = 0;

		virtual void ProcessCloneRemoveData(CNetGamePlayer *source, CNetGamePlayer *target, std::uint16_t object_id, int) = 0;
		virtual void ProcessCloneRemoveAckData(CNetGamePlayer *source, CNetGamePlayer *target, std::uint16_t object_id, eAckCode ack_code) = 0;*/

		virtual void _0xC8() = 0;
	public:
		atDList<atDNetObjectNode> m_objects_owned_by_player[32];

		rage::netObject* find_object_by_id(uint16_t object_id, bool can_delete_be_pending);

		void unregisterById(uint16_t object_id);
	};
}

class CNetworkObjectMgr : public rage::netObjectMgrBase
{
};

class CNetObjGame : public rage::netObject
{
public:
	static const unsigned int GLOBALFLAG_SCRIPTOBJECT = BIT(GLOBALFLAG_USER);	// set if this object is a script object - these objects require extra sync information
	static const unsigned int GLOBALFLAG_CLONEALWAYS_SCRIPT = BIT(GLOBALFLAG_USER + 1);  // set if this object is always cloned on machines running the same script that created the object
	static const unsigned int GLOBALFLAG_CLONEONLY_SCRIPT = BIT(GLOBALFLAG_USER + 2);  // set if this object is only cloned on machines running the same script that created the object
	static const unsigned int GLOBALFLAG_SCRIPT_MIGRATION = BIT(GLOBALFLAG_USER + 3);  // set if this object is only to migrate to machines running the same script that created the object
	static const unsigned int GLOBALFLAG_WAS_SCRIPTOBJECT = BIT(GLOBALFLAG_USER + 4);  // set if this object used to be a script object
	static const unsigned int GLOBALFLAG_RESERVEDOBJECT = BIT(GLOBALFLAG_USER + 5);  // set if this object was created as part of an external population reservation

	static const unsigned int LOCALFLAG_NOLONGERNEEDED = BIT(LOCALFLAG_USER);	// set if this object is a script object and is no longer needed by the script
	static const unsigned int LOCALFLAG_SHOWINCUTSCENE = BIT(LOCALFLAG_USER + 1);	// this object has to remain locally visible in a MP cutscene
	static const unsigned int LOCALFLAG_RESPAWNPED = BIT(LOCALFLAG_USER + 2);	// set if this object will be used for player ped swap, we cant accept ownership while this is set. After the change is done this FLAG is RESET
	static const unsigned int LOCALFLAG_FORCE_SYNCHRONISE = BIT(LOCALFLAG_USER + 3);	// set if the object should be forced to have sync data
	static const unsigned int LOCALFLAG_TELEPORT = BIT(LOCALFLAG_USER + 4);	// set if the object is being teleported and ownership can not change
	static const unsigned int LOCALFLAG_DISABLE_BLENDING = BIT(LOCALFLAG_USER + 5);	// set if the object is has been flagged for disabling network blending
	static const unsigned int LOCALFLAG_REMOVE_POST_TUTORIAL_CHANGE = BIT(LOCALFLAG_USER + 6);	// set if the object has been flagged for removal once a tutorial session state change has finished
	static const unsigned int LOCALFLAG_OWNERSHIP_CONFLICT = BIT(LOCALFLAG_USER + 7);	// set if the object is being unregistered due to an ownership conflict
	static const unsigned int LOCALFLAG_ENTITY_FIXED = BIT(LOCALFLAG_USER + 8);	// set if the object is being fixed so that it must be moved Straight to target.
	static const unsigned int LOCALFLAG_HAS_MIGRATED = BIT(LOCALFLAG_USER + 9);	// set if the object has migrated locally
	static const unsigned int LOCALFLAG_COUNTED_BY_RESERVATIONS = BIT(LOCALFLAG_USER + 10);	// set if the object has been accounted for by the script reservation system
	static const unsigned int LOCALFLAG_DISABLE_PROXIMITY_MIGRATION = BIT(LOCALFLAG_USER + 11);	// set if the object cannot migrate by proximity

	INIT_PAD(rage::netObject, 0xA8);
};
static_assert(sizeof(CNetObjGame) == 0xA8);

class CNetObjProximityMigratable : public CNetObjGame
{
	INIT_PAD(CNetObjGame, 0xC8);
};
static_assert(sizeof(CNetObjProximityMigratable) == 0xC8);

class CNetObjEntity : public CNetObjProximityMigratable
{
	INIT_PAD(CNetObjProximityMigratable, 0x120);
};
static_assert(sizeof(CNetObjEntity) == 0x120);

class CNetObjDynamicEntity : public CNetObjEntity
{
	INIT_PAD(CNetObjEntity, 0x130);
};
static_assert(sizeof(CNetObjDynamicEntity) == 0x130);

class CNetObjPhysical : public CNetObjDynamicEntity
{
	INIT_PAD(CNetObjDynamicEntity, 0x1C4) bool m_isInWater : 1; // CNetObjPed::ProcessLowLodPhysics - 81 BB ? ? 00 00 FF 3F 00 00 0F 84
	PAD(0x1C5, 0x200);
};
static_assert(sizeof(CNetObjPhysical) == 0x200);

class CNetObjPed : public CNetObjPhysical
{
	INIT_PAD(CNetObjPhysical, 0x366) uint8_t m_ClearDamageCount; // 33 D2 80 B9 ? ? 00 00 03
	PAD(0x367, 0x460);
};
static_assert(sizeof(CNetObjPed) == 0x460);

struct IPlayerNodeDataAccessor_vtbl
{
	void(*netINodeDataAccessor_dtor_0)(IPlayerNodeDataAccessor* _this);
	void(*GetPlayerCreateData)(const IPlayerNodeDataAccessor* _this/*, CPlayerCreationDataNode& data*/);
	void(*GetPlayerSectorPosData)(const IPlayerNodeDataAccessor* _this/*, CPlayerSectorPosNode& data*/);
	void(*GetPlayerGameStateData)(IPlayerNodeDataAccessor* _this/*, CPlayerGameStateDataNode& data*/);
	void(*GetPlayerAppearanceData)(IPlayerNodeDataAccessor* _this/*, CPlayerAppearanceDataNode& data*/);
	void(*GetCameraData)(IPlayerNodeDataAccessor* _this/*, CPlayerCameraDataNode& data*/);
	void(*GetPlayerPedGroupData)(IPlayerNodeDataAccessor* _this/*, CPlayerPedGroupDataNode& data*/);
	void(*GetPlayerWantedAndLOSData)(IPlayerNodeDataAccessor* _this/*, CPlayerWantedAndLOSDataNode& data*/);
	void(*GetAmbientModelStreamingData)(IPlayerNodeDataAccessor* _this/*, CPlayerAmbientModelStreamingNode& data*/);
	void(*GetPlayerGamerData)(IPlayerNodeDataAccessor* _this/*, CPlayerGamerDataNode& data*/);
	void(*GetPlayerExtendedGameStateData)(IPlayerNodeDataAccessor* _this/*, CPlayerExtendedGameStateNode& data*/);
	void(*SetPlayerCreateData)(IPlayerNodeDataAccessor* _this/*, const CPlayerCreationDataNode& data*/);
	void(*SetPlayerSectorPosData)(IPlayerNodeDataAccessor* _this/*, const CPlayerSectorPosNode& data*/);
	void(*SetPlayerGameStateData)(IPlayerNodeDataAccessor* _this/*, const CPlayerGameStateDataNode& data*/);
	void(*SetPlayerAppearanceData)(IPlayerNodeDataAccessor* _this/*, CPlayerAppearanceDataNode& data*/);
	void(*SetCameraData)(IPlayerNodeDataAccessor* _this/*, const CPlayerCameraDataNode& data*/);
	void(*SetPlayerPedGroupData)(IPlayerNodeDataAccessor* _this/*, CPlayerPedGroupDataNode& data*/);
	void(*SetPlayerWantedAndLOSData)(IPlayerNodeDataAccessor* _this/*, const CPlayerWantedAndLOSDataNode& data*/);
	void(*SetAmbientModelStreamingData)(IPlayerNodeDataAccessor* _this/*, const CPlayerAmbientModelStreamingNode& data*/);
	void(*SetPlayerGamerData)(IPlayerNodeDataAccessor* _this/*, const CPlayerGamerDataNode& data*/);
	void(*SetPlayerExtendedGameStateData)(IPlayerNodeDataAccessor* _this/*, const CPlayerExtendedGameStateNode& data*/);
};

class IPlayerNodeDataAccessor //: public rage::netINodeDataAccessor
{
public:
	IPlayerNodeDataAccessor_vtbl* __vftable;

	struct PlayerGameStateFlags
	{
		/* 0x00 */ bool controlsDisabledByScript;
		/* 0x01 */ bool newMaxHealthArmour;
		/* 0x02 */ bool hasSetJackSpeed;
		/* 0x03 */ bool isSpectating;
		/* 0x04 */ bool isAntagonisticToPlayer;
		/* 0x05 */ bool neverTarget;
		/* 0x06 */ bool useKinematicPhysics;
		/* 0x07 */ bool inTutorial;
		/* 0x08 */ bool pendingTutorialSessionChange;
		/* 0x09 */ bool respawning;
		/* 0x0A */ bool willJackAnyPlayer;
		/* 0x0B */ bool willJackWantedPlayersRatherThanStealCar;
		/* 0x0C */ bool dontDragMeOutOfCar;
		/* 0x0D */ bool playersDontDragMeOutOfCar;
		/* 0x0E */ bool randomPedsFlee;
		/* 0x0F */ bool everybodyBackOff;
		/* 0x10 */ bool bHasMicrophone;
		/* 0x11 */ bool bInvincible;
		/* 0x12 */ bool bHasMaxHealth;
		/* 0x13 */ bool myVehicleIsMyInteresting;
		/* 0x14 */ uint8_t cantBeKnockedOffBike;
		/* 0x15 */ bool bHelmetHasBeenShot;
		/* 0x16 */ bool notDamagedByBullets;
		/* 0x17 */ bool notDamagedByFlames;
		/* 0x18 */ bool ignoresExplosions;
		/* 0x19 */ bool notDamagedByCollisions;
		/* 0x1A */ bool notDamagedByMelee;
		/* 0x1B */ bool notDamagedBySmoke;
		/* 0x1C */ bool notDamagedBySteam;
		/* 0x1D */ bool playerIsWeird;
		/* 0x1E */ bool noCriticalHits;
		/* 0x1F */ bool disableHomingMissileLockForVehiclePedInside;
		/* 0x20 */ bool ignoreMeleeFistWeaponDamageMult;
		/* 0x21 */ bool lawPedsCanFleeFromNonWantedPlayer;
		/* 0x22 */ bool hasHelmet;
		/* 0x23 */ bool isSwitchingHelmetVisor;
		/* 0x24 */ bool dontTakeOffHelmet;
		/* 0x25 */ bool forceHelmetVisorSwitch;
		/* 0x26 */ bool isPerformingVehicleMelee;
		/* 0x27 */ bool useOverrideFootstepPtFx;
		/* 0x28 */ bool disableVehicleCombat;
		/* 0x29 */ bool treatFriendlyTargettingAndDamage;
		/* 0x2A */ bool useKinematicModeWhenStationary;
		/* 0x2B */ bool dontActivateRagdollFromExplosions;
		/* 0x2C */ bool allowBikeAlternateAnimations;
		/* 0x2D */ bool useLockpickVehicleEntryAnimations;
		/* 0x2E */ bool m_PlayerPreferFrontSeat;
		/* 0x2F */ bool ignoreInteriorCheckForSprinting;
		/* 0x30 */ bool dontActivateRagdollFromVehicleImpact;
		/* 0x31 */ bool swatHeliSpawnWithinLastSpottedLocation;
		/* 0x32 */ bool disableStartEngine;
		/* 0x33 */ bool lawOnlyAttackIfPlayerIsWanted;
		/* 0x34 */ bool disableHelmetArmor;
		/* 0x35 */ bool pedIsArresting;
		/* 0x36 */ bool isScuba;
	};
	static_assert(sizeof(PlayerGameStateFlags) == 0x37);
};
static_assert(sizeof(IPlayerNodeDataAccessor) == 0x08);

struct PlayerCard
{
	enum
	{
		NETSTAT_KD = 1,
		NETSTAT_RANK = 3,
		NETSTAT_MISC_0_A = 8,	 // STAMINA  (0-100)
		NETSTAT_MISC_1_A,		 // STRENGTH (0-100)
		NETSTAT_MISC_2_A,		 // SHOOTING (0-100)
		NETSTAT_MISC_3_A,		 // STEALTH	 (0-100)
		NETSTAT_MISC_4_A,		 // FLYING	 (0-100)
		NETSTAT_MISC_5_A,		 // DRIVING	 (0-100)
		NETSTAT_MISC_6_A,
		NETSTAT_SYNCED_STAT_COUNT = 14,
	};
};

struct CNetObjPlayer : public CNetObjPed, public IPlayerNodeDataAccessor
{
	PAD(sizeof(CNetObjPed) + sizeof(IPlayerNodeDataAccessor), 0x4AB)
	/* 0x4AB.0 */ bool m_visibleOnscreen : 1; // set when the player is visible onscreen, relative to the game camera
	/* 0x4AB.1 */ bool m_UsingFreeCam : 1; // set when the player is using freecam
	/* 0x4AB.2 */ bool m_UsingCinematicVehCam : 1; // set when the player is using cinematic cam
	/* 0x4AB.3 */ bool m_UseKinematicPhysics : 1;
	/* 0x4AB.4 */ bool m_bPendingTutorialSessionChange : 1;
	/* 0x4AB.5 */ bool m_bWaitingPopulationClearedInTutorialSession : 1;
	/* 0x4AB.6 */ bool m_bIsFriendlyFireAllowed : 1;
	/* 0x4AB.7 */ bool m_bIsPassive : 1;
	PAD(0x4AC, 0x528) ObjectId m_SpectatorObjectId; // 0 if not spectating anyone
	PAD(0x528 + 2, 0x530) rage::grcViewport* viewport;
	PAD(0x530 + 8, 0x8C0) float m_stats[PlayerCard::NETSTAT_SYNCED_STAT_COUNT];
};
static_assert(offsetof(CNetObjPlayer, m_stats) == 0x8C0); // 3095: 0x8A0 -> 0x8C0

class CNetObjVehicle : public CNetObjPhysical
{
	INIT_PAD(CNetObjPhysical, 0x770) bool m_PedInSeatStateFullySynced;
};

class CNetObjBike : public CNetObjVehicle
{
};

namespace rage
{
	class netSyncNodeBase
	{
	public:
		virtual ~netSyncNodeBase() = default;
		virtual bool GetIsDataNode() { return false; }
		virtual bool GetIsParentNode() { return false; }
		virtual void InitialiseNode() {}
		virtual void ShutdownNode() {}
		virtual void InitialiseDataNodesInTree(unsigned& dataPosition, unsigned& nodeIndex, unsigned& syncNodeIndex, unsigned& maxNumHeaderBits) {}
		virtual void Write(uint32_t sync_type, unsigned int obj_flags, netObject* obj, datBitBuffer* buf, int, __int64, char, unsigned int* sync_flags) {}

		/* 0x08 */ class netSyncNodeBase* next;
		/* 0x10 */ class netSyncNodeBase* prev;
		/* 0x18 */ class netSyncTree* parent_tree;
		/* 0x20 */ class netSyncParentNode* parent_node;
	};
	static_assert(sizeof(netSyncNodeBase) == 0x28);

	class netSyncDataNode : public netSyncNodeBase
	{
	public:
		virtual ~netSyncDataNode() = 0;
		virtual void vf_38() {};
		virtual void vf_40() {};
		virtual bool shouldTypeIdBeUsedForAppliedTypesFlag() {}; // 0x48
		virtual void vf_50() {};
		virtual void vf_58() {};
		virtual void vf_60() {};
		virtual void vf_68() {};
		virtual void vf_70() {};
		virtual void vf_78() {};
		virtual void vf_80() {};
		virtual void SetDirty(rage::netObject* object) {}; // 0x88
		virtual void vf_90() {};
		virtual void canApply(rage::netObject* object) {}; // 0x98
		virtual void vf_A0() {};
		virtual void vf_A8() {};
		virtual void vf_B0() {};
		virtual void ReadData(rage::datBitBuffer& bitBuffer, rage::netLoggingInterface* pLog) {}; // 0xB8
		virtual void WriteData(rage::netSyncTreeTargetObject* pObj, rage::datBitBuffer& bitBuffer, rage::netLoggingInterface* pLog, bool extractFromObject = true) {}; // 0xC0
		virtual unsigned GetDataSize(rage::netSyncTreeTargetObject& targetObject) const {} // 0xC8

		/* 0x28 */ int32_t id_1;
		/* 0x2C */ int32_t id_2;
		/* 0x30 */ int32_t id_3;
	private:
		/* 0x34 */ char pad_0x34[0x38 - 0x34];
	public:
		/* 0x38 */ uint32_t m_DataIndex;
	private:
		/* 0x3C */ char pad_0x3C[0x40 - 0x3C];
	public:
		/* 0x40 */ bool applies;
	};
	static_assert(sizeof(netSyncDataNode) == 0x41);
}

class CProjectBaseSyncDataNode : public rage::netSyncDataNode
{
	INIT_PAD(rage::netSyncDataNode, 0xA8) ICommonDataOperations* m_dataOperations;
	PAD(0xB0, 0xC0); // every inherited data node has the NodeCommonDataOperations template specialisation here
};
static_assert(sizeof(CProjectBaseSyncDataNode) == 0xC0);

class CSyncDataNodeInfrequent : public CProjectBaseSyncDataNode
{
};

class CSyncDataNodeFrequent : public CProjectBaseSyncDataNode
{
};

class CVehicleCreationDataNode : public CProjectBaseSyncDataNode
{
public:
	/* 0xC0 */ uint32_t m_popType;
	/* 0xC4 */ uint32_t m_randomSeed;
	/* 0xC8 */ hash_t model;
	PAD(0xC8 + 4, 0xE0);
};
static_assert(sizeof(CVehicleCreationDataNode) == 0xE0);

class CAutomobileCreationDataNode : public CProjectBaseSyncDataNode
{
};

class CDoorCreationDataNode : public CProjectBaseSyncDataNode
{
public:
	/* 0xC0 */ hash_t model;
};

class CPedCreationDataNode : public CProjectBaseSyncDataNode
{
public:
	/* 0xC0 */ uint32_t m_popType;
	/* 0xC4 */ hash_t model;
	uint32_t m_randomSeed;
	uint32_t m_maxHealth;
	bool m_inVehicle;
	ObjectId m_vehicleID;
};

class CPickupCreationDataNode : public CProjectBaseSyncDataNode
{
	INIT_PAD(CProjectBaseSyncDataNode, 0x118) hash_t pickup;
	/* 0x11C */ int m_amount;
	/* 0x120 */ hash_t model;
private:
	/* 0x124 */ char pad_0x124[0x128 - 0x124];
public:
	/* 0x128 */ hash_t m_weaponComponents[12];
	/* 0x158 */ uint32_t m_numWeaponComponents;
};
static_assert(offsetof(CPickupCreationDataNode, m_weaponComponents) == 0x128);
static_assert(offsetof(CPickupCreationDataNode, m_numWeaponComponents) == 0x158);

class CPickupPlacementCreationDataNode : public CProjectBaseSyncDataNode
{
	INIT_PAD(CProjectBaseSyncDataNode, 0xF0) hash_t m_pickupHash;
	/* 0xF4 */ uint32_t m_placementFlags;
	/* 0xF8 */ int m_amount;
	/* 0xFC */ hash_t model;
	// script hash is at 0x120
};
static_assert(offsetof(CPickupPlacementCreationDataNode, model) == 0xFC);

class CObjectCreationDataNode : public CProjectBaseSyncDataNode
{
	INIT_PAD(CProjectBaseSyncDataNode, 0x160) hash_t model;
};
#pragma pack(pop)

class IPhysicalNodeDataAccessor
{
public:
	struct ScriptPhysicalFlags
	{
		bool notDamagedByAnything;
		bool dontLoadCollision;
		bool allowFreezeIfNoCollision;
		bool damagedOnlyByPlayer;
		bool notDamagedByBullets;
		bool notDamagedByFlames;
		bool ignoresExplosions;
		bool notDamagedByCollisions;
		bool notDamagedByMelee;
		bool notDamagedByRelGroup;
		bool onlyDamageByRelGroup;
		bool notDamagedBySmoke;
		bool notDamagedBySteam;
		bool onlyDamagedWhenRunningScript;
		bool dontResetDamageFlagsOnCleanupMissionState;
		bool noReassign;
		bool passControlInTutorial;
		bool inCutscene;
		bool ghostedForGhostPlayers;
		bool pickUpByCargobobDisabled;
	};
};

class IPedNodeDataAccessor
{
public:
	static const unsigned int MAX_SYNCED_WEAPON_COMPONENTS = 12; // I believe this has been increased by 1 (from 11) in one of the updates
	static const unsigned int MAX_SYNCED_GADGETS = 3;

	struct ScriptPedPerception
	{
		uint32_t m_HearingRange;
		uint32_t m_SeeingRange;
		uint32_t m_SeeingRangePeripheral;
		float m_CentreOfGazeMaxAngle;
		float m_VisualFieldMinAzimuthAngle;
		float m_VisualFieldMaxAzimuthAngle;
		bool m_bIsHighlyPerceptive;
	};

	struct PedArrestFlags
	{
		bool m_isHandcuffed;
		bool m_canPerformArrest;
		bool m_canPerformUncuff;
		bool m_canBeArrested;
		bool m_isInCustody;
	};

	class CTaskData
	{
	public:
		/* 0x00 */ uint32_t m_TaskType;
		/* 0x04 */ uint32_t m_TaskDataSize;
		/* 0x08 */ uint8_t m_TaskData[/*CTaskInfo::MAX_SPECIFIC_TASK_INFO_SIZE*/ 602];
	};
};

#define MAX_WEAPONS 105
#define MAX_AMMOS 65

class CPedInventoryDataNode : public CSyncDataNodeInfrequent
{
public:
	INIT_PAD(CSyncDataNodeInfrequent, 0x1470) uint32_t m_itemSlots[MAX_WEAPONS];
	/* 0x1614 */ uint32_t m_numItems;
	/* 0x1618 */ uint32_t m_ammoSlots[MAX_AMMOS];
	/* 0x171C */ uint32_t m_ammoQuantity[MAX_AMMOS];
	/* 0x1820 */ uint32_t m_numAmmos;
	/* 0x1824 */ uint8_t m_itemSlotTint[MAX_WEAPONS];
	/* 0x188D */ uint8_t m_itemSlotNumComponents[MAX_WEAPONS];
	PAD(0x188D + MAX_WEAPONS, 0x1DE2) bool m_ammoInfinite[MAX_AMMOS];
	/* 0x1E23 */ bool m_allAmmoInfinite;
};
// CPedInventoryDataNode::Serialise
static_assert(offsetof(CPedInventoryDataNode, m_itemSlots) == 0x1470); // 1st call to serialise_weapon_or_ammo_type
static_assert(offsetof(CPedInventoryDataNode, m_ammoSlots) == 0x1618); // 2nd call to serialise_weapon_or_ammo_type
static_assert(offsetof(CPedInventoryDataNode, m_numItems) == 0x1614); // = MAX_WEAPONS
static_assert(offsetof(CPedInventoryDataNode, m_numAmmos) == 0x1820); // = MAX_AMMOS
static_assert(offsetof(CPedInventoryDataNode, m_itemSlotTint) == 0x1824); // After 1st call to serialise_weapon_or_ammo_type
static_assert(offsetof(CPedInventoryDataNode, m_itemSlotNumComponents) == 0x188D); // After m_itemSlotTint
static_assert(offsetof(CPedInventoryDataNode, m_ammoInfinite) == 0x1DE2); // 2nd call to a2 + 56i64
static_assert(offsetof(CPedInventoryDataNode, m_ammoQuantity) == 0x171C); // Last call to a2 + 104i64
static_assert(offsetof(CPedInventoryDataNode, m_allAmmoInfinite) == 0x1E23); // 1st call to a2 + 56i64

#pragma pack(push, 1)
class CPedAttachDataNode : public CSyncDataNodeInfrequent
{
public:
	/* 0xC0 */ rage::Vector3 m_attachOffset;
	/* 0xD0 */ soup::Quaternion m_attachQuat;
	/* 0xE0 */ ObjectId attached_to;
};
static_assert(sizeof(CPedAttachDataNode) == 0xE0 + 2);

class CPedHealthDataNode : public CSyncDataNodeInfrequent
{
public:
	/* 0xC0 */ uint32_t m_scriptMaxHealth;
	/* 0xC4 */ int32_t m_health; // This should be unsigned, but I do not trust that. Negative health values do happen.
	/* 0xC8 */ uint32_t m_armour;
	/* 0xCC */ uint32_t m_endurance;
	/* 0xD0 */ uint32_t m_scriptMaxEndurance;
	/* 0xD4 */ uint32_t m_weaponDamageHash;
	/* 0xD8 */ uint32_t m_hurtEndTime;
	/* 0xDC */ uint32_t m_weaponDamageComponent;
	/* 0xE0 */ ObjectId m_weaponDamageEntity;
	/* 0xE2 */ bool m_hasMaxHealth;
	/* 0xE3 */ bool m_hasDefaultArmour;
	/* 0xE4 */ bool m_hasMaxEndurance;
	/* 0xE5 */ bool m_killedWithHeadShot;
	/* 0xE6 */ bool m_killedWithMeleeDamage;
	/* 0xE7 */ bool m_hurtStarted;
	/* 0xE8 */ bool m_maxHealthSetByScript;
	/* 0xE9 */ bool m_maxEnduranceSetByScript;
};
static_assert(offsetof(CPedHealthDataNode, m_armour) == 0xC8);
static_assert(offsetof(CPedHealthDataNode, m_maxEnduranceSetByScript) == 0xE9);
static_assert(sizeof(CPedHealthDataNode) == 0xE9 + 1);

class CPedTaskTreeDataNode : public CSyncDataNodeFrequent
{
public:
	/* 0xC0 */ int32_t primary_task;
	/* 0xC4 */ int32_t num_networked_tasks;
};
static_assert(sizeof(CPedTaskTreeDataNode) == 0xC8);
#pragma pack(pop)

class CPedTaskSpecificDataNode : public CSyncDataNodeFrequent
{
public:
	/* 0xC0 */ int m_taskIndex;
	/* 0xC4 */ IPedNodeDataAccessor::CTaskData m_taskData;
};
static_assert(sizeof(CPedTaskSpecificDataNode) == 0x328);

#pragma pack(push, 1)
class CPedMovementGroupDataNode : public CSyncDataNodeInfrequent
{
};

class CPedAIDataNode : public CSyncDataNodeInfrequent
{
};

class CSyncedPedVarData
{
public:
	/* 0x00 */ uint32_t m_components_set_bitflag;
	/* 0x04 */ bool m_components_extra_length;
private:
	char pad[0x3];
public:
	/* 0x08 */ uint32_t m_componentHashes[12];
	int32_t m_drawableId[12]; // 4 bits but if m_components_extra_length then 7 bits 0x1C8
	int32_t m_textureId[12]; // 5 bits
	int32_t m_paletteId[12]; // 4 bits

	[[nodiscard]] bool isComponentSet(const int componentId) const noexcept; // componentNames[12] = { "HEAD", "BEARD", "HAIR", "UPPER", "LOWER", "HAND", "FEET", "TEETH", "ACCESSORIES", "TASK", "DECL", "JBIB" };
	[[nodiscard]] bool isInvalid() const noexcept;
};

class CPedAppearanceDataNode : public CSyncDataNodeInfrequent
{
	INIT_PAD(CSyncDataNodeInfrequent, 0x0F0) CSyncedPedVarData commons;
	PAD(0x0F0 + sizeof(CSyncedPedVarData), 0x1C0);
};
static_assert(sizeof(CPedAppearanceDataNode) == 0x1C0);

class CPedOrientationDataNode : public CSyncDataNodeFrequent
{
};

class CPedMovementDataNode : public CSyncDataNodeFrequent
{
};

class CPedTaskDataNode : public CSyncDataNodeFrequent
{
};

class CMigrationDataNode : public CProjectBaseSyncDataNode
{
};

class CGlobalFlagsDataNode : public CSyncDataNodeInfrequent
{
};

class IDynamicEntityNodeDataAccessor
{
public:
	struct decEnt
	{
		uint32_t Type;
		uint32_t Key;
		union
		{
			float Float;
			bool Bool;
			int Int;
			uint32_t String;
		};
	};
	static_assert(sizeof(decEnt) == 12);

	static const unsigned int MAX_DECOR_ENTRIES = 11;
};

class CDynamicEntityGameStateDataNode : public CSyncDataNodeInfrequent
{
public:
	/* 0xC0 */ uint32_t m_InteriorProxyLoc;
	/* 0xC4 */ bool m_loadsCollisions;
	/* 0xC5 */ bool m_retained;
	PAD(0xC5 + 1, 0xC8) uint32_t m_decoratorListCount;
	/* 0xCC */ IDynamicEntityNodeDataAccessor::decEnt m_decoratorList[IDynamicEntityNodeDataAccessor::MAX_DECOR_ENTRIES];
};
static_assert(sizeof(CDynamicEntityGameStateDataNode) == 0xCC + (sizeof(IDynamicEntityNodeDataAccessor::decEnt) * IDynamicEntityNodeDataAccessor::MAX_DECOR_ENTRIES));
#pragma pack(pop)

class CPhysicalGameStateDataNode : public CSyncDataNodeInfrequent
{
public:
	bool m_isVisible;           // gamestate flag indicating whether the object is visible
	bool m_renderScorched;      // render scorched game state flag
	bool m_isInWater;           // is in water game state flag
	bool m_alteringAlpha;		// the entity is fading out / alpha ramping
	bool m_fadingOut;           // the entity is fading out
	uint32_t m_alphaType;		// the type of alpha ramp the entity is doing
	int16_t m_customFadeDuration;	// A custom max duration for fading
	bool m_allowCloningWhileInTutorial; // if set, the entity won't be stopped from cloning for players who are not in a tutorial
};
static_assert(sizeof(CPhysicalGameStateDataNode) == 0xD0);

#pragma pack(push, 1)
class CPickupScriptGameStateNode : public CSyncDataNodeInfrequent
{
};

class CBikeGameStateDataNode : public CSyncDataNodeInfrequent
{
};

class CBoatGameStateDataNode : public CSyncDataNodeInfrequent
{
};

class CObjectGameStateDataNode : public CSyncDataNodeInfrequent
{
};

class CPlaneGameStateDataNode : public CSyncDataNodeInfrequent
{
};

class CSubmarineGameStateDataNode : public CSyncDataNodeInfrequent
{
};
#pragma pack(pop)

class CTrainGameStateDataNode : public CSyncDataNodeInfrequent
{
public:
	bool     m_IsEngine;                 // is this train an engine or carriage?
	bool	 m_AllowRemovalByPopulation; // used by stationary trains in missions
	bool     m_IsCaboose;                // Is this a caboose
	bool     m_IsMissionTrain;           // Is this a mission created train?
	bool     m_Direction;                // Direction traveling on track
	bool     m_HasPassengerCarriages;    // Does this train have any passenger carriages?
	bool     m_RenderDerailed;           // Should this train be rendered as derailed?
	bool	 m_StopForStations;		     // Stop for stations
	ObjectId m_EngineID;                 // ID of the engine this carriage is attached to (if this train is not an engine)
	int8_t   m_TrainConfigIndex;         // Config index of the entire train this carriage/engine is a part of
	int8_t   m_CarriageConfigIndex;      // Config index of the carriage
	int8_t   m_TrackID;                  // the track the train is on
	float    m_DistFromEngine;           // the distance of this carriage from the engine (0.0 if this is an engine)
	float    m_CruiseSpeed;              // the target cruise speed of the train (desired speed)
	ObjectId m_LinkedToBackwardID;	     // ID of the car linked backward from this train car
	ObjectId m_LinkedToForwardID;	     // ID of the car linked forward from this train car
	unsigned m_TrainState;			     // the train state
	bool	 m_doorsForcedOpen;		     // force the doors open
	bool     m_UseHighPrecisionBlending; // are we using high precision blending on the train
};
static_assert(offsetof(CTrainGameStateDataNode, m_UseHighPrecisionBlending) == 0xE1);

class IVehicleNodeDataAccessor
{
public:
	static const unsigned int MAX_VEHICLE_GADGET_DATA_SIZE = 94;

	struct ScriptVehicleFlags
	{
		bool takeLessDamage;
		bool vehicleCanBeTargetted;
		bool partOfConvoy;
		bool isDrowning;
		bool canBeVisiblyDamaged;
		bool freebies;
		bool lockedForNonScriptPlayers;
		bool respectLocksWhenHasDriver;
		bool lockDoorsOnCleanup;
		bool shouldFixIfNoCollision;
		bool automaticallyAttaches;
		bool scansWithNonPlayerDriver;
		bool disableExplodeOnContact;
		bool canEngineDegrade;
		bool canPlayerAircraftEngineDegrade;
		bool nonAmbientVehicleMayBeUsedByGoToPointAnyMeans;
		bool canBeUsedByFleeingPeds;
		bool allowNoPassengersLockOn;
		bool allowHomingMissleLockOn;
		bool disablePetrolTankDamage;
		bool isStolen;
		bool explodesOnHighExplosionDamage;
		bool rearDoorsHaveBeenBlownOffByStickybomb;
		bool isInCarModShop;
		bool specialEnterExit;
		bool onlyStartVehicleEngineOnThrottle;
		bool dontOpenRearDoorsOnExplosion;
		bool disableTowing;
		bool dontProcessVehicleGlass;
		bool useDesiredZCruiseSpeedForLanding;
		bool dontResetTurretHeadingInScriptedCamera;
		bool disableWantedConesResponse;
	};

	struct GadgetData
	{
		/* 0x00 */ uint32_t Type;
		/* 0x04 */ uint8_t Data[MAX_VEHICLE_GADGET_DATA_SIZE];
		PAD(0x04 + MAX_VEHICLE_GADGET_DATA_SIZE, 0x64);
	};
	static_assert(sizeof(GadgetData) == 0x64);
};

class CVehicleGadgetDataNode : public CSyncDataNodeFrequent
{
public:
	static const unsigned int MAX_NUM_GADGETS = 2;

	/* 0xC0 */ bool m_IsAttachedTrailer;
	PAD(0xC1, 0xD0) rage::Vector3 m_OffsetFromParentVehicle;
	/* 0xE0 */ uint32_t m_NumGadgets;
	/* 0xE4 */ IVehicleNodeDataAccessor::GadgetData m_GadgetData[MAX_NUM_GADGETS];
};
static_assert(sizeof(CVehicleGadgetDataNode) == 0xE4 + (sizeof(IVehicleNodeDataAccessor::GadgetData) * CVehicleGadgetDataNode::MAX_NUM_GADGETS));

class CPedGameStateDataNode : public CSyncDataNodeInfrequent
{
public:
	uint8_t	 m_weaponComponentsTint[IPedNodeDataAccessor::MAX_SYNCED_WEAPON_COMPONENTS];
	uint32_t m_weaponComponents[IPedNodeDataAccessor::MAX_SYNCED_WEAPON_COMPONENTS]; // hashes of weapon components equipped
	uint32_t m_equippedGadgets[IPedNodeDataAccessor::MAX_SYNCED_GADGETS]; // hashes of gadgets equipped

	IPedNodeDataAccessor::ScriptPedPerception  m_PedPerception;          // Ped Perception values

	uint32_t m_arrestState;           // ped arrest state
	uint32_t m_deathState;            // ped death state
	uint32_t m_weapon;                // currently selected weapon type
	uint32_t m_numWeaponComponents;            // the number of weapon components on current equipped weapon
	uint32_t m_numGadgets;            // the number of currently equipped gadgets
	uint32_t m_seat;                  // seat the ped is sitting in in the vehicle
	uint32_t m_nMovementModeOverrideID;
	uint32_t m_LookAtFlags;           // eHeadIkFlags 

	ObjectId m_vehicleID;             // ID of the vehicle this ped is currently in
	ObjectId m_mountID;				  // ID of the mount this ped is currently in
	ObjectId m_custodianID;			  // ID of the player that is and has taken us into custody.
	ObjectId m_LookAtObjectID;        // If looking at an object, ID of object ped is looking at

	uint8_t	 m_weaponObjectTintIndex; // weapon object tint index (coloured weapons)
	uint8_t	 m_vehicleweaponindex;

	uint8_t	 m_cleardamagecount;

	IPedNodeDataAccessor::PedArrestFlags m_arrestFlags;	  // Arrest Data.

	bool m_unk_added_in_1_69;
	
	bool	 m_keepTasksAfterCleanup; // ped keeps his tasks given when he was a script ped
	bool     m_weaponObjectExists;    // is there a weapon object
	bool     m_weaponObjectVisible;   // is the weapon object visible
	bool	 m_weaponObjectHasAmmo;
	bool	 m_weaponObjectAttachLeft;
	bool	 m_doingWeaponSwap;		  // The ped is running a CTaskSwapWeapon
	bool     m_inVehicle;             // is this ped in a vehicle?
	bool     m_hasVehicle;            // does this ped have a vehicle?
	bool	 m_onMount;				  // is this ped on a mount?
	bool	 m_hasCustodianOrArrestFlags; // does this ped have a custodian.
	bool	 m_bvehicleweaponindex;
	bool	 m_bActionModeEnabled;
	bool	 m_bStealthModeEnabled;
	bool	 m_flashLightOn;
	bool	 m_killedByStealth;
	bool	 m_killedByTakedown;
	bool	 m_killedByKnockdown;
	bool	 m_killedByStandardMelee;
	bool	 m_bPedPerceptionModified;
	bool	 m_isLookingAtObject;     // Is looking at an object
	bool	 m_changeToAmbientPopTypeOnMigration;
	bool	 m_isUpright;
	bool	 m_isDuckingInVehicle;
	bool	 m_isUsingLowriderLeanAnims;
	bool	 m_isUsingAlternateLowriderLeanAnims;
	bool     m_HasValidWeaponClipset;             // do we have a valid weapon clip set? Used as part of a hack fix for when pointing while holding a weapon
	bool	 m_createdByConcealedPlayer;
	bool	 m_dontBehaveLikeLaw;
	bool	 m_hitByTranqWeapon;
	bool	 m_permanentlyDisablePotentialToBeWalkedIntoResponse;
	bool	 m_dontActivateRagdollFromAnyPedImpact;
	bool	 m_hasDroppedWeapon;
	bool	 m_canBeIncapacitated;
	bool	 m_bDisableStartEngine;
	bool	 m_disableBlindFiringInShotReactions;
};
//static_assert(offsetof(CPedGameStateDataNode, m_weaponComponentsTint) == 0xC0);
//static_assert(offsetof(CPedGameStateDataNode, m_weaponComponents) == 0xCC);
//static_assert(offsetof(CPedGameStateDataNode, m_equippedGadgets) == 0xFC);
//static_assert(offsetof(CPedGameStateDataNode, m_PedPerception.m_HearingRange) == 0x108);
//static_assert(offsetof(CPedGameStateDataNode, m_PedPerception.m_SeeingRange) == 0x10C);
//static_assert(offsetof(CPedGameStateDataNode, m_weapon) == 0x12C);
//static_assert(offsetof(CPedGameStateDataNode, m_numWeaponComponents) == 0x130);
//static_assert(offsetof(CPedGameStateDataNode, m_numGadgets) == 0x134);
//static_assert(offsetof(CPedGameStateDataNode, m_keepTasksAfterCleanup) == 0x154);
//static_assert(offsetof(CPedGameStateDataNode, m_doingWeaponSwap) == 0x159);

class CVehicleGameStateDataNode : public CSyncDataNodeInfrequent
{
public:
	/* 0xC0 */ float m_HeadlightMultiplier;
	/* 0xC4 */ float m_customPathNodeStreamingRadius;
	/* 0xC8 */ float m_downforceModifierFront;
	/* 0xCC */ float m_downforceModifierRear;
	/* 0xD0 */ uint32_t m_radioStation;
	PAD(0xD0 + 4, 0xDC) uint32_t m_doorsNotAllowedToBeBrokenOff; // bitflags of 1 << door index
	PAD(0xDC + 4, 0x148);
};
static_assert(sizeof(CVehicleGameStateDataNode) == 0x148);

#pragma pack(push, 1)
class CDoorScriptGameStateDataNode : public CSyncDataNodeInfrequent
{
};

class CEntityScriptGameStateDataNode : public CSyncDataNodeInfrequent
{
};

class CObjectScriptGameStateDataNode : public CSyncDataNodeInfrequent
{
};

class CPhysicalScriptGameStateDataNode : public CSyncDataNodeInfrequent
{
public:
	/* 0xC0 */ IPhysicalNodeDataAccessor::ScriptPhysicalFlags m_PhysicalFlags;
};

class CVehicleScriptGameStateDataNode : public CSyncDataNodeInfrequent
{
public:
	/* 0xC0 */ IVehicleNodeDataAccessor::ScriptVehicleFlags m_VehicleFlags;
	PAD(0xC0 + sizeof(IVehicleNodeDataAccessor::ScriptVehicleFlags), 0x178);
};

class CDoorScriptInfoDataNode : public CSyncDataNodeInfrequent
{
};

class CGameScriptObjInfo
{
	void** vftable;
	__int64 pad;
};
static_assert(sizeof(CGameScriptObjInfo) == 0x10);

class CEntityScriptInfoDataNode : public CSyncDataNodeInfrequent
{
	INIT_PAD(CSyncDataNodeInfrequent, 0xC8) CGameScriptObjInfo obj_info;
	CGameScriptId script;
};
#pragma pack(pop)

class CEntityOrientationDataNode : public CSyncDataNodeFrequent
{
};

class CPhysicalVelocityDataNode : public CSyncDataNodeFrequent
{
};

class CPhysicalAttachDataNode : public CSyncDataNodeInfrequent
{
public:
	/* 0xC0 */ bool is_attached;
	/* 0xC2 */ ObjectId attached_to;
	rage::Vector3 m_attachmentOffset;
	soup::Quaternion m_attachmentQuat;
	rage::Vector3 m_attachmentParentOffset;
};
static_assert(offsetof(CPhysicalAttachDataNode, is_attached) == 0xC0);
static_assert(offsetof(CPhysicalAttachDataNode, attached_to) == 0xC2);

#pragma pack(push, 1)
class CPhysicalAngVelocityDataNode : public CSyncDataNodeFrequent
{
};

class CVehicleAngVelocityDataNode : public CPhysicalAngVelocityDataNode
{
	INIT_PAD(CPhysicalAngVelocityDataNode, 0xE8);
};

class CVehicleSteeringDataNode : public CSyncDataNodeFrequent
{
	INIT_PAD(CSyncDataNodeFrequent, 0xC8);
};

class CVehicleControlDataNode : public CSyncDataNodeFrequent
{
	INIT_PAD(CSyncDataNodeFrequent, 0x119) bool m_isSubCar;
	PAD(0x119 + 1, 0x130);
};
#pragma pack(pop)

static const unsigned int NUM_KIT_MOD_SYNC_VARIABLES = 13;

class CVehicleAppearanceDataNode : public CSyncDataNodeInfrequent
{
public:
	static const unsigned int SIZEOF_LICENCE_PLATE = 8;
	static const unsigned MAX_VEHICLE_BADGES = /* DECAL_NUM_VEHICLE_BADGES */ 4;

	INIT_PAD(CSyncDataNodeInfrequent, 0x1E0)
	uint32_t     m_disableExtras;         // bit flags indicating which "extra" car parts are disabled
	uint32_t     m_liveryID;              // ID of the livery for the vehicle
	uint32_t     m_livery2ID;             // ID of the livery2 for the vehicle

	uint32_t		m_allKitMods[NUM_KIT_MOD_SYNC_VARIABLES];
	uint32_t		m_toggleMods;			 // bitfield of the toggle mods that are switched on
	int32_t		m_LicencePlateTexIndex;  // Licence plate texture index.

	uint32_t		m_horntype;

	uint8_t		m_licencePlate[SIZEOF_LICENCE_PLATE]; // Licence Plate

	uint8_t      m_bodyColour1;           // vehicle body colour 1
	uint8_t      m_bodyColour2;           // vehicle body colour 2
	uint8_t      m_bodyColour3;           // vehicle body colour 3
	uint8_t      m_bodyColour4;           // vehicle body colour 4
	uint8_t      m_bodyColour5;           // vehicle body colour 5
	uint8_t      m_bodyColour6;           // vehicle body colour 6
	uint8_t      m_bodyDirtLevel;         // vehicle body dirt level
	uint16_t		m_kitIndex;				 // the kit index that the variation data is using	 
	uint8_t		m_wheelType;			 // wheel type value
	uint8_t		m_wheelMod;				 // wheel mod value
	uint8_t		m_rearWheelMod;			 // rear wheel mod value (for bikes) 
	uint8_t		m_windowTint;			 // window tint
	uint8_t		m_smokeColorR;			 // smoke color R
	uint8_t		m_smokeColorG;			 // smoke color G
	uint8_t		m_smokeColorB;			 // smoke color B
	uint8_t		m_customPrimaryR;		 // custom secondary color R
	uint8_t		m_customPrimaryG;		 // custom secondary color G
	uint8_t		m_customPrimaryB;		 // custom secondary color B
	uint8_t		m_customSecondaryR;		 // custom secondary color R
	uint8_t		m_customSecondaryG;		 // custom secondary color G
	uint8_t		m_customSecondaryB;		 // custom secondary color B
	uint8_t		m_neonColorR;			 // neon color R
	uint8_t		m_neonColorG;			 // neon color G
	uint8_t		m_neonColorB;			 // neon color B

	uint8_t		m_envEffScale;

	bool    m_bVehicleBadgeData[MAX_VEHICLE_BADGES];
	bool	m_bWindowTint;			 // has a window tint
	bool	m_bSmokeColor;			 // has a smoke color
	bool	m_VehicleBadge;
	bool	m_wheelVariation0;
	bool	m_wheelVariation1;
	bool	m_customPrimaryColor;
	bool	m_customSecondaryColor;
	bool	m_hasLiveryID;
	bool	m_hasLivery2ID;
	bool    m_hasDifferentRearWheel; // has a rear wheel that might have a different type (bikes)
	bool	m_neonOn;
	bool	m_neonLOn;
	bool	m_neonROn;
	bool	m_neonFOn;
	bool	m_neonBOn;
	bool    m_neonSuppressed;
	PAD(0x264, 0x270);
};
static_assert(sizeof(CVehicleAppearanceDataNode) == 0x270);
static_assert(offsetof(CVehicleAppearanceDataNode, m_bodyDirtLevel) == 0x23A);
static_assert(offsetof(CVehicleAppearanceDataNode, m_wheelType) == 0x23E);
static_assert(offsetof(CVehicleAppearanceDataNode, m_wheelMod) == 0x23F);
static_assert(offsetof(CVehicleAppearanceDataNode, m_neonOn) == 0x25D);
//static_assert(offsetof(CVehicleAppearanceDataNode, m_neonSuppressed) == 0x260); // not quite aligned correctly

#pragma pack(push, 1)
class CVehicleDamageStatusDataNode : public CSyncDataNodeInfrequent
{
	INIT_PAD(CSyncDataNodeInfrequent, 0x130);
};

class CPedComponentReservationDataNode : public CSyncDataNodeInfrequent
{
};

class CVehicleComponentReservationDataNode : public CSyncDataNodeInfrequent
{
	INIT_PAD(CSyncDataNodeInfrequent, 0x108);
};

class CPhysicalHealthDataNode : public CSyncDataNodeInfrequent
{
};

class CHeliHealthDataNode : public CPhysicalHealthDataNode
{
};

class CVehicleHealthDataNode : public CSyncDataNodeInfrequent
{
	INIT_PAD(CSyncDataNodeInfrequent, 0x168);
};
#pragma pack(pop)

class CVehicleTaskDataNode : public CSyncDataNodeInfrequent
{
public:
	/* 0xC0 */ uint32_t m_taskType;
	/* 0xC4 */ uint32_t m_taskDataSize;
	/* 0xC8 */ uint8_t m_taskData[255];
};
static_assert(offsetof(CVehicleTaskDataNode, m_taskType) == 0xC0);
static_assert(offsetof(CVehicleTaskDataNode, m_taskDataSize) == 0xC4);
static_assert(offsetof(CVehicleTaskDataNode, m_taskData) == 0xC8);

#define MAX_VEHICLE_SEATS 16
#define MAX_TASK_MIGRATION_DATA_SIZE 100

class CVehicleProximityMigrationDataNode : public CProjectBaseSyncDataNode
{
public:
	/* 0xC0 */ uint32_t m_maxOccupants;
	/* 0xC4 */ bool m_hasOccupant[MAX_VEHICLE_SEATS];
	/* 0xD4 */ int16_t m_occupantID[MAX_VEHICLE_SEATS];
	bool m_hasPopType;
	uint32_t m_PopType;
	uint32_t m_status;
	uint32_t m_lastDriverTime;
	/* 0x104 */ bool m_isMoving; // true if pos & vel are set
	PAD(0x105, 0x110) rage::Vector3 m_position;
	/* 0x120 */ int32_t m_packedVelocityX;
	int32_t m_packedVelocityY;
	int32_t m_packedVelocityZ;
	float m_SpeedMultiplier;
	uint16_t m_RespotCounter;
	bool m_hasTaskData;
	uint32_t m_taskType;
	uint32_t m_taskMigrationDataSize;
	uint8_t m_taskMigrationData[MAX_TASK_MIGRATION_DATA_SIZE]; 
};
static_assert(offsetof(CVehicleProximityMigrationDataNode, m_maxOccupants) == 0xC0);
static_assert(offsetof(CVehicleProximityMigrationDataNode, m_hasOccupant) == 0xC4);
static_assert(offsetof(CVehicleProximityMigrationDataNode, m_occupantID) == 0xD4);
static_assert(offsetof(CVehicleProximityMigrationDataNode, m_isMoving) == 0x104);
static_assert(offsetof(CVehicleProximityMigrationDataNode, m_position) == 0x110);
static_assert(offsetof(CVehicleProximityMigrationDataNode, m_packedVelocityX) == 0x120);

#pragma pack(push, 1)
struct Sector
{
	int16_t x;
	int16_t y;
	int16_t z;
};

class CSectorDataNode : public CSyncDataNodeFrequent
{
public:
	/* 0xC0 */ Sector sector;
};

class CSectorPositionDataNode : public CSyncDataNodeFrequent
{
public:
	/* 0xC0 */ rage::Vector3 pos;
};

class CObjectSectorPosNode : public CSyncDataNodeFrequent
{
public:
	/* 0xC0 */ rage::Vector3 pos; // offset could be wrong
};

class CPickupSectorPosNode : public CSyncDataNodeFrequent
{
public:
	/* 0xC0 */ rage::Vector3 pos; // offset could be wrong
};

namespace rage
{
	class netSyncParentNode : public netSyncNodeBase
	{
	public:
		virtual bool read(__int64 syncType, unsigned int syncFlags, rage::datBitBuffer* buffer, rage::netLogStub* logger) { return false; } // 0x38

		/* 0x28 */ uint32_t supported_sync_types;
		PAD(0x28 + 4, 0x30) class netSyncParentNode* parent;
		/* 0x38 */ netSyncNodeBase* first_child;
	};
	static_assert(sizeof(netSyncParentNode) == 0x38 + 8);
}

class CProjectBaseSyncParentNode : public rage::netSyncParentNode
{
	INIT_PAD(rage::netSyncParentNode, 0x60);
};

namespace rage
{
	class netSyncTree
	{
	public:
		virtual ~netSyncTree() = 0;
		virtual void serialize(int sync_type, int object_flags, netObject* obj, __int64 a5, int a6, __int64 a7, char a8, __int64 a9) = 0; // 0x08
		virtual void apply(netObject* obj) = 0; // 0x10
		virtual void vf_18() = 0;
		virtual void vf_20() = 0;
		virtual bool shouldRead(__int64 syncType, unsigned int* pSyncFlags, rage::datBitBuffer* buffer) = 0; // 0x28

		/* 0x008 */ netObject* object;
		/* 0x010 */ netSyncParentNode* root;
		PAD(0x018, 0x020) uint32_t num_create_nodes;
		/* 0x024 */ uint32_t num_update_nodes;
		PAD(0x024 + 4, 0x030) netSyncDataNode* create_nodes[41];
		PAD(0x178, 0x188) netSyncDataNode* update_nodes[31];
		PAD(0x280, 0x4B4) bool m_FailedToWriteAllData;
		/* 0x4B5 */ bool did_read_invalid_data; // added at some point, set to true when CPedTaskSpecificDataNode::Serialise is used by a reader and detects invalid task type

		[[nodiscard]] static bool extendsVehicle(NetworkObjectType object_type) noexcept;
		[[nodiscard]] static bool extendsPedBase(NetworkObjectType object_type) noexcept;
		[[nodiscard]] static bool extendsPhysicalBase(NetworkObjectType object_type) noexcept;
		[[nodiscard]] static bool extendsDynamicEntityBase(NetworkObjectType object_type) noexcept;

		[[nodiscard]] hash_t& _model(NetworkObjectType object_type) noexcept;
		[[nodiscard]] hash_t getModel(NetworkObjectType object_type) noexcept;
		void setModel(NetworkObjectType object_type, hash_t model) noexcept;

		[[nodiscard]] bool sectorApplies() noexcept;
		[[nodiscard]] bool sectorPosApplies(NetworkObjectType object_type) const noexcept;
		[[nodiscard]] bool posApplies(NetworkObjectType object_type) noexcept;
		[[nodiscard]] Sector& _sector() noexcept;
		[[nodiscard]] Vector3& _sectorPos(NetworkObjectType object_type) noexcept;
		static void world2sector(const Stand::v3& pos, Sector& outSector, Stand::v3& outSectorPos) noexcept;
		static Vector3 sector2world(const Sector& sector, const Stand::v3& sector_pos) noexcept;
		[[nodiscard]] Vector3 getPos(NetworkObjectType object_type) noexcept;
		void setPos(NetworkObjectType object_type, const Vector3& pos) noexcept;

		[[nodiscard]] bool attachApplies(NetworkObjectType object_type) const noexcept;
		[[nodiscard]] ObjectId getAttachedTo(NetworkObjectType object_type) const noexcept;
		void setAttachedTo(NetworkObjectType object_type, ObjectId attached_to) noexcept;

		[[nodiscard]] bool isVisible(NetworkObjectType object_type) const noexcept;

		[[nodiscard]] std::string dump(NetworkObjectType object_type, bool is_create);

		[[nodiscard]] const char* castAndGetNodeName(NetworkObjectType object_type, const rage::netSyncDataNode* node) const;
	};
	static_assert(sizeof(netSyncTree) == 0x4B6);
}

class CProjectSyncTree : public rage::netSyncTree
{
};

class CProximityMigrateableSyncTreeBase : public CProjectSyncTree
{
	INIT_PAD(CProjectSyncTree, 0x5E8) CMigrationDataNode m_migrationNode;
	PAD(0x5E8 + sizeof(CMigrationDataNode), 0x6B8) CGlobalFlagsDataNode m_globalFlagsNode;
	PAD(0x6B8 + sizeof(CGlobalFlagsDataNode), 0x780) CSectorDataNode sector;
	PAD(0x780 + sizeof(CSectorDataNode), 0x848) CSectorPositionDataNode sector_position;

	[[nodiscard]] const char* getNodeName(const rage::netSyncDataNode* node) const;
};
static_assert(sizeof(CProximityMigrateableSyncTreeBase) == 0x848 + sizeof(CSectorPositionDataNode));

class CEntitySyncTreeBase : public CProximityMigrateableSyncTreeBase
{
	INIT_PAD(CProximityMigrateableSyncTreeBase, 0xAF8) CEntityScriptInfoDataNode entity_script_info;
	/* 0xC10 */ CEntityScriptGameStateDataNode entity_script_game_state;

	[[nodiscard]] const char* getNodeName(const rage::netSyncDataNode* node) const;
};
static_assert(sizeof(CEntitySyncTreeBase) == 0xC10 + sizeof(CEntityScriptGameStateDataNode));

class CDynamicEntitySyncTreeBase : public CEntitySyncTreeBase
{
	INIT_PAD(CEntitySyncTreeBase, 0xCD8) CDynamicEntityGameStateDataNode dynamic_entity_game_state;

	[[nodiscard]] const char* getNodeName(const rage::netSyncDataNode* node) const;
};
static_assert(sizeof(CDynamicEntitySyncTreeBase) == 0xCD8 + sizeof(CDynamicEntityGameStateDataNode));

class CPedSyncTreeBase : public CDynamicEntitySyncTreeBase
{
	INIT_PAD(CDynamicEntitySyncTreeBase, 0x1020) CPhysicalGameStateDataNode physical_game_state;
	PAD(0x1020 + sizeof(CPhysicalGameStateDataNode), 0x1290) CPedAttachDataNode attach_data;
	PAD(0x1290 + sizeof(CPedAttachDataNode), 0x1558) CPedGameStateDataNode ped_game_state;
	PAD(0x1558 + sizeof(CPedGameStateDataNode), 0x17D8) CPedHealthDataNode health_data;
	PAD(0x17D8 + sizeof(CPedHealthDataNode), 0x18C8) CPedTaskTreeDataNode task_data;
	PAD(0x18C8 + sizeof(CPedTaskTreeDataNode), 0x1A38) CPedTaskSpecificDataNode task_specific[8];

	[[nodiscard]] const char* getNodeName(const rage::netSyncDataNode* node) const;

	[[nodiscard]] CPedTaskSpecificDataNode* getTaskSpecificData(int32_t task_id)
	{
		for (auto& node : task_specific)
		{
			if (node.m_taskData.m_TaskType == task_id)
			{
				return &node;
			}
		}
		return nullptr;
	}
};
static_assert(sizeof(CPedSyncTreeBase) == 0x1A38 + (sizeof(CPedTaskSpecificDataNode) * 8));
static_assert(offsetof(CPedSyncTreeBase, ped_game_state) == 0x1558);

class CPhysicalSyncTreeBase : public CDynamicEntitySyncTreeBase
{
	INIT_PAD(CDynamicEntitySyncTreeBase, 0xFC0) CEntityOrientationDataNode m_orientationNode;
	PAD(0xFC0 + sizeof(CEntityOrientationDataNode), 0x10C0) CPhysicalVelocityDataNode m_velocityNode;
	PAD(0x10C0 + sizeof(CEntityOrientationDataNode), 0x1340) CPhysicalAttachDataNode attach_data;
	PAD(0x1340 + sizeof(CPhysicalAttachDataNode), 0x1460) CPhysicalGameStateDataNode physical_game_state;
	/* 0x1530 */ CPhysicalScriptGameStateDataNode physical_script_game_state;
	PAD(0x1530 + sizeof(CPhysicalScriptGameStateDataNode), 0x1620);

	[[nodiscard]] const char* getNodeName(const rage::netSyncDataNode* node) const;
};
static_assert(sizeof(CPhysicalSyncTreeBase) == 0x1620);
#pragma pack(pop)

class CVehicleSyncTree : public CPhysicalSyncTreeBase
{
public:
	/* 0x1620 */ CProjectBaseSyncParentNode m_controlParentNode;
	/* 0x1680 */ CVehicleCreationDataNode creation_data;
	/* 0x1760 */ CVehicleProximityMigrationDataNode m_vehicleProximityMigrationNode;
	/* 0x1900 */ CVehicleAngVelocityDataNode m_vehicleAngVelocityNode;
	/* 0x19E8 */ CVehicleSteeringDataNode steering_data;
	/* 0x1AB0 */ CVehicleControlDataNode m_vehicleControlNode;
	/* 0x1BE0 */ CVehicleAppearanceDataNode appearance_data;
	/* 0x1E50 */ CVehicleDamageStatusDataNode m_damageStatusNode;
	/* 0x1F80 */ CVehicleGameStateDataNode m_vehicleGameStateNode;
	/* 0x20C8 */ CVehicleHealthDataNode m_vehicleHealthNode;
	/* 0x2230 */ CVehicleScriptGameStateDataNode m_vehicleScriptGameStateNode;
	/* 0x23A8 */ CVehicleComponentReservationDataNode m_componentReservationNode;
	/* 0x24B0 */ CVehicleTaskDataNode vehicle_task_data;
	PAD(0x2678, 0x2680) CVehicleGadgetDataNode gadget_data;

	[[nodiscard]] const char* getNodeName(const rage::netSyncDataNode* node) const;
};
static_assert(offsetof(CVehicleSyncTree, m_controlParentNode) == 0x1620);
static_assert(offsetof(CVehicleSyncTree, creation_data) == 0x1680);
static_assert(offsetof(CVehicleSyncTree, m_vehicleProximityMigrationNode) == 0x1760);
static_assert(offsetof(CVehicleSyncTree, m_vehicleAngVelocityNode) == 0x1900);
static_assert(offsetof(CVehicleSyncTree, steering_data) == 0x19E8);
static_assert(offsetof(CVehicleSyncTree, m_vehicleControlNode) == 0x1AB0);
static_assert(offsetof(CVehicleSyncTree, appearance_data) == 0x1BE0);
static_assert(offsetof(CVehicleSyncTree, m_damageStatusNode) == 0x1E50);
static_assert(offsetof(CVehicleSyncTree, m_vehicleGameStateNode) == 0x1F80);
static_assert(offsetof(CVehicleSyncTree, m_vehicleHealthNode) == 0x20C8);
static_assert(offsetof(CVehicleSyncTree, m_vehicleScriptGameStateNode) == 0x2230); // 2944
static_assert(offsetof(CVehicleSyncTree, m_componentReservationNode) == 0x23A8); // 2944
static_assert(offsetof(CVehicleSyncTree, vehicle_task_data) == 0x24B0); // 2944
static_assert(offsetof(CVehicleSyncTree, gadget_data) == 0x2680); // 2944

#pragma pack(push, 1)
class CAutomobileSyncTreeBase : public CVehicleSyncTree
{
	INIT_PAD(CVehicleSyncTree, 0x2830) CAutomobileCreationDataNode automobile_creation_data;

	[[nodiscard]] const char* getNodeName(const rage::netSyncDataNode* node) const;
};
static_assert(sizeof(CAutomobileSyncTreeBase) == 0x2830 + sizeof(CAutomobileCreationDataNode));
static_assert(offsetof(CAutomobileSyncTreeBase, automobile_creation_data) == 0x2830); // 2944

class CAutomobileSyncTree : public CAutomobileSyncTreeBase
{
};

class CBikeSyncTree : public CVehicleSyncTree
{
};

class CBoatSyncTree : public CVehicleSyncTree
{
};

class CDoorSyncTree : public CPhysicalSyncTreeBase
{
public:
	/* 0x1620 */ CDoorCreationDataNode creation_data;
};
static_assert(sizeof(CDoorSyncTree) == 0x1620 + sizeof(CDoorCreationDataNode));

class CHeliSyncTree : public CAutomobileSyncTreeBase
{
};

class CObjectSyncTree : public CPhysicalSyncTreeBase
{
public:
	/* 0x1620 */ CObjectCreationDataNode creation_data;
	PAD(0x1620 + sizeof(CObjectCreationDataNode), 0x17A0) CObjectSectorPosNode sector_position;
};
static_assert(sizeof(CObjectSyncTree) == 0x17A0 + sizeof(CObjectSectorPosNode));

class CPedSyncTree : public CPedSyncTreeBase
{
	INIT_PAD(CPedSyncTreeBase, 0x3380) CPedCreationDataNode creation_data;
	PAD(0x3380 + sizeof(CPedCreationDataNode), 0x3AB0) CPedAppearanceDataNode appearance_data; // correct b2699
};
static_assert(sizeof(CPedSyncTree) == 0x3AB0 + sizeof(CPedAppearanceDataNode));

class CPickupSyncTree : public CPhysicalSyncTreeBase
{
public:
	/* 0x1620 */ CPickupCreationDataNode creation_data;
	PAD(0x1620 + sizeof(CPickupCreationDataNode), 0x1790) CPickupSectorPosNode sector_position;
};
static_assert(sizeof(CPickupSyncTree) == 0x1790 + sizeof(CPickupSectorPosNode));

class CPickupPlacementSyncTree : public CProximityMigrateableSyncTreeBase
{
public:
	INIT_PAD(CProximityMigrateableSyncTreeBase, 0x920) CPickupPlacementCreationDataNode creation_data;
};
static_assert(offsetof(CPickupPlacementSyncTree, creation_data) == 0x920);

class CPlaneSyncTree : public CVehicleSyncTree
{
};

class CSubmarineSyncTree : public CVehicleSyncTree
{
};

class CTrainSyncTree : public CVehicleSyncTree
{
	INIT_PAD(CVehicleSyncTree, 0x2830) CTrainGameStateDataNode train_game_state;
};
static_assert(sizeof(CTrainSyncTree) == 0x2830 + sizeof(CTrainGameStateDataNode));
#pragma pack(pop)
