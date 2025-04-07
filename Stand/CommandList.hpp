#pragma once

#include "CommandPhysical.hpp"

#include <memory>
#include <functional>

#include <soup/WeakRef.hpp>

#include "CommandBuilder.hpp"

namespace Stand
{
	enum ListIndicatorType : uint8_t
	{
		LISTINDICATOR_ARROW = 0,
		LISTINDICATOR_ARROW_IF_CHILDREN,
		LISTINDICATOR_OFF,
		LISTINDICATOR_ON,

		_LISTINDICATOR_SIZE
	};
	static_assert(LISTINDICATOR_ON == 0b11); // indicator_type uses 2 bits

#pragma pack(push, 1)
	class CommandList : public CommandPhysical
	{
	public:
		static constexpr cursor_t max_web_commands = 360;

		std::vector<std::unique_ptr<Command>> children{};
		cursor_t m_cursor = 0;
		cursor_t m_offset = 0;
		soup::WeakRef<Command> parent_for_back{};
		size_t dividers = 0;
		bool web_state_update_queued : 1 = false;
		ListIndicatorType indicator_type : 2 = LISTINDICATOR_ARROW;

		explicit CommandList(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, const commandflags_t flags = CMDFLAGS_LIST, const CommandType type = COMMAND_LIST);

		template <class T, typename... Args>
		[[nodiscard]] std::unique_ptr<T> makeChild(Args&&...args)
		{
			return std::make_unique<T>(this, std::forward<Args>(args)...);
		}

		template <class T, typename... Args>
		[[nodiscard]] std::unique_ptr<T> makeChild(Label&& menu_name, std::vector<CommandName>&& command_names, Args&&...args)
		{
			return std::make_unique<T>(this, std::move(menu_name), std::move(command_names), std::forward<Args>(args)...);
		}

		template <class T, typename Arg1, typename... Args>
		[[nodiscard]] std::unique_ptr<T> makeChild(Arg1&& arg1, Label&& menu_name, std::vector<CommandName>&& command_names, Args&&...args)
		{
			return std::make_unique<T>(this, std::move(arg1), std::move(menu_name), std::move(command_names), std::forward<Args>(args)...);
		}

		template <class T, typename... Args>
		T* createChild(Args&&...args)
		{
			return (T*)this->children.emplace_back(std::make_unique<T>(this, std::forward<Args>(args)...)).get();
		}

		template <class T, typename... Args>
		T* createChild(std::vector<CommandName>&& command_names, Args&&...args)
		{
			return (T*)this->children.emplace_back(std::make_unique<T>(this, std::move(command_names), std::forward<Args>(args)...)).get();
		}
		
		template <class T, typename... Args>
		T* createChild(Label&& menu_name, std::vector<CommandName>&& command_names, Args&&...args)
		{
			return (T*)this->children.emplace_back(std::make_unique<T>(this, std::move(menu_name), std::move(command_names), std::forward<Args>(args)...)).get();
		}

		template <class T, typename Arg1, typename... Args>
		T* createChild(Arg1&& arg1, Label&& menu_name, std::vector<CommandName>&& command_names, Args&&...args)
		{
			return (T*)this->children.emplace_back(std::make_unique<T>(this, std::move(arg1), std::move(menu_name), std::move(command_names), std::forward<Args>(args)...)).get();
		}

		template <class T, typename... Args>
		CommandBuilder<T> createChildWithBuilder(Args&&...args)
		{
			return this->createChild<T>(std::forward<Args>(args)...);
		}

		[[nodiscard]] bool isRoot() const noexcept;

		void preDelete() override;
		void preDetach() override;

		void dispatchPreDeleteForAllChildren() const;
		void resetChildrenWithPreDelete();
		void resetChildren() noexcept;
		void processChildrenUpdate();
		void processChildrenVisualUpdate();
		void processChildrenUpdateWithPossibleCursorDisplacement(Command* prev_focus, ThreadContext tc);

		void emplaceVisible(std::unique_ptr<Command>&& cmd);
		void removeChild(std::vector<std::unique_ptr<Command>>::iterator it);

