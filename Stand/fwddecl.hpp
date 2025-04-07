#pragma once

#include <cstdint>

enum eEntityType : uint8_t;

namespace Stand
{
	class PatternBatch;
	class DetourHook;

	class Script;

	class AbstractEntity;
	class AbstractModel;
	class AbstractPlayer;
	struct AbstractRenderer;

	enum name_t : uint8_t;
	enum PedBoneId : int;

	struct Codename;
	class Label;
	struct EventLabel;

	class Click;
	enum ThreadContext : uint8_t;

	class Hotkey;

	class Command;
	class CommandLink;
	class CommandIssuable;
	class CommandPhysical;
	class CommandToggleNoCorrelation;
	class CommandToggle;
	class CommandToggleCustom;
	class CommandSlider;
	class CommandSliderFloat;
	class CommandList;
	class CommandListAction;
	class CommandListActionItem;
	class CommandListConcealer;
	class CommandListNameshare;
	class CommandListPlayer;
	class CommandListSelect;
	class CommandAction;
	class CommandInput;
	class CommandInputRid;
	class CommandInputGamerName;
	class CommandTextslider;
	class CommandTextsliderStateful;
	class CommandSearch;
	class CommandOutfitComponent;
	class CommandOutfitComponentVariation;
	class CommandOutfitProp;
	class CommandOutfitPropVariation;
	class CommandColourCustom;
	class CommandColour;
	class CommandRainbow;
	class CommandDivider;
	class CommandReadonlyValue;

	struct CommandExtraInfo;

	class CommandPlayer;
	class PlayerProvider;
	class CommandPlayerClassifier;
	class CommandPlayerClassifierDetection;

	class CommandHistoricPlayer;
	class CommandListHistoricPlayers;
	class HistoricPlayer;

	class CommandLuaScript;
	struct CommandLuaData;

	struct CommandListFolderItemData;

	class CommandCtxHotkeys;

	struct reScriptTickEvent;
	struct evtChatEvent;
	struct evtPlayerJoinEvent;

	struct FlowEventReactionData;
	struct EventAggregation;
	struct NuancedEvent;
	struct NuancedEventWithExtraData;

	struct Vector2Plus;
	struct Box;
	struct Circle;

	struct TextSettings;
	struct Colour;

	class BackgroundBlur;
	class EphemeralBackgroundBlur;

	struct ErrorInfo;
	struct ExceptionData;

	class Toaster;

	struct TargetingData;

	class RemoteGamer;
	class RemoteStats;
	class ScAccount;

	class UniqueThread;

	struct StringCastable;
	
	class ScriptGlobal;

	enum DirectPacket : uint64_t;

	class LangData;

	struct MatchmakingConfig;

	class CommandSessionBrowserSession;

	template <typename T>
	class Whitelist;

	struct Weapon;

	class ConfigState;

	enum Direction : uint8_t;

	class CommandListSession;
	class CommandListSessionMember;

	enum SessionType : int;
	enum TransitionState : int;

	struct VehicleItem;

	struct ToastConfig;

	class GridItemList;
}
