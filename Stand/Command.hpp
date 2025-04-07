#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include <soup/fwd.hpp>
#include <soup/TransientToken.hpp>

#include "fwddecl.hpp"
#include "typedecl.hpp"

#include "CommandFlags.hpp"
#include "nullsub.hpp"
#include "ThreadContext.hpp"

namespace Stand
{
#if CMDFLAG_BITS == 15
	enum CommandType : uint8_t
#elif CMDFLAG_BITS == 23
	enum CommandType : uint32_t
#endif
	{
		COMMAND_FULLTYPEFLAG     = 0b1110000,
		COMMAND_FLAG_LIST        = 0b1000000,
		COMMAND_FLAG_LIST_ACTION = 0b1100000,
		COMMAND_FLAG_TOGGLE      = 0b0010000,
		COMMAND_FLAG_SLIDER      = 0b0100000,

		COMMAND_LINK = 0,
		COMMAND_ISSUABLE,

		COMMAND_FIRST_PHYSICAL,
		COMMAND_ACTION = COMMAND_FIRST_PHYSICAL,
		COMMAND_ACTION_ITEM,
		COMMAND_INPUT,
		COMMAND_TEXTSLIDER,
		COMMAND_TEXTSLIDER_STATEFUL,
		COMMAND_READONLY_NAME,
		COMMAND_READONLY_VALUE,
		COMMAND_READONLY_LINK,
		COMMAND_DIVIDER,

		COMMAND_LIST = COMMAND_FLAG_LIST,
		COMMAND_LIST_CUSTOM_SPECIAL_MEANING,
		COMMAND_LIST_PLAYER,
		COMMAND_LIST_COLOUR,
		COMMAND_LIST_HISTORICPLAYER,
		COMMAND_LIST_READONLY,
		COMMAND_LIST_REFRESHABLE,
		COMMAND_LIST_CONCEALER,
		COMMAND_LIST_SEARCH,
		COMMAND_LIST_NAMESHARE,

		COMMAND_LIST_ACTION = COMMAND_FLAG_LIST_ACTION,
		COMMAND_LIST_SELECT,

		COMMAND_TOGGLE_NO_CORRELATION = COMMAND_FLAG_TOGGLE,
		COMMAND_TOGGLE,
		COMMAND_TOGGLE_CUSTOM,

		COMMAND_SLIDER = COMMAND_FLAG_SLIDER,
		COMMAND_SLIDER_FLOAT,
		COMMAND_SLIDER_RAINBOW,
	};

	using warningflags_t = uint8_t;
#define WARNING_COMMAND_BOUND	0b1
#define WARNING_SKIPPABLE		0b10

#pragma pack(push, 1)
	class Command
	{
	public:
		/* 0x08.0 */ CommandList* parent;
#if CMDFLAG_BITS == 15
		/* 0x10.0 */ const CommandType type : 7;
		/* 0x10.7 */ bool has_lua_data : 1;
		/* 0x11.0 */ commandflags_t flags : 15;
		/* 0x12.7 */ commandflags_t job_queued : 1;
		/* 0x13.0 */ soup::TransientToken transient_token;
#elif CMDFLAG_BITS == 23
		/* 0x10.0 */ const CommandType type : 7;
		/* 0x10.7 */ uint32_t has_lua_data : 1;
		/* 0x11.0 */ uint32_t flags : 23;
		/* 0x13.7 */ uint32_t job_queued : 1;
		/* 0x14.0 */ soup::TransientToken transient_token;
#endif

		explicit Command(CommandType type, CommandList* parent, commandflags_t flags = 0)
			: type(type), has_lua_data(false), flags(flags), job_queued(false), parent(parent)
		{
		}

		virtual ~Command() noexcept;

		template <typename T = Command>
		[[nodiscard]] soup::WeakRef<T> getWeakRef()
		{
			if (this != nullptr)
			{
				return soup::WeakRef<T>(static_cast<T*>(this));
			}
			return soup::WeakRef<T>();
		}

		[[nodiscard]] bool isAttached() const noexcept;
		[[nodiscard]] std::unique_ptr<Command> detach();
		void replace(std::unique_ptr<Command>&& cmd);