		[[nodiscard]] bool canDispatchOnTickInViewportForChildren() const noexcept;
		[[nodiscard]] bool canDispatchOnTickInWebViewportForChildren() const noexcept;

		void onClick(Click& click) override;
	protected:
		static void onClickImplUnavailable(Click& click);

	public:
		void open(ThreadContext thread_context, bool unintrusive = false, bool without_grid_update = false, bool can_update_root_cursor = true);
		void redirectOpen(Click& click, const CommandPhysical* cmd);
		void redirectOpen(ThreadContext tc, const CommandPhysical* cmd);
		void redirectOpen(const CommandPhysical* cmd);

		[[nodiscard]] CommandList* getParentForBack(CommandList* tab);
	private:
		[[nodiscard]] CommandList* getParentForBackIfApplicable(const CommandList* const tab) const;
	public:
		[[nodiscard]] CommandList* getParentForTab(const CommandList* const tab) const;

		void setIndicatorType(ListIndicatorType t);

		void fixCursorAndOffset(bool no_padding = false);
		bool fixCursor();
		void fixOffset(bool no_padding = false);
		void processFocusUpdate(ThreadContext thread_context, Direction momentum, bool user_action = false);

		virtual void onActiveListUpdate();

		void resetCursor() noexcept;
		bool resetCursorIfApplicable() noexcept;

		virtual void onBack(ThreadContext thread_context);

		[[nodiscard]] bool isCurrentUiList() const;
		[[nodiscard]] bool isCurrentMenuList() const;
		[[nodiscard]] bool isCurrentWebList() const;

		[[nodiscard]] bool isCurrentUiOrWebList() const;
		[[nodiscard]] bool isCurrentMenuOrWebList() const;

		[[nodiscard]] bool isCurrentMenuListInTab() const;

		[[nodiscard]] bool isThisOrSublist(const CommandList* list) const;
		[[nodiscard]] bool isThisOrSublistInTab(const CommandList* list) const;

		[[nodiscard]] bool isThisOrSublistCurrentUiList() const;
		[[nodiscard]] bool isThisOrSublistCurrentMenuList() const;
		[[nodiscard]] bool isThisOrSublistCurrentWebList() const;
		[[nodiscard]] bool isThisOrSublistCurrentUiOrWebList() const;
		[[nodiscard]] bool isThisOrSublistCurrentMenuOrWebList() const;

		[[nodiscard]] bool isThisOrSublistCurrentUiListInTab() const;
		[[nodiscard]] bool isThisOrSublistActiveInMyTabMenu() const;
		[[nodiscard]] bool isThisOrSublistCurrentMenuListInTab() const;

		void goBackIfActive(ThreadContext thread_context);

		using CommandPhysical::isInViewport;
		[[nodiscard]] bool isInViewport(const CommandPhysical* const physical) const;
		[[nodiscard]] bool isInViewportRootReadlocked(const CommandPhysical* const physical) const;

		[[nodiscard]] bool canUpdateCursor() const;

		void approachCursor(cursor_t desired);

		void evacuateCursor();
		void evacuateCursorTab(uint8_t tab_idx);
		void evacuateCursorWeb();
		void evacuateCursorStandalone();
		void evacuateCursorStandaloneNocheck();

		void updateCursorGoingBack(CommandList* prev_active_list);

		void updateWebState();
	private:
		void updateWebStateNoCheck();
	public:
		void updateWebStateImpl() const;

		[[nodiscard]] Command* getFocus() const;
		[[nodiscard]] CommandPhysical* getFocusPhysical() const;
		[[nodiscard]] Command* getChild(CommandType type) const;

		[[nodiscard]] CommandPhysical* resolveChildByMenuName(const Label& target) const;
		[[nodiscard]] CommandPhysical* resolveChildByMenuNameHash(hash_t target) const;
		[[nodiscard]] CommandPhysical* resolveChildByMenuNameLiteral(const std::string& target) const;
		[[nodiscard]] CommandPhysical* resolveChildByMenuNameWebString(const std::string& target) const;
		[[nodiscard]] CommandPhysical* recursivelyResolveChildByMenuName(const Label& target) const;
		[[nodiscard]] CommandPhysical* recursivelyResolveChildByMenuNameHash(hash_t target) const;
		[[nodiscard]] CommandPhysical* resolveCommandConfig(const std::string& target, const std::string& prefix = {}) const;
		[[nodiscard]] CommandPhysical* resolveCommandEnglish(const std::string& target, const std::string& prefix = {}) const;
		[[nodiscard]] CommandPhysical* resolveCommandWeb(const std::string& target, const std::string& prefix = {}) const;

