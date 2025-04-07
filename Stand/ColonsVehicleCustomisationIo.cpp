#include "ColonsVehicleCustomisationIo.hpp"

#include <fstream>

#include <fmt/format.h>

#include <soup/Rgb.hpp>

#include "AbstractEntity.hpp"
#include "ColonsAndTabs.hpp"
#include "joaatToString.hpp"
#include "str2int.hpp"
#include "VehicleColour.hpp"

namespace Stand
{
	bool ColonsVehicleCustomisationIo::handleSaveCommand(Click& click, std::wstring& args, VehicleCustomisationIo& veh_cio, AbstractEntity* veh)
	{
		auto path = std::move(std::wstring(_wgetenv(L"appdata")).append(LR"(\Stand\Vehicles\)").append(std::move(args)).append(L".txt"));
		args.clear();

		std::vector<std::string> order{};
		for (int idx = 0; idx < VehicleCustomisationIo::_NUM_PROPERIES; ++idx)
		{
			switch (idx)
			{
			case VehicleCustomisationIo::ENVEFF_SCALE:
				continue;
			}
			if (veh != nullptr && !isPropertyApplicable(*veh, idx))
			{
				continue;
			}
			order.emplace_back(ColonsVehicleCustomisationIo::idx_names[idx]);
		}

		std::ofstream file_out(std::move(path));
		ColonsVehicleCustomisationIo colons_cio{};
		veh_cio.copyTo(colons_cio);
		ColonsAndTabs::writeBySchema(file_out, colons_cio.data, std::move(order), true);
		SOUP_IF_UNLIKELY (file_out.bad())
		{
			click.setResponse(LOC("GENFAIL"));
			return false;
		}
		return true;
	}

	[[nodiscard]] static constexpr bool isBoolean(int idx) noexcept
	{
		if (VehicleCustomisationIo::isVehicleMod(idx))
		{
			return VehicleMods::isBooleanExcludeLights(VehicleCustomisationIo::translatePropertyToVehicleMod(idx));
		}
		switch (idx)
		{
		case VehicleCustomisationIo::NEON_FRONT:
		case VehicleCustomisationIo::NEON_BACK:
		case VehicleCustomisationIo::NEON_LEFT:
		case VehicleCustomisationIo::NEON_RIGHT:
		case VehicleCustomisationIo::EXTRA_1:
		case VehicleCustomisationIo::EXTRA_2:
		case VehicleCustomisationIo::EXTRA_3:
		case VehicleCustomisationIo::EXTRA_4:
		case VehicleCustomisationIo::EXTRA_5:
		case VehicleCustomisationIo::EXTRA_6:
		case VehicleCustomisationIo::EXTRA_7:
		case VehicleCustomisationIo::EXTRA_8:
		case VehicleCustomisationIo::EXTRA_9:
		case VehicleCustomisationIo::EXTRA_10:
		case VehicleCustomisationIo::EXTRA_11:
		case VehicleCustomisationIo::EXTRA_12:
		case VehicleCustomisationIo::EXTRA_13:
		case VehicleCustomisationIo::EXTRA_14:
		case VehicleCustomisationIo::EXTRA_15:
			return true;
		}
		return false;
	}

	[[nodiscard]] static constexpr bool isOptVehicleColour(int idx) noexcept
	{
		return idx == VehicleCustomisationIo::CUSTOM_PRIMARY_COLOUR || idx == VehicleCustomisationIo::CUSTOM_SECONDARY_COLOUR;
	}

	[[nodiscard]] static constexpr bool isVehicleColour(int idx) noexcept
	{
		return idx == VehicleCustomisationIo::TYRE_SMOKE_COLOUR || idx == VehicleCustomisationIo::NEON_COLOUR;
	}

	[[nodiscard]] static constexpr bool isFloat(int idx) noexcept
	{
		return idx == VehicleCustomisationIo::ENVEFF_SCALE;
	}

