#pragma once

namespace Stand
{
	template <typename Struct, typename MemberType>
	struct Offset
	{
		uintptr_t offset;

		Offset(MemberType Struct::* offset)
			: offset(reinterpret_cast<uintptr_t>(&(reinterpret_cast<Struct*>(nullptr)->*offset)))
		{
		}

		[[nodiscard]] MemberType& resolve(Struct& obj) const noexcept
		{
			return *reinterpret_cast<MemberType*>(reinterpret_cast<uintptr_t>(&obj) + offset);
		}
	};
};
