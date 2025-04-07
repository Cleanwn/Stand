#include "sc_presence_attributes.hpp"

#ifdef STAND_DEBUG
#include <fmt/format.h>
#endif

#include "pointers.hpp"
#include "rlPc.hpp"

using namespace Stand;

namespace rage
{
	rlScPresenceAttribute* rlScPresenceAttribute::byIndex(uint32_t i)
	{
		return reinterpret_cast<rlScPresenceAttribute*>(reinterpret_cast<uint64_t>(pointers::presence_attributes) + (i * sizeof(rlScPresenceAttribute)));
	}

	rlScPresenceAttribute* rlScPresenceAttribute::findByName(const char* name)
	{
		if (pointers::num_presence_attributes != nullptr)
		{
			for (uint32_t i = 0; i < *pointers::num_presence_attributes; ++i)
			{
				auto* const prop = rlScPresenceAttribute::byIndex(i);
				if (strcmp(prop->name, name) == 0)
				{
					return prop;
				}
			}
		}
		return nullptr;
	}

#ifdef STAND_DEBUG
	std::string rlScPresenceAttribute::toString() const
	{
		std::string str = name;
		str.append(": ");
		/*if (!unk)
		{
			str.append("[not set]");
		}
		else*/ switch (type)
		{
		default:
			str.append("[type ");
			str.append(fmt::to_string(type));
			str.push_back(']');
			break;

		case RLSC_PRESTYPE_S64:
			str.append(fmt::to_string(int_value));
			break;

		case RLSC_PRESTYPE_DOUBLE:
			str.append(fmt::to_string(double_value));
			str.push_back('d');
			break;

		case RLSC_PRESTYPE_STRING:
			str.append(string_value);
			break;
		}
		return str;
	}
#endif

	void rlScPresenceAttribute::setInt(int64_t value) noexcept
	{
		if (int_value != value)
		{
			pointers::g_rlPc->presence_manager->SetIntAttribute(0, name, value);
		}
	}

	void rlScPresenceAttribute::setString(const char* value) noexcept
	{
		if (strcmp(string_value, value) != 0)
		{
			pointers::g_rlPc->presence_manager->SetStringAttribute(0, name, value);
		}
	}

	void rlScPresenceAttribute::setString(const std::string& value) noexcept
	{
		if (value != string_value)
		{
			pointers::g_rlPc->presence_manager->SetStringAttribute(0, name, value.c_str());
		}
	}
}
