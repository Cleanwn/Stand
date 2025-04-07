#pragma once

#include "TextureBase.hpp"

namespace Stand
{
	struct Texture : public TextureBase
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;

		operator ID3D11ShaderResourceView* () const
		{
			return this->srv.Get();
		}
	};
}
