#pragma once

#include <string>

#include <soup/VectorBase.hpp>
#include <soup/Vector3.hpp>

#pragma pack(push, 1)
namespace Stand
{
	void v_collectAxis(std::string& res, float axis);
	void v_collectAxis(std::wstring& res, float axis);
	[[nodiscard]] float v_getZFromHeightmap(float x, float y);
	void v_world2screen(float worldX, float worldY, float worldZ, float* screenX, float* screenY);

	template<uint8_t axes, typename T>
	struct VectorBase : public soup::VectorBase<axes, T>
	{
		operator std::string() const
		{
			std::string res;
			for (uint8_t i = 0; i != soup::VectorBase<axes, T>::getAxes(); ++i)
			{
				v_collectAxis(res, soup::VectorBase<axes, T>::axis(i));
			}
			return res;
		}

		operator std::wstring() const
		{
			std::wstring res;
			for (uint8_t i = 0; i != soup::VectorBase<axes, T>::getAxes(); ++i)
			{
				v_collectAxis(res, soup::VectorBase<axes, T>::axis(i));
			}
			return res;
		}

		[[nodiscard]] float getZFromHeightmap() const
		{
			return v_getZFromHeightmap(((const T*)this)->x, ((const T*)this)->y);
		}
	};
}

namespace rage
{
	struct Vector2 : public Stand::VectorBase<2, rage::Vector2>
	{
		float x = 0.0f;
		float y = 0.0f;

		constexpr Vector2() noexcept = default;

		constexpr Vector2(float x, float y) noexcept
			: x(x), y(y)
		{
		}

		[[nodiscard]] constexpr bool isInBounds() const noexcept
		{
			return x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f;
		}
	};
	static_assert(sizeof(Vector2) == sizeof(float) * 2);

	struct scrVector3;
}

namespace Stand
{
	template<typename T>
	struct Vector3Base : soup::Vector3Base<T, VectorBase<3, T>>
	{
		template <typename T2>
		[[nodiscard]] float fov_to(const T2& b) const
		{
			const float fov = (this->distance(b) * 2.0f);
			return fov >= 44.0f ? 1.0f : 45.0f - fov;
		}

		[[nodiscard]] rage::Vector2 getScreenPos() const
		{
			rage::Vector2 screen_pos;
			v_world2screen(((const T*)this)->x, ((const T*)this)->y, ((const T*)this)->z, &screen_pos.x, &screen_pos.y);
			return screen_pos;
		}

		[[nodiscard]] rage::Vector2 getScreenPosIgnoreOrigin() const
		{
			if (this->isNull())
			{
				return { -1.0f, 1.0f };
			}
			return getScreenPos();
		}
	};

	struct v3 : public Vector3Base<v3>
	{
		float x{};
		float y{};
		float z{};

		constexpr v3() noexcept = default;

		constexpr v3(float x, float y, float z = 0.0f) noexcept
			: x(x), y(y), z(z)
		{
		}

		constexpr v3(const soup::Vector3& b) noexcept
			: x(b.x), y(b.y), z(b.z)
		{
		}

		v3(const rage::scrVector3& b) noexcept;
	};
	static_assert(sizeof(v3) == sizeof(float) * 3);
}

namespace rage
{
	struct Vector3 : public Stand::v3
	{
		float w{};

		using Stand::v3::v3;

		constexpr Vector3(float x, float y, float z, float w)
			: v3(x, y, z), w(w)
		{
		}

		void operator=(const Stand::v3& b) noexcept
		{
			x = b.x;
			y = b.y;
			z = b.z;
		}
	};
	static_assert(sizeof(Vector3) == sizeof(float) * 4);

	struct Vector4
	{
		float x{};
		float y{};
		float z{};
		float w{};

		[[nodiscard]] int GetWi() const noexcept
		{
			return *reinterpret_cast<const int*>(&w);
		}
	};
	static_assert(sizeof(Vector4) == sizeof(float) * 4);
#pragma pack(pop)

	using Vec3V = Vector3;
	using Vec4V = Vector4;

	struct scrVector3 : public Stand::Vector3Base<rage::scrVector3>
	{
		alignas(8) float x, y, z;

		scrVector3() noexcept = default;

		scrVector3(float x, float y, float z = 0.0f) noexcept
			: x(x), y(y), z(z)
		{
		}

		explicit scrVector3(const Stand::v3& vec) noexcept;

		~scrVector3() noexcept = default;

		void operator=(const scrVector3& b) noexcept;
	};
	static_assert(sizeof(scrVector3) == 0x18);

	struct scrVector4
	{
		alignas(8) float x, y, z, w;

		scrVector4() noexcept = default;

		scrVector4(float x, float y, float z, float w) noexcept
			: x(x), y(y), z(z), w(w)
		{
		}

		~scrVector4() noexcept = default;
	};
	static_assert(sizeof(scrVector4) == 0x20);
}
