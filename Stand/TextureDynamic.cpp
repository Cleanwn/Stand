#include "TextureDynamic.hpp"

#include "Renderer.hpp"

namespace Stand
{
	bool TextureDynamic::hasUnderlyingTexture() const
	{
		return tex;
	}

	void TextureDynamic::createUnderlyingTexture(UINT width, UINT height)
	{
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = this->width = width;
		desc.Height = this->height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		g_renderer.m_pDevice->CreateTexture2D(&desc, nullptr, tex.ReleaseAndGetAddressOf());
		srv.Reset();
	}

	void TextureDynamic::ensureHasUnderlyingTextureOfSize(UINT width, UINT height)
	{
		if (!hasUnderlyingTexture() || this->width != width || this->height != height)
		{
			if (width > g_renderer.client_size.x)
			{
				width = (UINT)g_renderer.client_size.x;
			}
			if (height > g_renderer.client_size.y)
			{
				height = (UINT)g_renderer.client_size.y;
			}
			createUnderlyingTexture(width, height);
		}
	}

	void TextureDynamic::fromBackbuffer(const RECT& r)
	{
		ensureHasUnderlyingTextureOfSize((r.right - r.left), (r.bottom - r.top));

		ID3D11Resource* backbufferRes;
		g_renderer.m_pRenderTargetView->GetResource(&backbufferRes);

		ID3D11Texture2D* backbufferTex = nullptr;
		backbufferRes->QueryInterface(__uuidof(ID3D11Texture2D), (LPVOID*)&backbufferTex);
		backbufferRes->Release();

		D3D11_BOX box{};
		box.left = r.left;
		box.right = r.right;
		box.top = r.top;
		box.bottom = r.bottom;
		box.front = 0;
		box.back = 1;
		g_renderer.m_pContext->CopySubresourceRegion(tex.Get(), 0, 0, 0, 0, backbufferTex, 0, &box);

		backbufferTex->Release();
	}

	void TextureDynamic::drawOnto(const std::function<void()>& draw_func, const bool change_viewport) const
	{
		// Store original VP
		UINT nvp = 1;
		D3D11_VIEWPORT og_vp;
		if (change_viewport)
		{
			g_renderer.m_pContext->RSGetViewports(&nvp, &og_vp);
		}

		// Create RTV for texture
		ID3D11RenderTargetView* render_target;
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;
		SOUP_IF_LIKELY (g_renderer.m_pDevice->CreateRenderTargetView(tex.Get(), &desc, &render_target) == S_OK)
		{
			// Clear texture RTV
			FLOAT clr[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			g_renderer.m_pContext->ClearRenderTargetView(render_target, clr);

			// Switch states & draw
			g_renderer.m_pContext->OMSetRenderTargets(1, &render_target, nullptr);
			if (change_viewport)
			{
				D3D11_VIEWPORT vp{};
				vp.Width = (FLOAT)width;
				vp.Height = (FLOAT)height;
				g_renderer.m_pContext->RSSetViewports(1, &vp);
			}
			draw_func();

			// Restore original RTV & VP
			g_renderer.m_pContext->OMSetRenderTargets(1, &g_renderer.m_pRenderTargetView, nullptr);
			if (change_viewport)
			{
				g_renderer.m_pContext->RSSetViewports(1, &og_vp);
			}

			// Release texture RTV
			render_target->Release();
		}
	}

	void TextureDynamic::blur(TextureDynamic& tmp, const uint8_t passes)
	{
		tmp.ensureHasUnderlyingTextureOfSize(width, height);

		const auto draw_func = []
		{
			g_renderer.postProcess->SetEffect(BasicPostProcess::GaussianBlur_5x5);
			g_renderer.postProcess->Process(g_renderer.m_pContext);
		};

		g_renderer.postProcess->SetSourceTexture(*this);
		for (uint8_t i = 0; i < passes; ++i)
		{
			tmp.drawOnto(draw_func, true);
			g_renderer.postProcess->SetSourceTexture(tmp);
			this->drawOnto(draw_func, true);
			g_renderer.postProcess->SetSourceTexture(*this);
		}
	}

	TextureDynamic::operator ID3D11ShaderResourceView* ()
	{
		if (!srv)
		{
			g_renderer.m_pDevice->CreateShaderResourceView(tex.Get(), nullptr, srv.GetAddressOf());
		}
		return srv.Get();
	}

	void TextureDynamic::reset()
	{
		tex.Reset();
		srv.Reset();
	}
}
