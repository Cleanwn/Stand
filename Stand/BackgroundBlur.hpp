#pragma once

#include <soup/TransientToken.hpp>

#include "TextureDynamic.hpp"

namespace Stand
{
	class BackgroundBlur
	{
	private:
		TextureDynamic tex;
		TextureDynamic blurTmpTex;
		
	public:
		void drawH(float x, float y, float width, float height, uint8_t passes);
		void drawC(float x, float y, float width, float height, uint8_t passes);
		void reset();
	};

	class EphemeralBackgroundBlur : public BackgroundBlur
	{
	public:
		soup::TransientToken transient_token;
	};
}
