#pragma once

#include "dx_common.hpp"

namespace Stand
{
	struct TextureBase
	{
		UINT width;
		UINT height;

		void measure(ID3D11Resource* rsrc);

		[[nodiscard]] float negotiateRenderHeight(const float height) const noexcept;

		[[nodiscard]] float getRenderWidth(const float height) const noexcept;
		[[nodiscard]] float getRenderHeight(const float width) const noexcept;
	};
}
