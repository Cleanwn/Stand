#pragma once

#include "fwddecl.hpp"
#include "atStringHash.hpp"

namespace Stand
{
#pragma pack(push, 1)
	struct OnlineVersion
	{
		uint16_t id;
		//hash_t name_hash;

		OnlineVersion()
			: id(0)//, name_hash(0)
		{
		}

		consteval OnlineVersion(const char* in)
		{
			// in[0] - '1'
			// in[1] - '.'
			id = (in[2] - '0'); id *= 10;
			id += (in[3] - '0'); id *= 10;
			// in[4] - ' ' or '.'
			if (in[4] == '.')
			{
				id += (in[5] - '0');
				// in[6] - ' '
				//name_hash = rage::atStringHash_consteval(&in[7]);
			}
			else
			{
				//name_hash = rage::atStringHash_consteval(&in[5]);
			}
		}

		[[nodiscard]] std::string getVersionString() const;
		[[nodiscard]] Label getName() const;

		[[nodiscard]] bool empty() const noexcept
		{
			return id == 0 /*&& name_hash == 0*/;
		}

		void reset() noexcept
		{
			id = 0;
			//name_hash = 0;
		}
	};
	static_assert(sizeof(OnlineVersion) < 8); // we instantiate this for every item, so we want be sure a pointer is not more efficient
#pragma pack(pop)
}
