#pragma once

#include <cstdint>

#define NAMES_LAST_SIZE 64
#define NAMES_MALE_SIZE 64
#define NAMES_FEMALE_SIZE 64

namespace Stand
{
	enum name_t : uint8_t
	{
		NAME_MALE,
		NAME_FEMALE,
		NAME_LAST,
	};

	struct Names
	{
		static const char* const last[NAMES_LAST_SIZE];
		static const char* const male[NAMES_MALE_SIZE];
		static const char* const female[NAMES_FEMALE_SIZE];

		static const char* randFirstName(uint8_t gender);
		static const char* randLastName();
	};
}