		void setParent(CommandList* parent);

		template <typename T>
		[[nodiscard]] bool isT() const noexcept;

		template <typename T>
		[[nodiscard]] __forceinline T* as() noexcept
		{
			return reinterpret_cast<T*>(this);
		}
		
		template <typename T>
		[[nodiscard]] __forceinline const T* as() const noexcept
		{
			return reinterpret_cast<const T*>(this);
		}

		[[nodiscard]] __forceinline bool isLink() const noexcept
		{
			return type == COMMAND_LINK;
		}

		[[nodiscard]] __forceinline bool isIssuable() const noexcept
		{
			return !isLink();
		}

		[[nodiscard]] __forceinline bool isPhysical() const noexcept
		{
			return type >= COMMAND_FIRST_PHYSICAL;
		}

		[[nodiscard]] __forceinline bool isList() const noexcept
		{
			return (type & COMMAND_FLAG_LIST);
		}

		[[nodiscard]] bool isListNameshare() const noexcept
		{
			return type == COMMAND_LIST_NAMESHARE || isListAction();
		}

		[[nodiscard]] __forceinline bool isListAction() const noexcept
		{
			return (type & COMMAND_FULLTYPEFLAG) == COMMAND_FLAG_LIST_ACTION;
		}

		[[nodiscard]] __forceinline bool isListNonAction() const noexcept
		{
			return (type & COMMAND_FULLTYPEFLAG) == COMMAND_FLAG_LIST;
		}

		[[nodiscard]] __forceinline bool isToggle() const noexcept
		{
			return (type & COMMAND_FULLTYPEFLAG) == COMMAND_FLAG_TOGGLE;
		}

		[[nodiscard]] __forceinline bool isSlider() const noexcept
		{
			return (type & COMMAND_FULLTYPEFLAG) == COMMAND_FLAG_SLIDER;
		}

		[[nodiscard]] bool handlesLeftRightInput() const noexcept;
		[[nodiscard]] bool isTextslider() const noexcept;

		[[nodiscard]] CommandPhysical* getPhysical() noexcept;
		[[nodiscard]] const CommandPhysical* getPhysical() const noexcept;

		[[nodiscard]] bool canBeResolved() const noexcept;

		[[nodiscard]] bool isConcealed() const noexcept
		{
			return flags & CMDFLAG_CONCEALED;
		}

		[[nodiscard]] bool hasParent(CommandList* target) const;
		[[nodiscard]] CommandList* resolveParent(const CommandType type);
		[[nodiscard]] const CommandList* resolveParent(const CommandType type) const;
		[[nodiscard]] std::string getPathConfig() const;
		[[nodiscard]] std::string getPathConfig(CommandList* target) const;
		[[nodiscard]] std::string getPathEn() const;
		[[nodiscard]] std::string getPathEn(CommandList* target) const;
		[[nodiscard]] std::string getPathEnFromParent(const CommandType type) const;
		[[nodiscard]] std::string getPathWeb() const;
		[[nodiscard]] CommandList* getParentWithContextMenuPassthrough() const;
		[[nodiscard]] CommandList* getParentWithContextMenuPassthrough(const CommandList* tab) const;
	private:
		[[nodiscard]] CommandList* getTabInner();
		[[nodiscard]] const CommandList* getTabInner() const;
		[[nodiscard]] CommandList* getTabInner(const CommandList* tab);
	public:
		[[nodiscard]] CommandList* getTab();
		[[nodiscard]] const CommandList* getTab() const;
		[[nodiscard]] CommandList* getTab(const CommandList* tab);
		[[nodiscard]] uint8_t getTabIndex() const;
		[[nodiscard]] std::string getLocalisedAddress() const;
		[[nodiscard]] std::string getLocalisedAddress(const std::string& separator) const;
		[[nodiscard]] std::wstring getLocalisedAddressW(const std::wstring& separator) const;
		[[nodiscard]] std::string getEnglishAddress(const std::string& separator) const;
		[[nodiscard]] std::wstring getEnglishAddressW(const std::wstring& separator) const;

