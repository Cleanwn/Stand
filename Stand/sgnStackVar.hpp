#pragma once

#include <cstdint>
#include <string>

namespace ScriptGenie
{
	// THIS NAMESPACE IS (MORE OR LESS) A MIRROR OF https://github.com/calamity-inc/ScriptGenie

	enum StackVarType : uint8_t
	{
		SV_INT = 0,
		SV_STR,
		SV_GLOBAL,
	};

	struct GlobalVar
	{
		uint32_t index;
		uint32_t offset;

		constexpr GlobalVar(uint32_t index) noexcept
			: index(index), offset(0)
		{
		}

#ifdef STAND_DEBUG
		[[nodiscard]] std::string toString() const
		{
			std::string str = "&(Global_";
			str.append(std::to_string(index));
			str.append(".f_");
			str.append(std::to_string(offset));
			str.push_back(')');
			return str;
		}
#endif
	};

	struct StackVar
	{
		StackVarType type;
		union {
			int64_t val_int;
			const char* val_str;
			GlobalVar val_global;
		};

		StackVar(int64_t val) noexcept
		{
			type = SV_INT;
			val_int = val;
		}

		StackVar(const char* val) noexcept
		{
			type = SV_STR;
			val_str = val;
		}

		StackVar(GlobalVar val) noexcept
		{
			type = SV_GLOBAL;
			val_global = val;
		}

		[[nodiscard]] bool operator ==(int64_t val) const noexcept
		{
			return type == SV_INT
				&& val_int == val
				;
		}

#ifdef STAND_DEBUG
		[[nodiscard]] std::string toString() const
		{
			if (type == SV_INT)
			{
				return std::to_string(val_int);
			}
			if (type == SV_STR)
			{
				std::string str = val_str;
				str.insert(0, 1, '"');
				str.push_back('"');
				return str;
			}
			if (type == SV_GLOBAL)
			{
				return val_global.toString();
			}
			return "INVALID";
		}
#endif
	};
}
