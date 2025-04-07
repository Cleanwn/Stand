#pragma once

#include "CommandLua.hpp"

namespace Stand
{
	template <typename T>
	class CommandLuaSliderBase : public CommandLuaPhysical<T>
	{
	private:
		using Base = CommandLuaPhysical<T>;

	protected:
		using Base::Base;

	public:
		std::vector<std::pair<int, std::wstring>> replacements;

		std::wstring formatNumber(int num, bool allow_replacements) const final
		{
			if (allow_replacements)
			{
				for (const auto& replacement : replacements)
				{
					if (replacement.first == num)
					{
						return replacement.second;
					}
				}
			}
			return Base::formatNumber(num, allow_replacements);
		}
	};
}
