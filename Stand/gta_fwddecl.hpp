#pragma once

#include <cstdint>

namespace rage
{
	using atHashValue = uint32_t;

	using netSequence = uint16_t;

	template <typename T>
	class atArray;

	template <typename T, size_t S>
	struct atFixedArray;

	class datBitBuffer;
	class datExportBuffer;
	class datImportBuffer;
	class RsonWriter;

	class sysMemAllocator;

	class scriptIdBase;
	class scriptId;
	class scriptHandler;
	class scriptHandlerNetComponent;
	class scriptHandlerObject;
	class scriptHandlerMgr;

	class scrProgram;
	class scrProgramTable;

	class scrThreadContext;
	class scrThread;
	union scrValue;
	class tlsContext;

	class scriptResource;

	class netLoggingInterface;
	class netLogStub;

	class netPlayer;
	class netPlayerMgrBase;

	struct netPeerAddress;
	struct netPeerId;

	class netConnectionManager;
	class netEndpoint;
	class netAddress;
	class netConnection;
	class Cxn;
	struct netTunnelDesc;

	class ReceivedMessageData;

	class netIpAddress;
	class netSocketAddress;

	class netGameEvent;
	class netEventMgr;

	class scrNativeCallContext;
	class scrNativeRegistrationTable;

	struct Vector2;
	struct Vector3;
	struct vector4;
	struct scrVector3;
	struct scrVector4;

	class Matrix34;

	class fwRefAwareBase;
	class fwExtensibleBase;
	class fwExtensionList;
	class fwExtension;
	class fwEntity;
	class fwArchetype;

	class fwModelId;

	class GenericPool;
	class VehiclePool;

	struct rlGamerHandle;
	struct rlPeerInfo;
	class rlGamerInfo;

	class snSession;
	class snPeer;
	class rlSession;
	struct rlSessionInfo;
	struct rlSessionDetail;
	struct rlSessionDetailResponse;
	struct rlMatchingAttributes;
	struct rlScMatchmakingMatchId;

	struct snJoinSessionTask;
	struct snMigrateSessionTask;

	class snEvent;

	struct rlGetGamerStateTask;
	struct rlScMatchmakingFindTask;

	struct rlMetric;

	class rlMatchingFilter;
	struct netStatus;
	struct parTreeNode;
	struct rlRosResult;

	struct phArchetype;
	struct phArchetypeDamp;

	struct phCollider;

	struct rlPresenceEvent;
	struct rlPresenceEventJoinedViaPresence;

	struct rlRosCredentials;
	class rlRosGeoLocInfo;

	template <typename T>
	class sysObfuscated_Mutate;

	struct fwBasePool;
	template <typename T> struct fwPool;

	struct fwScriptGuid;

	struct ClassId;

	struct fwProfanityFilter;

	enum eMigrationType : int32_t;

	struct sysDependency;

	class rlProfileStatsReadResults;
}

class GtaThread;

class CGameScriptId;
class CGameScriptHandler;
class CGameScriptHandlerNetwork;
class CGameScriptHandlerMgr;

class CEntity;
class CDynamicEntity;
class CPhysical;

class CObject;
class CPed;
class CPickup;
class CProjectile;
class CRotaryWingAircraft;
class CTrain;
class CVehicle;
class CPlane;

class CPedFactory;
class CVehicleFactory;

class CVehicleGadget;
class CVehicleWeapon;

class CNetGamePlayer;
class CNetGamePlayerDataMsg;
class CNetworkPlayerMgr;
class CPlayerInfo;
class CNetworkObjectMgr;

class CReplayInterfaceGame;

class CScriptedGameEvent;
class CNetworkIncrementStatEvent;

struct CNetworkTextChat;
struct CMultiplayerChat;

class CNetworkVoice;

class CWeapon;
class CWeaponInfo;
class CWeaponInfoManager;

struct WaterQuad;

class CPedHeadBlendData;

class CBaseModelInfo;
class CVehicleModelInfo;
enum VehicleType : unsigned int;

enum eCarLockState : uint32_t;

class CVehicleModelInfoData;
class CVehicleLayoutInfo;

class CDrivebyWeaponGroup;
class CVehicleDriveByInfo;

struct CVehicleMetadataMgr;

class CVehicleDamage;
class CAircraftDamage;

class CHandlingData;
class CHandlingDataMgr;

namespace rage
{
	class netHttpRequest;
}

