#pragma once

#include "Command.hpp"

#include <unordered_set>
#include <vector>

#include <soup/ObfusString.hpp>

#include "Click.hpp"
#include "CommandName.hpp" // COMPACT_COMMAND_NAMES
#include "CommandPerm.hpp"

namespace Stand
{
#if COMPACT_COMMAND_NAMES
	#define CMDNAME(x) x
	#define utf8ToCmdName(x) x
	#define cmdNameToUtf8(x) x
	[[nodiscard]] std::wstring cmdNameToUtf16(const std::string& str);
#else
	#define CMDNAME(x) (L ## x)
	[[nodiscard]] std::wstring utf8ToCmdName(const std::string& str);
	[[nodiscard]] std::string cmdNameToUtf8(const std::wstring& str);
	#define cmdNameToUtf16(x) x
#endif

#define CMDNAMES_0() std::vector<CommandName>{}
#define CMDNAMES_1(a1) std::vector<CommandName>{ CMDNAME(a1) }
#define CMDNAMES_2(a1, a2) std::vector<CommandName>{ CMDNAME(a1), CMDNAME(a2) }
#define CMDNAMES_3(a1, a2, a3) std::vector<CommandName>{ CMDNAME(a1), CMDNAME(a2), CMDNAME(a3) }
#define CMDNAMES_4(a1, a2, a3, a4) std::vector<CommandName>{ CMDNAME(a1), CMDNAME(a2), CMDNAME(a3), CMDNAME(a4) }
#define CMDNAMES_5(a1, a2, a3, a4, a5) std::vector<CommandName>{ CMDNAME(a1), CMDNAME(a2), CMDNAME(a3), CMDNAME(a4), CMDNAME(a5) }
#define CMDNAMES(...) MULTI_MACRO(CMDNAMES, __VA_ARGS__)

#define CMDNAME_SHRT(a1) utf8ToCmdName(a1)
#define CMDNAMES_SHRT(a1) std::vector<CommandName>{ CMDNAME_SHRT(a1) }

#define CMDNAME_OBF(x) utf8ToCmdName(soup::ObfusString(x).str())
#define CMDNAMES_OBF_0() std::vector<CommandName>{}
#define CMDNAMES_OBF_1(a1) std::vector<CommandName>{ CMDNAME_OBF(a1) }
#define CMDNAMES_OBF_2(a1, a2) std::vector<CommandName>{ CMDNAME_OBF(a1), CMDNAME_OBF(a2) }
#define CMDNAMES_OBF_3(a1, a2, a3) std::vector<CommandName>{ CMDNAME_OBF(a1), CMDNAME_OBF(a2), CMDNAME_OBF(a3) }
#define CMDNAMES_OBF_4(a1, a2, a3, a4) std::vector<CommandName>{ CMDNAME_OBF(a1), CMDNAME_OBF(a2), CMDNAME_OBF(a3), CMDNAME_OBF(a4) }
#define CMDNAMES_OBF(...) MULTI_MACRO(CMDNAMES_OBF, __VA_ARGS__)

#pragma pack(push, 1)
	class CommandIssuable : public Command
	{
	public:
		static inline std::unordered_set<std::wstring> collapse_command_names = {};

		std::vector<CommandName> command_names;
		CommandPerm perm;

		explicit CommandIssuable(CommandList* parent, std::vector<CommandName>&& command_names, CommandPerm perm = COMMANDPERM_USERONLY, commandflags_t flags = 0, CommandType type = COMMAND_ISSUABLE);
	protected:
		bool mustHaveCommandName() const;

	public:
		CommandPhysical* getPhysical() = delete;
		[[nodiscard]] const Label& getMenuName() const = delete;

		[[nodiscard]] CommandList* getRoot();
		[[nodiscard]] bool isTab() const;

		[[nodiscard]] std::wstring getActivationNameLocalisedUtf16() const;
		[[nodiscard]] std::string getActivationNameLocalisedUtf8() const;

		void addSuffixToCommandNames(CommandName&& suffix);
		void addSuffixToCommandNamesSimple(const CommandName& suffix);
		[[nodiscard]] std::vector<CommandName> getPrefixedCommandNames(const CommandName& prefix) const;
		[[nodiscard]] std::vector<CommandName> getSuffixedCommandNames(const CommandName& suffix) const;
		[[nodiscard]] std::vector<CommandName> getSuffixedCommandNames(const std::vector<CommandName>& suffixes) const;
		static void combineCommandNames(std::vector<CommandName>& out_command_names, const std::vector<CommandName>& prefixes, const std::vector<CommandName>& suffixes);

		[[nodiscard]] static std::vector<CommandName> combineCommandNames(const std::vector<CommandName>& prefixes, const std::vector<CommandName>& suffixes);
		[[nodiscard]] static std::vector<CommandName> combineCommandNames(const std::vector<CommandName>& prefixes, const CommandName& suffix);
		[[nodiscard]] static std::vector<CommandName> combineCommandNames(const CommandName& prefix, const std::vector<CommandName>& suffixes);

		[[nodiscard]] bool isAuthorised(const CommandPerm perm) const;
		[[nodiscard]] bool isAuthorised(const Click& click) const;

		[[nodiscard]] std::wstring getCompletionHint() const;
		virtual void getExtraInfo(CommandExtraInfo& info, std::wstring& args);

		virtual void onCommand(Click& click, std::wstring& args) = 0;
	};
#pragma pack(pop)
}
