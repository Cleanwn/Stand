#include "as_fraction.hpp"

#include <valarray>

namespace Stand
{
	std::pair<int, int> as_fraction(double number)
	{
		constexpr double precision = 5e-4;

		int sign = number >= 0 ? 1 : -1;
		number = number * sign;
		double new_number, whole_part;
		double decimal_part = number - (int)number;

		std::valarray<double> vec_1{ double((int)number), 1 }, vec_2{ 1,0 }, temporary;

		while (decimal_part > precision)
		{
			new_number = 1 / decimal_part;
			whole_part = (int)new_number;

			temporary = vec_1;
			vec_1 = whole_part * vec_1 + vec_2;
			vec_2 = temporary;

			decimal_part = new_number - whole_part;
		}

		return { int(sign * vec_1[0]), int(vec_1[1]) };
	}
}
