#pragma once

#include "TextureBase.hpp"

namespace Stand
{
	struct TextureArray : public TextureBase
	{
		std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textures;
	};
}