		void focus(ThreadContext thread_context, bool unintrusive = false);
		void focusInParent(ThreadContext thread_context);

		void openHotkeysList(ThreadContext thread_context);

		[[nodiscard]] cursor_t getCursorPositionInParent() const;

		void beGone();
		void beGoneRootWriteLocked();
		virtual void preDelete();
		virtual void preDetach();

		[[nodiscard]] const Label& getMenuName() const;
		[[nodiscard]] std::string getNameForConfig() const;
		[[nodiscard]] std::wstring getMenuNameForAddress() const;
		[[nodiscard]] uint32_t getHash() const;
		[[nodiscard]] bool shouldShowUntrimmedName() const;

		void populateCorner(std::vector<std::wstring>& corner) const;

		[[nodiscard]] static std::wstring getGenericIrreversableWarningLabel();
		void showWarning(Click& click, std::wstring&& message, warningflags_t flags, std::function<void(ThreadContext)>&& proceed_callback, std::function<void(ThreadContext)>&& cancel_callback = &nullsub<bool>);
		void showWarningA(Click& click, const std::string& message, warningflags_t flags, std::function<void(ThreadContext)>&& proceed_callback, std::function<void(ThreadContext)>&& cancel_callback = &nullsub<bool>);
		void warnSelfHarm(Click& click, std::function<void(ThreadContext)>&& proceed_callback);

		[[nodiscard]] bool hasLuaData() const noexcept { return has_lua_data; }
		[[nodiscard]] CommandLuaData* getLuaData() noexcept;
		[[nodiscard]] const CommandLuaData* getLuaData() const noexcept;
	};
#pragma pack(pop)

	template <>
	inline bool Command::isT<CommandLink>() const noexcept
	{
		return isLink();
	}

	template <>
	inline bool Command::isT<CommandIssuable>() const noexcept
	{
		return isIssuable();
	}

	template <>
	inline bool Command::isT<CommandPhysical>() const noexcept
	{
		return isPhysical();
	}

	template <>
	inline bool Command::isT<CommandList>() const noexcept
	{
		return isList();
	}

	template <>
	inline bool Command::isT<CommandListPlayer>() const noexcept
	{
		return type == COMMAND_LIST_PLAYER;
	}

	template <>
	inline bool Command::isT<CommandListSelect>() const noexcept
	{
		return type == COMMAND_LIST_SELECT;
	}

	template <>
	inline bool Command::isT<CommandHistoricPlayer>() const noexcept
	{
		return type == COMMAND_LIST_HISTORICPLAYER;
	}

	template <>
	inline bool Command::isT<CommandListConcealer>() const noexcept
	{
		return type == COMMAND_LIST_CONCEALER || type == COMMAND_LIST_PLAYER;
	}

	template <>
	inline bool Command::isT<CommandToggleNoCorrelation>() const noexcept
	{
		return isToggle();
	}

	template <>
	inline bool Command::isT<CommandToggle>() const noexcept
	{
		return type == COMMAND_TOGGLE;
	}

	template <>
	inline bool Command::isT<CommandToggleCustom>() const noexcept
	{
		return type == COMMAND_TOGGLE_CUSTOM;
	}

	template <>
	inline bool Command::isT<CommandInput>() const noexcept
	{
		return type == COMMAND_INPUT;
	}

	template <>
	inline bool Command::isT<CommandTextslider>() const noexcept
	{
		return type == COMMAND_TEXTSLIDER;
	}

	template <>
	inline bool Command::isT<CommandReadonlyValue>() const noexcept
	{
		return type == COMMAND_READONLY_VALUE;
	}

	template <>
	inline bool Command::isT<CommandSlider>() const noexcept
	{
		return isSlider();
	}

	template <>
	inline bool Command::isT<CommandSliderFloat>() const noexcept
	{
		return type == COMMAND_SLIDER_FLOAT;
	}

	template <>
	inline bool Command::isT<CommandTextsliderStateful>() const noexcept
	{
		return type == COMMAND_TEXTSLIDER_STATEFUL;
	}
}