		enum CommandNameMatchType : int8_t
		{
			NMT_OVERSHOT = -1,
			NMT_MISSED = 0,
			NMT_GRAZED,
			NMT_HIT,
		};
		[[nodiscard]] static int8_t checkCommandNameMatch(const CommandName& command_name_prefix, const CommandName& command_name);

		template <class T = CommandIssuable>
		bool findCommandsWhereCommandNameStartsWith(std::vector<T*>& results, const CommandName& command_name_prefix, const CommandPerm perm = COMMANDPERM_ALL, const size_t results_limit = 2000) const
		{
			for (auto& _command : this->children)
			{
				if (!_command->isT<T>())
				{
					continue;
				}
				auto* command = reinterpret_cast<T*>(_command.get());
				bool recurse_into = !command->isListAction();
				bool grazed = false;
				for (const auto& command_name : command->command_names)
				{
					auto nmt = checkCommandNameMatch(command_name_prefix, command_name);
					if (nmt != NMT_MISSED)
					{
						if (nmt == NMT_HIT)
						{
							results = { command };
							return true;
						}
						else if (nmt == NMT_GRAZED)
						{
							if (!grazed) // Don't want the same command multiple times in results (for command box completions, e.g. when entering "quit")
							{
								grazed = true;
								if (results.size() != results_limit) // If we hit results limit, keep searching for possible hit, but don't record any more partials. (for `menu.focus(menu.ref_by_command_name("time"))`)
								{
									results.emplace_back(command);
								}
							}
						}
						else if (nmt == NMT_OVERSHOT)
						{
							recurse_into = true;
						}
					}
				}
				if (command->isList()
					&& recurse_into
					&& command->as<CommandList>()->hasCommandsWhereCommandNameStartsWith(command_name_prefix)
					&& command->as<CommandList>()->findCommandsWhereCommandNameStartsWith<T>(results, command_name_prefix, perm, results_limit)
					)
				{
					return true;
				}
			}
			return false;
		}

		[[nodiscard]] virtual bool hasCommandsWhereCommandNameStartsWith(const CommandName& command_name_prefix) const noexcept;

		void recursivelyApplyDefaultState() const;

		void recursivelySaveStateInMemory(std::unordered_map<std::string, std::string>& state, const std::string& prefix = {}) const;

		[[nodiscard]] size_t countChildren(bool include_invisible, bool include_non_resolvable) const noexcept;
		[[nodiscard]] size_t countVisibleChildren() const noexcept;
		[[nodiscard]] std::wstring getCursorText() const;
		[[nodiscard]] cursor_t getCursorIgnoreUnresolvable() const;
		[[nodiscard]] size_t countResolvableChildren() const noexcept;

		void forEachChild(std::function<void(Command*)>&& func) const;
		void forEachPhysicalChild(const std::function<void(CommandPhysical*)>& func) const;
		void forEachChildAsPhysical(std::function<bool(CommandPhysical*)>&& consumer) const;
		void recursiveForEachNonListChild(const std::function<void(Command*)>& func) const;
		void recursiveForEachNonListChildRootReadLocked(const std::function<void(Command*)>& func) const;
		void recursiveForEachChild(const std::function<bool(const std::unique_ptr<Command>&)>& consumer) const;
	private:
		void recursiveForEachChildImpl(const std::function<bool(const std::unique_ptr<Command>&)>& consumer) const;

	public:
		void setCursorPos(const Command* child) const;

		using sort_algo_t = bool(*)(const std::unique_ptr<Command>&, const std::unique_ptr<Command>&);
		static bool sortAlgoName(const std::unique_ptr<Command>& a, const std::unique_ptr<Command>& b);
		void sortChildren(sort_algo_t sort_algo = &sortAlgoName);
	};
#pragma pack(pop)
}
