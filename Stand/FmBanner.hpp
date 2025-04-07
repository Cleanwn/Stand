#pragma once

#include <string>

namespace Stand
{
	struct FmBanner
	{
		int style = 0;

		char headline[16]{};

		char subtext[32]{};
		char customtext_1[64]{}; // non-label
		char customtext_2[16]{};
		char customtext_3[16]{};
		int customint_1 = -1;
		int customint_2 = -1;
		int player_1 = -1;

		[[nodiscard]] static bool isSlotInUse(const int slot);
		[[nodiscard]] static int getFreeSlot(const int fallback = 0);
		[[nodiscard]] static FmBanner fromSlot(const int slot);

		[[nodiscard]] std::string getSubtextString() const;

		void display(const int slot = 0);
	};
}
