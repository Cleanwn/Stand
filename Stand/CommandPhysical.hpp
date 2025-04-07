#pragma once

#include "CommandIssuable.hpp"

#include "Click.hpp"
#include "Direction.hpp"
#include "Hotkey.hpp"
#include "Label.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandPhysical : public CommandIssuable
	{
	public:
		Label menu_name;
		Label help_text;
		std::vector<Hotkey> hotkeys;

		explicit CommandPhysical(const CommandType type, CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, const commandflags_t flags = 0, const CommandPerm perm = COMMANDPERM_USERONLY, const std::vector<Hotkey>& default_hotkeys = {});

		void onAttach() const noexcept;
		void preDelete() override;
		void preDetach() override;

		[[nodiscard]] const Label& getMenuName() const;
		void setMenuName(Label&& menu_name);
		void setMenuNameNoWeb(Label&& menu_name);
	private:
		void updateWebState() const;

	public:
		void setCommandNames(std::vector<CommandName>&& command_names);
		[[nodiscard]] static std::vector<CommandName> getCommandNamesFromMenuName(const Label& menu_name);
	protected:
		void useCommandNamesFromMenuName();

	public:
		void setHelpText(const Label& help_text);
		void setHelpText(Label&& help_text);
	private:
		void updateHelpTextOnWeb() const;

	public:
		[[nodiscard]] bool isInViewport() const;
		[[nodiscard]] bool isFocused() const;

		void processVisualUpdate(bool draw_data_only = false) const;

		[[nodiscard]] virtual Label getActivationName() const;
	protected:
		Label getActivationNameImplCombineWithParent(const char* joiner = " ") const;

	public:
		[[nodiscard]] bool hasHelpTextForListing() const;

		[[nodiscard]] bool isSelectable() const noexcept;

		[[nodiscard]] virtual std::wstring getCommandSyntax() const;
	protected:
		[[nodiscard]] static std::wstring getCommandSyntaxImpl(const std::wstring& syntax);

	public:
		[[nodiscard]] bool canBeUsedByOtherPlayers() const;

		[[nodiscard]] bool isHelpTextShown() const;
		void populateCorner(std::vector<std::wstring>& corner) const;

	public:
		[[nodiscard]] bool canShowSliderRangeInFeatureList() const;
		[[nodiscard]] std::string getFeatureListExtraText() const;

		[[nodiscard]] bool isActiveOnWeb() const;

		[[nodiscard]] CommandPhysical* getStateCommand();

		void ensurePhysicalFocus(ThreadContext thread_context);

		virtual void onFocus(ThreadContext thread_context, Direction momentum);
		virtual void onBlur(ThreadContext thread_context, Direction momentum);

		virtual void onTickInGameViewport();
		virtual void onTickInWebViewport();
		[[nodiscard]] bool receivesOnTickInGameViewportThisTick() const;
		[[nodiscard]] bool receivesOnTickInWebViewportThisTick() const;
	protected:
		void onTickInWebViewportImplRedirect();
		void dispatchOnTickInGameViewportIfNotThisTick();

	public:
		virtual void onPreScriptedAccess();

		virtual void onClick(Click& click);

		void getExtraInfo(CommandExtraInfo& info, std::wstring& args) override;

		void onCommand(Click& click, std::wstring& args) override;
	protected:
		[[nodiscard]] bool checkArgsLength(Click& click, const std::wstring& args, const size_t max);
		[[nodiscard]] bool onCommandValidateFileName(Click& click, std::wstring& args);
		void onCommandInvalidArg(Click& click, const std::wstring& arg) const;
		void onCommandReopen(Click& click, const std::wstring& args) const;

	public:
		virtual bool onLeft(Click& click, bool holding);
		virtual bool onRight(Click& click, bool holding);

		[[nodiscard]] __forceinline bool supportsStateOperations() const noexcept
		{
			return (flags & CMDFLAG_SUPPORTS_STATE_OPERATIONS);
		}

		[[nodiscard]] bool supportsUserStateOperations() const noexcept
		{
			return supportsStateOperations()
				&& !(flags & CMDFLAG_NO_USER_STATE_OPERATIONS)
				;
		}

		[[nodiscard]] __forceinline bool supportsSavedState() const noexcept
		{
			return !(flags & CMDFLAG_NO_SAVED_STATE);
		}

		void loadState(ClickType type);
		[[nodiscard]] virtual std::string getNameForConfig() const;
		[[nodiscard]] virtual std::string getState() const;
		[[nodiscard]] virtual std::string getDefaultState() const;
		virtual void setState(Click& click, const std::string& state);
		virtual void applyDefaultState(); // Needs a script thread; NOYIELD is fine.
		void saveStateInMemory();
		void processStateChange();

		[[nodiscard]] bool isOpenInContextMenu() const noexcept;

		[[nodiscard]] virtual const std::vector<Hotkey>& getDefaultHotkeys() const noexcept;
		[[nodiscard]] bool canCountAsCommandWithHotkeys() const noexcept;
		[[nodiscard]] bool canBeTriggeredInCurrentState() const;
		[[nodiscard]] bool isInDefaultHotkeyState() const;
		void applyDefaultHotkeys();
		[[nodiscard]] bool canHotkeyBeRemoved(const Hotkey hotkey) const noexcept;
		[[nodiscard]] bool hasHotkey(const Hotkey hotkey) const noexcept;
		void modifyHotkeys(bool hotkey_add, const Hotkey hotkey, ClickType type);
		void updateHotkeysState() const;
		void checkAddToCommandsWithHotkeys();
		void removeFromCommandsWithHotkeys() const noexcept;
		void removeHotkey(const Hotkey hotkey, ClickType type);
		virtual void onHotkeysChanged(ClickType type) const;
		void updateHotkeysInContextMenu() const;

		void queueJob(std::function<void()>&& func);
		void queueJob(std::function<void(ThreadContext)>&& func);

		void ensureScriptThread(ThreadContext thread_context, std::function<void()>&& func);
		void ensureScriptThread(std::function<void()>&& func);
		void ensureScriptThread(const Click& click, std::function<void()>&& func);
		void ensureScriptThread(Click& click, std::function<void(Click&)>&& func);

		void ensureScriptThread(ThreadContext thread_context, std::function<void(ThreadContext)>&& func);
		void ensureScriptThread(const Click& click, std::function<void(ThreadContext)>&& func);

		void ensureScriptThread(ThreadContext thread_context, bool root_readlocked, std::function<void(ThreadContext, bool)>&& func);
		void ensureScriptThread(const Click& click, bool root_readlocked, std::function<void(ThreadContext, bool)>&& func);

		void ensureYieldableScriptThread(ThreadContext thread_context, std::function<void()>&& func);
		void ensureYieldableScriptThread(const Click& click, std::function<void()>&& func);
		void ensureYieldableScriptThread(std::function<void()>&& func);

		void ensureGameTls(std::function<void()>&& func);

		void queueWorkerJob(std::function<void()>&& func);
		void ensureWorkerContext(ThreadContext thread_context, std::function<void()>&& func);
		void ensureWorkerContext(const Click& click, std::function<void()>&& func);

#define HANDLER_BREAK return false;
#define HANDLER_CHECK(cond) SOUP_IF_UNLIKELY (!(cond)) HANDLER_BREAK;
#define HANDLER_END return true;
#define HANDLER(cond, impl) HANDLER_CHECK(cond); impl; HANDLER_END;

		void registerScriptTickEventHandlerInContext(std::function<bool()>&& handler);
		void registerScriptTickEventHandler(ThreadContext thread_context, std::function<bool()>&& handler);
		void registerScriptTickEventHandler(const Click& click, std::function<bool()>&& handler);
		void registerScriptTickEventHandler(std::function<bool()>&& handler);
		void registerAsyncTickEventHandler(ThreadContext thread_context, std::function<bool()>&& handler);
		void registerAsyncTickEventHandler(const Click& click, std::function<bool()>&& handler);
		void registerPostTickEventHandler(std::function<bool()>&& handler);
		void registerPresentEventHandler(std::function<bool()>&& handler);
	};
#pragma pack(pop)
}
