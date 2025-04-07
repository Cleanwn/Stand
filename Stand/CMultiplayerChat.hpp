#pragma once

#include <cstdint>

#pragma pack(push, 1)
struct CMultiplayerChat
{
	enum ChatState : int32_t
	{
		CLOSED = 0,
		TEAM = 1,
		ALL = 2
	};

public:
	/*  0 */ int32_t iHideChatWindowCount;
	/*  4 */ int32_t iChatBackCount;
	/*  8 */ int32_t message_length;
	/* 12 */ int32_t last_delete_tick;
private:
	/* 16 */ char pad_16[20 - 16];
public:
	/* 20 */ char is_open;
private:
	/* 21 */ char pad_21[28 - 21];
public:
	/* 28 */ ChatState state;
private:
	/* 32 */ char pad_32[44 - 32];
public:
	/* 44 */ wchar_t message[140];

	[[nodiscard]] ChatState getState() const noexcept
	{
		if (is_open)
		{
			return state;
		}
		// state/m_eTypeMode may not be initialised at this point.
		return CLOSED;
	}
};
#pragma pack(pop)
static_assert(sizeof(CMultiplayerChat) == 44 + 280);
