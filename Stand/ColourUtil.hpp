#pragma once

#include "dx_common.hpp"

namespace Stand
{
	struct ColourUtil
	{
		[[nodiscard]] static DirectX::SimpleMath::Vector3 rgb_to_hsv(const DirectX::SimpleMath::Vector3& rgb);
		[[nodiscard]] static DirectX::SimpleMath::Vector3 hsv_to_rgb(const DirectX::SimpleMath::Vector3& hsv);

		template <typename T>
		[[nodiscard]] static double luminance(const T& rgb)
		{
			const double r = rgb.x <= 0.03928 ? rgb.x / 12.92 : pow((rgb.x + 0.055) / 1.055, 2.4);
			const double g = rgb.y <= 0.03928 ? rgb.y / 12.92 : pow((rgb.y + 0.055) / 1.055, 2.4);
			const double b = rgb.z <= 0.03928 ? rgb.z / 12.92 : pow((rgb.z + 0.055) / 1.055, 2.4);
			return (r * 0.2126) + (g * 0.7152) + (b * 0.0722);
		}

		template <typename T>
		[[nodiscard]] static double contrast(const T& rgb1, const T& rgb2)
		{
			const double lum1 = luminance(rgb1);
			const double lum2 = luminance(rgb2);
			return (std::max(lum1, lum2) + 0.05) / (std::min(lum1, lum2) + 0.05);
		}

		template <typename T>
		[[nodiscard]] static bool isContrastSufficient(const T& rgb1, const T& rgb2)
		{
			return contrast(rgb1, rgb2) > 3.0;
		}

		template <typename T>
		static void negateIfInsufficientContrast(T& rgb1, const T& rgb2)
		{
			if (!isContrastSufficient(rgb1, rgb2))
			{
				rgb1.Negate();
			}
		}
	};
}
