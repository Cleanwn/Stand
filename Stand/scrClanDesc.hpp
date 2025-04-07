#pragma once

#include "scrValue.hpp"

struct scrClanDesc
{
	/*   0 */ rage::scrValue id;
	/*   8 */ rage::scrTextLabel63 name;
	/* 136 */ rage::scrTextLabel7 tag;
	/* 152 */ rage::scrValue member_count;
	/* 160 */ rage::scrValue alt_badge; // system clan
	/* 168 */ rage::scrValue open_clan;
	/* 176 */ rage::scrTextLabel31 position; // rank name
	/* 240 */ rage::scrValue rank_order;
	/* 248 */ rage::scrValue created_time_posix;
	/* 256 */ rage::scrValue r;
	/* 264 */ rage::scrValue g;
	/* 272 */ rage::scrValue b;
};
static_assert(sizeof(scrClanDesc) == 280);
