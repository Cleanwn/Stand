#include "TextureBase.hpp"

namespace Stand
{
	void TextureBase::measure(ID3D11Resource* rsrc)
	{
		ID3D11Texture2D* pTextureInterface;
		rsrc->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
		{
			D3D11_TEXTURE2D_DESC desc;
			pTextureInterface->GetDesc(&desc);
			this->width = desc.Width;
			this->height = desc.Height;
		}
		pTextureInterface->Release();
		rsrc->Release();
	}

	float TextureBase::negotiateRenderHeight(const float height) const noexcept
	{
		const auto fMyHeight = float(this->height);
		if (height > fMyHeight)
		{
			return fMyHeight;
		}
		return height;
	}

	float TextureBase::getRenderWidth(const float height) const noexcept
	{
		return float(this->width) * (height / float(this->height));
	}

	float TextureBase::getRenderHeight(const float width) const noexcept
	{
		return float(this->height) * (width / float(this->width));
	}
}
