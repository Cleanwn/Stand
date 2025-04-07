#pragma once

#include <soup/CidrSubnet4.hpp>
#include <soup/CidrSupernet.hpp>

namespace Stand
{
#pragma pack(push, 1)
	struct CidrSupernets
	{
		inline static constexpr soup::CidrSupernet take_two = {
			std::array {
				// https://ipinfo.io/AS46555#blocks
				soup::CidrSubnet4(SOUP_IPV4(104, 255, 104, 0), 22),
				soup::CidrSubnet4(SOUP_IPV4(104, 255, 105, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(104, 255, 106, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(104, 255, 107, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(192, 81, 240, 0), 22),
				soup::CidrSubnet4(SOUP_IPV4(192, 81, 241, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(192, 81, 242, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(192, 81, 243, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(192, 81, 244, 0), 22),
				soup::CidrSubnet4(SOUP_IPV4(192, 81, 245, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(192, 81, 246, 0), 24),
				// https://ipinfo.io/AS394977#blocks
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 224, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 225, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 226, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 227, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 228, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 229, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 231, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 232, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 233, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 234, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 235, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 236, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 237, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 238, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 239, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 240, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 241, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 242, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 244, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 245, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 246, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 247, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 253, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 254, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(139, 138, 255, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(199, 168, 61, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(199, 168, 62, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(199, 229, 224, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(209, 204, 240, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(209, 204, 241, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(209, 204, 242, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(209, 204, 244, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(209, 204, 245, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(209, 204, 246, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(209, 204, 248, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(209, 204, 249, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(209, 204, 250, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(209, 204, 252, 0), 24),
			}
		};

		inline static constexpr soup::CidrSupernet fbi = {
			std::array {
				// https://db-ip.com/as25996
				soup::CidrSubnet4(SOUP_IPV4(149, 101, 100, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(149, 101, 101, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(149, 101, 102, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(149, 101, 103, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(149, 101, 118, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(149, 101, 164, 0), 23),
				soup::CidrSubnet4(SOUP_IPV4(149, 101, 216, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(149, 101, 229, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(149, 101, 242, 0), 24),
				soup::CidrSubnet4(SOUP_IPV4(153, 31, 0, 0), 17),
				soup::CidrSubnet4(SOUP_IPV4(153, 31, 128, 0), 18),
				soup::CidrSubnet4(SOUP_IPV4(153, 31, 192, 0), 18),
			}
		};

		inline static constexpr soup::CidrSupernet cia = {
			std::array {
				// https://ipinfo.io/AS7046/198.81.128.0/18
				soup::CidrSubnet4(SOUP_IPV4(198, 81, 128, 0), 18),
			}
		};

		inline static constexpr soup::CidrSupernet residential_austria = {
			std::array {
				soup::CidrSubnet4(SOUP_IPV4(194, 166, 250, 0), 24),
			}
		};

		inline static constexpr soup::CidrSupernet residential_germany = {
			std::array {
				soup::CidrSubnet4(SOUP_IPV4(84, 56, 218, 0), 24),
			}
		};

		inline static constexpr soup::CidrSupernet residential_italy = {
			std::array {
				soup::CidrSubnet4(SOUP_IPV4(5, 89, 190, 0), 24),
			}
		};

		inline static constexpr soup::CidrSupernet residential_netherlands = {
			std::array {
				soup::CidrSubnet4(SOUP_IPV4(217, 123, 124, 0), 24),
			}
		};

		inline static constexpr soup::CidrSupernet residential_uk = {
			std::array {
				soup::CidrSubnet4(SOUP_IPV4(81, 109, 120, 0), 24),
			}
		};

		inline static constexpr soup::CidrSupernet _private = {
			std::array {
				soup::CidrSubnet4(SOUP_IPV4(192, 168, 0, 0), 16),
				soup::CidrSubnet4(SOUP_IPV4(10, 0, 0, 0), 8),
			}
		};

		inline static constexpr soup::CidrSupernet invalid = {
			std::array {
				soup::CidrSubnet4(SOUP_IPV4(0, 0, 0, 0), 8),
				soup::CidrSubnet4(SOUP_IPV4(127, 0, 0, 0), 8),
				soup::CidrSubnet4(SOUP_IPV4(255, 255, 255, 255), 32),
			}
		};
	};
#pragma pack(pop)

	static_assert(CidrSupernets::take_two.contains(SOUP_IPV4(0, 0, 0, 0)) == false);
	static_assert(CidrSupernets::take_two.contains(SOUP_IPV4(1, 1, 1, 1)) == false);
	static_assert(CidrSupernets::take_two.contains(SOUP_IPV4(255, 255, 255, 255)) == false);
	static_assert(CidrSupernets::take_two.contains(SOUP_IPV4(91, 53, 213, 226)) == false);
	static_assert(CidrSupernets::take_two.contains(SOUP_IPV4(85, 249, 212, 18)) == false);
	static_assert(CidrSupernets::take_two.contains(SOUP_IPV4(104, 255, 104, 69)) == true);
	static_assert(CidrSupernets::take_two.contains(SOUP_IPV4(209, 204, 244, 132)) == true);
	static_assert(CidrSupernets::take_two.contains(SOUP_IPV4(139, 138, 227, 0)) == true);

	static_assert(CidrSupernets::_private.contains(SOUP_IPV4(192, 168, 2, 1)) == true);
	static_assert(CidrSupernets::_private.contains(SOUP_IPV4(10, 10, 10, 10)) == true);
	static_assert(CidrSupernets::_private.contains(SOUP_IPV4(0, 0, 0, 0)) == false);
	static_assert(CidrSupernets::_private.contains(SOUP_IPV4(1, 1, 1, 1)) == false);
	static_assert(CidrSupernets::_private.contains(SOUP_IPV4(255, 255, 255, 255)) == false);
	static_assert(CidrSupernets::_private.contains(SOUP_IPV4(91, 53, 213, 226)) == false);
	static_assert(CidrSupernets::_private.contains(SOUP_IPV4(85, 249, 212, 18)) == false);
	static_assert(CidrSupernets::_private.contains(SOUP_IPV4(104, 255, 104, 69)) == false);
	static_assert(CidrSupernets::_private.contains(SOUP_IPV4(209, 204, 244, 132)) == false);
	static_assert(CidrSupernets::_private.contains(SOUP_IPV4(139, 138, 227, 0)) == false);
}