	static void parseVehicleColour(VehicleColour* val, std::string&& str)
	{
		soup::Rgb::expandHex(str);
		val->r = std::stol(str.substr(0, 2), nullptr, 16);
		val->g = std::stol(str.substr(2, 2), nullptr, 16);
		val->b = std::stol(str.substr(4, 2), nullptr, 16);
	}

	Mixed ColonsVehicleCustomisationIo::read(int idx)
	{
		std::string str{};
		{
			auto entry = data.find(idx_names[idx]);
			if (entry != data.end())
			{
				str = entry->second;
			}
		}
		if (isBoolean(idx))
		{
			if (!str.empty())
			{
				return str == "Yes";
			}
			switch (idx)
			{
			case EXTRA_1:
			case EXTRA_2:
			case EXTRA_3:
			case EXTRA_4:
			case EXTRA_5:
			case EXTRA_6:
			case EXTRA_7:
			case EXTRA_8:
			case EXTRA_9:
			case EXTRA_10:
			case EXTRA_11:
			case EXTRA_12:
			case EXTRA_13:
			case EXTRA_14:
			case EXTRA_15:
				return true;
			}
			return false;
		}
		if (VehicleMods::hasVariation(translatePropertyToVehicleMod(idx)))
		{
			auto sep = str.find(',');
			if (sep == std::string::npos)
			{
				return std::pair<int, bool>(str2int<int>(str, 0), false);
			}
			return std::pair<int, bool>(str2int<int>(str.substr(0, sep), 0), true);
		}
		if (isOptVehicleColour(idx))
		{
			OptVehicleColour val{};
			if (!str.empty() && str.at(0) == '#')
			{
				val.is_applicable = true;
				parseVehicleColour(&val.colour, std::move(str));
			}
			return val;
		}
		if (isVehicleColour(idx))
		{
			VehicleColour val{};
			if (!str.empty())
			{
				parseVehicleColour(&val, std::move(str));
			}
			return val;
		}
		if (isFloat(idx))
		{
			if (str.empty())
			{
				return 0.0f;
			}
			return std::stof(str);
		}
		if (idx == MODEL)
		{
			return stringToJoaat(str);
		}
		if (idx == PLATE_TEXT)
		{
			if (str.empty())
			{
				return "Stand";
			}
			return str;
		}
		return str2int<int>(str, 0);
	}

	void ColonsVehicleCustomisationIo::write(int idx, Mixed&& value)
	{
		data.emplace(idx_names[idx], writeInner(idx, std::move(value)));
	}

	[[nodiscard]] static std::string formatVehicleColour(VehicleColour* c)
	{
		return fmt::format("#{:0>2x}{:0>2x}{:0>2x}", c->r, c->g, c->b);
	}

	std::string ColonsVehicleCustomisationIo::writeInner(int idx, Mixed&& value) const
	{
		if (isBoolean(idx))
		{
			return value.get<bool>() ? "Yes" : "No";
		}
		if (VehicleMods::hasVariation(translatePropertyToVehicleMod(idx)))
		{
			auto val = value.as<std::pair<int, bool>>();
			std::string str = fmt::to_string(val->first);
			if (val->second)
			{
				str.append(", Custom");
			}
			return str;
		}
		if (isOptVehicleColour(idx))
		{
			auto val = value.as<OptVehicleColour>();
			if (val->is_applicable)
			{
				return formatVehicleColour(&val->colour);
			}
			return {};
		}
		if (idx == MODEL)
		{
			return joaatToString(value.get<int>());
		}
		if (idx == PLATE_TEXT)
		{
			return value.c_str();
		}
		if (isVehicleColour(idx))
		{
			return formatVehicleColour(value.as<VehicleColour>());
		}
		if (isFloat(idx))
		{
			return fmt::to_string(value.get<float>());
		}
		return fmt::to_string(value.get<int>());
	}
}
