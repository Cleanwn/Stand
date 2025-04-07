#pragma once

namespace rage
{
	union scrValue
	{
		int Int;
		unsigned Uns;
		float Float;
		const char* String;
		scrValue* Reference;
		size_t Any;

		[[nodiscard]] bool operator==(const scrValue& b) const noexcept
		{
			return Int == b.Int;
		}
	};
	static_assert(sizeof(scrValue) == 8);

	typedef char scrTextLabel7[2 * sizeof(scrValue)];
	typedef char scrTextLabel15[4 * sizeof(scrValue)];
	typedef char scrTextLabel23[6 * sizeof(scrValue)];
	typedef char scrTextLabel31[8 * sizeof(scrValue)];
	typedef char scrTextLabel63[16 * sizeof(scrValue)];
}
