#pragma once

#include <wrl/client.h>

#pragma comment(lib, "d3d11.lib")
#include <D3D11.h>

#include <CommonStates.h>
#include <DDSTextureLoader.h>
#include <DirectXHelpers.h>
#include <Effects.h>
#include <GamePad.h>
#include <GeometricPrimitive.h>
#include <PostProcess.h>
#include <PrimitiveBatch.h>
#include <ScreenGrab.h>
#include <SimpleMath.h>
#include <SpriteFont.h>
#include <SpriteBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>

#define TEXT_HEIGHT_PX 0.021875

namespace Stand
{
	using namespace DirectX;
	using namespace Microsoft::WRL;

	template <typename T>
	inline void SafeRelease(T*& p)
	{
		if (p != nullptr)
		{
			p->Release();
			p = nullptr;
		}
	}

	// https://github.com/microsoft/DirectXTK/blob/main/Src/PlatformHelpers.h#L35

	class com_exception : public std::exception
	{
	public:
		com_exception(HRESULT hr) noexcept : result(hr) {}

		const char* what() const noexcept override
		{
			static char s_str[64] = {};
			sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
			return s_str;
		}

		HRESULT get_result() const noexcept { return result; }

	private:
		HRESULT result;
	};

	inline void ThrowIfFailed(HRESULT hr) noexcept(false)
	{
		if (FAILED(hr))
		{
			throw com_exception(hr);
		}
	}
}