class CNetworkSession;
class CBlacklistedGamers;

class CExtraContentManager;
class CNetworkAssetVerifier;

struct EventDispatcher;

class camFrame;
class camBaseObject;
class camBaseObjectMetadata;
class camBaseCamera;
class camScriptedCamera;

class CMiniMapBlip;

class CTextFile;

class CPlayerListMenu;
class InviteMgr;

class CTunables;
class CloudEvent;

namespace rage
{
	class crSkeleton;

	class Mat34V;

	class phBoundComposite;

	class ComponentInfo;
	class ComponentInfoManager;
}

class CProfileSettings;

namespace rage
{
	struct fragType;
}

struct UI3DDrawManager;

namespace rage
{
	struct gameSkeleton;

	struct netTimeSync;

	enum netTaskStatus : int;
	struct FriendsReaderTask;
	
	struct grcViewport;
}

struct sStatData;

struct PopMultiplierArea;

namespace rage
{
	class ropeManager;
	class ropeInstance;
}

struct AcThingData;

// net array

namespace rage
{
	class netArrayHandlerBase;
	class netHostBroadcastDataHandlerBase;

	class netArrayManager;
}

// synctrees

namespace rage
{
	class netSyncTree;
	class netSyncDataNode;
	enum class SyncType : uint32_t;

	class CSyncDataBase;
}

class CPlayerSyncTree;

class CDynamicEntityGameStateDataNode;
class CPedInventoryDataNode;
class CPedTaskSpecificDataNode;
class CPlayerAppearanceDataNode;
class CPlayerGamerDataNode;
class CPlayerGameStateDataNode;
class CVehicleControlDataNode;
class CVehicleCreationDataNode;
class CVehicleGadgetDataNode;
class CVehicleProximityMigrationDataNode;

class IPlayerNodeDataAccessor;
template <class Node, class Accessor>
class NodeCommonDataOperations;

// pathfind

class CNodeAddress;
class CGpsSlot;

// rgsc

namespace rage
{
	class RgscDelegate; // extends rgsc::IRgscDelegateV3 extends rgsc::IRgscDelegateV2 extends rgsc::IRgscDelegateV1 extends rgsc::IRgscDelegate extends rgsc::IRgscUnknown

	struct rlPc;

	struct rlScPresenceAttribute;

	class rlScPresenceMessage;
}

namespace rgsc
{
	struct rlPcProfileManager;
	struct IPlayerManagerV1;
	struct IPresenceManagerV1;
}

// rage::rlClan

namespace rage
{
	class rlClanClient;
	struct rlClanDesc;
	struct rlClanMembershipData;
}

// rage::aiTask

namespace rage
{
	class aiTask;
	class aiTaskTree;
}

class CTask;

// rage::fwEvent

namespace rage
{
	class fwEvent;
}

class CEventNetwork;

class CEventGroupScriptNetwork;

// rage::netMessage

namespace rage
{
	struct netMessage;

	struct snMsgBase;

	struct CMsgTextMessage;
	struct netComplaintMsg;
	struct msgScriptMigrateHost;
	struct snMsgAddGamerToSessionBase;
	struct snMsgJoinRequest;
	struct snMsgRemoveGamersFromSessionCmd;

	struct netIceSessionOffer;
	struct netIceSessionAnswer;
}

struct MsgTextMessage;
struct MsgLostConnectionToHost;

// rage::netObject

namespace rage
{
	class netSyncTreeTargetObject;
	class netObject;
	class netObjectMgrBase;
}

class CNetObjEntity;
class CNetObjPhysical;
struct CNetObjPlayer;
class CNetObjVehicle;

// presence events

namespace rage
{
	class netGamePresenceEvent;
}

class CGamePresenceEventDispatcher;

// rendering

namespace rage
{
	struct fwCustomShaderEffect;

	struct grmShaderGroup;
}

struct CCustomShaderEffectBaseType;
struct CCustomShaderEffectVehicle;

namespace rage
{
	class fwRenderThreadInterface;
}

class CViewportManager;

// files

namespace rage
{
	struct fiPackfile;
	struct fiPackEntry;
}

// scaleform

struct CScaleformStore;
struct ScaleformMovieStruct;

// actionscript

struct GASEnvironment;
struct GASString;
struct GASValue;

// scaleform gfx

struct GFxMovieRoot;
struct GFxMovieView;
struct GFxValue;
