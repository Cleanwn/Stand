#include "ColourUtil.hpp"

namespace Stand
{
	DirectX::SimpleMath::Vector3 ColourUtil::rgb_to_hsv(const DirectX::SimpleMath::Vector3& rgb)
	{
		// https://stackoverflow.com/a/6930407/4796321

		DirectX::SimpleMath::Vector3 hsv;

		double min = rgb.x < rgb.y ? rgb.x : rgb.y;
		min = min < rgb.z ? min : rgb.z;

		double max = rgb.x > rgb.y ? rgb.x : rgb.y;
		max = max > rgb.z ? max : rgb.z;

		min = rgb.x < rgb.y ? rgb.x : rgb.y;
		min = min < rgb.z ? min : rgb.z;

		max = rgb.x > rgb.y ? rgb.x : rgb.y;
		max = max > rgb.z ? max : rgb.z;

		hsv.z = (float)max; // value
		double delta = max - min;
		if (delta < 0.00001)
		{
			hsv.y = 0;
			hsv.x = 0; // undefined, maybe nan?
			return hsv;
		}
		if (max > 0.0)
		{
			// NOTE: if Max is == 0, this divide would cause a crash
			hsv.y = (float)(delta / max); // saturation
		}
		else
		{
			// if max is 0, then r = g = b = 0              
			// s = 0, h is undefined
			hsv.y = 0.0;
			hsv.x = 0.0; // out.x = NAN;
			return hsv;
		}
		if (rgb.x >= max) // > is bogus, just keeps compilor happy
		{
			// between yellow & magenta
			hsv.x = (float)(((double)rgb.y - (double)rgb.z) / delta);
		}
		else if (rgb.y >= max)
		{
			// between cyan & yellow
			hsv.x = (float)(2.0 + ((double)rgb.z - (double)rgb.x) / delta);
		}
		else
		{
			// between magenta & cyan
			hsv.x = (float)(4.0 + ((double)rgb.x - (double)rgb.y) / delta);
		}

		hsv.x *= 60.0; // hue degrees

		if (hsv.x < 0.0)
		{
			hsv.x += 360.0;
		}

		return hsv;
	}

	DirectX::SimpleMath::Vector3 ColourUtil::hsv_to_rgb(const DirectX::SimpleMath::Vector3& hsv)
	{
		// https://stackoverflow.com/a/6930407/4796321

		DirectX::SimpleMath::Vector3 rgb;

		if (hsv.y <= 0.0) // < is bogus, just shuts up warnings
		{
			rgb.x = hsv.z;
			rgb.y = hsv.z;
			rgb.z = hsv.z;
			return rgb;
		}

		double hh = hsv.x;
		while (hh >= 360.0)
		{
			hh -= 360.0;
		}
		hh /= 60.0;
		long i = (long)hh;
		double ff = hh - i;
		double p = hsv.z * (1.0 - hsv.y);
		double q = hsv.z * (1.0 - (hsv.y * ff));
		double t = hsv.z * (1.0 - (hsv.y * (1.0 - ff)));

		switch (i)
		{
		case 0:
			rgb.x = hsv.z;
			rgb.y = (float)t;
			rgb.z = (float)p;
			break;
		case 1:
			rgb.x = (float)q;
			rgb.y = hsv.z;
			rgb.z = (float)p;
			break;
		case 2:
			rgb.x = (float)p;
			rgb.y = hsv.z;
			rgb.z = (float)t;
			break;

		case 3:
			rgb.x = (float)p;
			rgb.y = (float)q;
			rgb.z = hsv.z;
			break;
		case 4:
			rgb.x = (float)t;
			rgb.y = (float)p;
			rgb.z = hsv.z;
			break;
		case 5:
		default:
			rgb.x = hsv.z;
			rgb.y = (float)p;
			rgb.z = (float)q;
			break;
		}

		return rgb;
	}
}
