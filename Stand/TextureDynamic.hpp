#pragma once

#include "TextureBase.hpp"

namespace Stand
{
	struct TextureDynamic : public TextureBase
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex{};
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv{};

		[[nodiscard]] bool hasUnderlyingTexture() const;
		void createUnderlyingTexture(UINT width, UINT height);
		void ensureHasUnderlyingTextureOfSize(UINT width, UINT height);

		[[nodiscard]] void fromBackbuffer(const RECT& r);

		// These can't be called while in a draw context
		void drawOnto(const std::function<void()>& draw_func, const bool change_viewport = false) const;
		void blur(TextureDynamic& tmp, const uint8_t passes);

		[[nodiscard]] operator ID3D11ShaderResourceView* ();

		void reset();
	};
}
