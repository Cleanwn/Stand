#include "vector.hpp"

#include <fmt/xchar.h>

#include "natives.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"

namespace Stand
{
	void v_collectAxis(std::string& res, float axis)
	{
		StringUtils::list_append(res, axis);
	}

	void v_collectAxis(std::wstring& res, float axis)
	{
		StringUtils::list_append(res, fmt::to_wstring(axis));
	}

	float v_getZFromHeightmap(float x, float y)
	{
		return PATHFIND::GET_APPROX_HEIGHT_FOR_POINT(x, y);
	}

	void v_world2screen(float worldX, float worldY, float worldZ, float* screenX, float* screenY)
	{
		Util::GET_SCREEN_COORD_FROM_WORLD_COORD_NO_ADJUSTMENT(worldX, worldY, worldZ, screenX, screenY);
	}

	v3::v3(const rage::scrVector3& b) noexcept
		: x(b.x), y(b.y), z(b.z)
	{
	}
}

namespace rage
{
	scrVector3::scrVector3(const Stand::v3& vec) noexcept
		: x(vec.x), y(vec.y), z(vec.z)
	{
	}

	void scrVector3::operator =(const scrVector3& b) noexcept
	{
		x = b.x;
		y = b.y;
		z = b.z;
	}
}
