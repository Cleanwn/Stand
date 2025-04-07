#pragma once

#include "CommandList.hpp"

#include "rlSessionInfo.hpp"

namespace Stand
{
	enum RequestMembersState : uint8_t
	{
		RM_PROMPT = 0,
		RM_CONFIRMED,
		RM_SENT_JR,
		RM_SENT_JR_DIFF_SLOT,
		RM_SPYING,
		RM_SPYING_RECONNECT,
	};

#pragma pack(push, 1)
	class CommandListSession : public CommandList
	{
	private:
		size_t base_offset;

		bool is_active = false;

		uint8_t dead = 0;
		CommandDivider* divider = nullptr;
		CommandAction* request_members_action = nullptr;
	public:
		RequestMembersState request_members_state = RM_PROMPT;
		uint32_t slot = -1;

		using CommandList::CommandList;

		void onClick(Click& click) final;

		void onActiveListUpdate() final;
	private:
		void onTickAsActiveList();
		void reset();
		void resetRequestMembers();
		void removeRequestMembersAction();
		void removeMembers();
		void setStatus(Label&& status);
		void setStatusToMemberCount(const rage::rlSessionDetail& detail, unsigned int count);

		[[nodiscard]] size_t getMembersOffset() const noexcept;
	public:
		[[nodiscard]] size_t getNumShownMembers() const noexcept;

	public:
		[[nodiscard]] std::string getGsinfo() const;
		[[nodiscard]] virtual const rage::rlSessionInfo* getSessionInfoIfAvailable() const = 0;
		[[nodiscard]] virtual int64_t getSessionType() const;
		virtual bool checkCreateJoinAndSpecateCommands() = 0;
	};
#pragma pack(pop)
}
