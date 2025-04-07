#pragma once

#include <cstdint>
#include <functional>

#include "fwddecl.hpp"
#include "gta_player.hpp"

#include "ClickType.hpp"
#include "CommandPerm.hpp"

namespace Stand
{
	enum class ChainState : uint8_t
	{
		NONE,
		PASSED_THROUGH,
		EXECUTED,
	};

	enum ClickResponseType : uint8_t
	{
		RESPONSE_CUSTOM,
		RESPONSE_GENERIC,
		RESPONSE_NONE,
	};

	enum Sound : uint8_t
	{
		SOUND_SELECT,
		SOUND_YES,
		SOUND_NO,
		SOUND_ERROR,
	};

	[[nodiscard]] extern CommandList* getRootListPtr();

#pragma pack(push, 1)
	class Click
	{
	public:
		inline static bool self_chat_commands_friends_perms = false;
		inline static CommandPerm perms_friends = COMMANDPERM_FRIENDLY | COMMANDPERM_NEUTRAL | COMMANDPERM_SPAWN | COMMANDPERM_RUDE | COMMANDPERM_AGGRESSIVE;
		inline static CommandPerm perms_crew = COMMANDPERM_FRIENDLY | COMMANDPERM_NEUTRAL | COMMANDPERM_SPAWN | COMMANDPERM_RUDE;
		inline static CommandPerm perms_team = COMMANDPERM_FRIENDLY | COMMANDPERM_NEUTRAL | COMMANDPERM_SPAWN | COMMANDPERM_RUDE;
		inline static CommandPerm perms_strangers = COMMANDPERM_FRIENDLY | COMMANDPERM_NEUTRAL | COMMANDPERM_SPAWN | COMMANDPERM_RUDE;
		inline static bool auto_proceed_warnings_for_commands = false;
		inline static bool auto_proceed_warnings_for_hotkeys = false;

		inline static time_t last_successful_permission_check = 0;

		const ClickType type;
		ThreadContext thread_context;
		const compactplayer_t issued_by;
		compactplayer_t issued_for;
		const bool issued_by_user;
		bool issuer_is_explicit = false;
		ChainState chain_state = ChainState::NONE;
		Sound sound_feedback = SOUND_SELECT;
		Label response = NOLABEL;
		ClickResponseType response_type = RESPONSE_GENERIC;
		bool response_acknowledged = false;

		explicit Click(ClickType type) noexcept;
		explicit Click(ClickType type, ThreadContext thread_context) noexcept;
		explicit Click(ClickType type, ThreadContext thread_context, compactplayer_t issuer) noexcept;

		Click(const Click& b) noexcept;
		Click(Click&& b) noexcept;

		~Click();

		[[nodiscard]] Click derive(ClickType type) const noexcept;
		[[nodiscard]] Click deriveAuto() const noexcept;

		[[nodiscard]] bool canShowCommandBox() const noexcept;
		void showCommandBoxIfPossible(const std::wstring& prefill);
		void showCommandBox(const std::wstring& prefill);
		[[nodiscard]] AbstractPlayer getIssuer() const noexcept;
		[[nodiscard]] AbstractPlayer getEffectiveIssuer() const noexcept;
		[[nodiscard]] bool issuedByUser() const noexcept;
		[[nodiscard]] bool issuedByAndForUser() const noexcept;

		void ensureYieldableScriptThread(std::function<void()>&& func) const;

		void ensureScriptThread(std::function<void()>&& func) const;
		void ensureScriptThread(std::function<void(Click&)>&& func);

		void ensureWorkerContext(std::function<void()>&& func) const;

		[[nodiscard]] bool isMenu() const noexcept;
		[[nodiscard]] bool isAuto() const noexcept;
		[[nodiscard]] bool isStand() const noexcept;
		[[nodiscard]] bool isBulk() const noexcept;
		[[nodiscard]] bool isChat() const noexcept;
		[[nodiscard]] bool isWeb() const noexcept;

		[[nodiscard]] bool isCommandBoxVariety() const noexcept;
		[[nodiscard]] bool canOpenCommandBox() const noexcept;

		void setUnavailable() noexcept;
		[[nodiscard]] bool isStopInputAllowed() const noexcept;
		void stopInputIfAllowed() const noexcept;
		[[nodiscard]] bool inOnline() noexcept;
		[[nodiscard]] bool inOnline() const noexcept;
		[[nodiscard]] __declspec(noinline) bool isBasicEdition() noexcept;
		[[nodiscard]] __declspec(noinline) bool isRegularEdition() noexcept;
		[[nodiscard]] __declspec(noinline) bool isRegularEditionOrSp() noexcept;
		[[nodiscard]] __declspec(noinline) bool isUltimateEdition() noexcept;
		[[nodiscard]] bool canBypassEditionRestrictions() noexcept;
		[[nodiscard]] bool isHost() noexcept;
		[[nodiscard]] bool isBoss() noexcept;

		[[nodiscard]] bool canExecuteChainCommand() noexcept;
		void setChainCommandExecuted() noexcept;

		[[nodiscard]] bool hasResponse() const noexcept;
		[[nodiscard]] bool canHaveResponse() const noexcept;
		[[nodiscard]] bool canHaveGenericResponse() const noexcept;
		[[nodiscard]] bool canHaveGenericErrorResponse() const noexcept;
		void setResponse(Label&& response) noexcept;
		void setResponse(Label&& response) const;
		void setGenericResponse(Label&& response);
		void setNoResponse() noexcept;
		void notAvailableOnUser();
		void uwotm8();
		void setSpoofUpdate();

		void ensureResponse();
		void ensureActivationName(const std::string& activation_name);
		[[nodiscard]] Label getResponse();
		void forgetResponse() noexcept;
		void respond();

		[[nodiscard]] bool isSoundAllowed() const noexcept;

		CommandPerm getEffectivePermissions() const;

		[[nodiscard]] bool shouldAutomaticallyProceedOnWarning(bool skippable) const noexcept;

		void setResponseCommandUnknownOrAmbiguous(bool is_unknown, const std::wstring& input);

		[[nodiscard]] CommandIssuable* getCommandIssuableFromArg(const std::wstring& arg);
		[[nodiscard]] CommandPhysical* getCommandPhysicalFromArg(const std::wstring& arg);
	};
#pragma pack(pop)
}
