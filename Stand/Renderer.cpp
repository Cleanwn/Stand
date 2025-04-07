#include "Renderer.hpp"

#include <memory>

#include <fmt/core.h>

#include <soup/unicode.hpp>

#include "AntiAntiCheat.hpp"
#include "bin.hpp"
#include "Chat.hpp"
#include "CMultiplayerChat.hpp"
#include "ColourUtil.hpp"
#include "Commandbox.hpp"
#include "CommandboxGrid.hpp"
#include "conf.hpp"
#include "ContextMenu.hpp"
#include "Exceptional.hpp"
#include "ExecCtx.hpp"
#include "FiberPool.hpp"
#include "font_bevietnamprolight.hpp"
#include "font_nanumgothic.hpp"
#include "font_yahei.hpp"
#include "get_current_time_millis.hpp"
#include "Gui.hpp"
#include "homoglyphs.hpp"
#include "huddecl.hpp"
#include "input.hpp"
#include "is_session.hpp"
#include "FileLogger.hpp"
#include "MenuGrid.hpp"
#include "Metrics.hpp"
#include "natives.hpp"
#include "NotifyGrid.hpp"
#include "pointers.hpp"
#include "Pong.hpp"
#include "regular_event.hpp"
#include "ScriptMgr.hpp"
#include "StringUtils.hpp"
#include "TabCompletionProvider.hpp"
#include "tbScreenshotMode.hpp"
#include "Tutorial.hpp"
#include "TutorialGrid.hpp"
#include "UnicodePrivateUse.hpp"
#include "Util.hpp"

#if DRAW_HTML
#include <soup/lyoDocument.hpp>
#include <soup/lyoFlatDocument.hpp>
#include "RenderTarget.hpp"
#endif

#define VK_NUMPAD_RETURN 0x0E

namespace Stand
{
	[[nodiscard]] static float scale_image(const float width, const float height, const float scalex, const float scaley)
	{
		float ret = 1.0f;
		if (width > height)
		{
			ret = width * scalex;
		}
		if (height > width)
		{
			ret = height * scaley;
		}
		return ret;
	}

	void DrawSpriteData::draw() const
	{
		EXCEPTIONAL_LOCK(g_renderer.script_textures_mtx)
		auto tex = g_renderer.script_textures.find(tex_id);
		if (tex != g_renderer.script_textures.end())
		{
			g_renderer.m_pSpriteBatch->Draw(
				tex->second,
				DirectX::XMFLOAT2(posX * g_renderer.client_size.x, posY * g_renderer.client_size.y),
				nullptr,
				g_renderer.propagateAlpha(DirectX::SimpleMath::Color(r, g, b, a)),
				(rotation * 2.0f * (float)M_PI),
				DirectX::XMFLOAT2(centerX * tex->second.width, centerY * tex->second.height),
				scale_image(g_renderer.client_size.x / tex->second.width, g_renderer.client_size.y / tex->second.height, sizeX * 2.0f, sizeY * 2.0f),
				DirectX::SpriteEffects::SpriteEffects_None
			);
		}
		EXCEPTIONAL_UNLOCK(g_renderer.script_textures_mtx)
	}

	Circle::Circle(size_t resolution)
		: radius(1.0f), points({})
	{
		for (size_t i = 0; i <= resolution; i++)
		{
			points.emplace_back(DirectX::SimpleMath::Vector2(cosf(float(M_PI) * 2.0f * i / resolution), sinf(float(M_PI) * 2.0f * i / resolution)));
		}
	}

	Circle Circle::inst_50 = Circle{ 50 };
	Circle Circle::inst_100 = Circle{ 100 };

	void Circle::setRadius(float r)
	{
		float factor = (r / radius);
		for (auto& point : points)
		{
			point *= factor;
		}
		radius = r;
	}

	const DirectX::SimpleMath::Color* Renderer::getBgRectColourPtr() const
	{
		return &bgRectColour;
	}

	const DirectX::SimpleMath::Color& Renderer::getBgRectColour() const
	{
		return *getBgRectColourPtr();
	}

	const DirectX::SimpleMath::Color* Renderer::getFocusRectColourPtr() const
	{
		return &focusRectColour;
	}

	const DirectX::SimpleMath::Color& Renderer::getFocusRectColour() const
	{
		return *getFocusRectColourPtr();
	}

	const DirectX::SimpleMath::Color& Renderer::getFocusTextColour() const
	{
		return focusTextColour;
	}

	const DirectX::SimpleMath::Color& Renderer::getFocusRightTextColour() const
	{
		return focusRightTextColour;
	}

	const DirectX::SimpleMath::Color& Renderer::getFocusSpriteColour() const
	{
		return focusSpriteColour;
	}

	const DirectX::SimpleMath::Color& Renderer::getBgTextColour() const
	{
		return bgTextColour;
	}

	const DirectX::SimpleMath::Color& Renderer::getBgRightTextColour() const
	{
		return bgRightTextColour;
	}

	const DirectX::SimpleMath::Color& Renderer::getBgSpriteColour() const
	{
		return bgSpriteColour;
	}

	int16_t Renderer::getCursorBorderWidth() const
	{
		return cursor_border_width;
	}

	bool Renderer::getCursorBorderRounded() const
	{
		return cursor_border_rounded;
	}

	const DirectX::SimpleMath::Color& Renderer::getCursorBorderColour() const
	{
		return cursor_border_colour;
	}

	void Renderer::initialiseDevices()
	{
		auto theme_path = getThemePath();

		if (SUCCEEDED((*pointers::swapchain)->GetDevice(__uuidof(m_pDevice), (void**)&m_pDevice)))
		{
			m_pDevice->GetImmediateContext(&m_pContext);

			m_stateSaver = std::make_unique<StateSaver>();

			m_pCommonState = std::make_unique<CommonStates>(m_pDevice);

			m_pSpriteBatch = std::make_unique<SpriteBatch>(m_pContext);

			m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(m_pContext);

			m_effect = std::make_unique<BasicEffect>(m_pDevice);
			m_effect->SetVertexColorEnabled(true);

			void const* shaderByteCode;
			size_t byteCodeLength;

			m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

			if (!SUCCEEDED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements,
				VertexPositionColor::InputElementCount,
				shaderByteCode, byteCodeLength,
				m_inputLayout.ReleaseAndGetAddressOf())))
			{
				Util::toast("Failed to create input layout", TOAST_ALL);
			}

			postProcess = std::make_unique<BasicPostProcess>(m_pDevice);

			m_font_yahei = loadFont(getYahei());
			m_font_nanumgothic = loadFont(getNanumGothic());
			m_font_bevietnamprolight = loadFont(getBeVietnamProLight());
			loadUserFont();

			g_menu_grid.updateNow();
			g_commandbox_grid.updateNow();
		}
		else
		{
			Util::toast("Renderer Init Error 1", TOAST_ALL);
		}

		if (SUCCEEDED((*pointers::swapchain)->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_pRenderTargetTexture)))
		{
			m_pDevice->CreateRenderTargetView(m_pRenderTargetTexture, nullptr, &m_pRenderTargetView);
			m_pRenderTargetTexture->Release();

			reloadTextures(std::move(theme_path));

			EXCEPTIONAL_LOCK(script_textures_mtx)
			script_textures.clear();
			for (const auto& it : ReloadArray)
			{
				Texture t;
				ID3D11Resource* rsrc = nullptr;
				if (SUCCEEDED(CreateWICTextureFromFile(m_pDevice, it.name.c_str(), &rsrc, t.srv.ReleaseAndGetAddressOf(), 0)))
				{
					t.measure(rsrc);
					script_textures.emplace(it.id, std::move(t));
				}
				else
				{
					g_logger.log(std::move(std::string("Failed to reload texture ").append(fmt::to_string(it.id)).append(" from ").append(StringUtils::utf16_to_utf8(it.name))));
				}
			}
			EXCEPTIONAL_UNLOCK(script_textures_mtx)
		}
		else
		{
			Util::toast("Renderer Init Error 2", TOAST_ALL);
		}

		if (SUCCEEDED((*pointers::swapchain)->GetFullscreenState(&m_fullscreenState, nullptr)))
		{
			UINT nvp = 1;
			D3D11_VIEWPORT vp;
			m_pContext->RSGetViewports(&nvp, &vp);
			client_size = DirectX::SimpleMath::Vector2{ vp.Width, vp.Height };
			client_ratio = (client_size.x / client_size.y);
			resolution_text_scale = std::min(client_size.x / 1920.0f, client_size.y / 1080.0f);
			float expected_width = (client_size.y * (16.0f / 9.0f));
			if (client_size.x > expected_width)
			{
				hudCorrectionC.x = ((client_size.x - expected_width) / 2.0f);
				hudCorrectionC.y = 0.0f;
			}
			else
			{
				hudCorrectionC.x = 0.0f;
				float expected_height = (client_size.x * (9.0f / 16.0f));
				hudCorrectionC.y = ((client_size.y - expected_height) / 2.0f);
			}
		}
		else
		{
			Util::toast("Renderer Init Error 3", TOAST_ALL);
		}

		inited = true;
	}

	std::wstring Renderer::getThemePath()
	{
		auto theme_path = std::wstring(_wgetenv(L"appdata")).append(LR"(\Stand\Theme\)");
		if (!std::filesystem::is_directory(theme_path))
		{
			std::filesystem::create_directory(theme_path);
		}
		return theme_path;
	}

	bool Renderer::isUserFontAcceptableForLanguage() const
	{
		if (!m_font_user)
		{
			return false;
		}
		switch (Lang::active_id)
		{
		case LANG_DE:
			return m_font_user->ContainsCharacter(L'Ö');

		case LANG_PT:
			return m_font_user->ContainsCharacter(L'ã');

		case LANG_LT:
			return m_font_user->ContainsCharacter(L'š');

		case LANG_ZH:
			return m_font_user->ContainsCharacter(L'文');

		case LANG_KO:
			return m_font_user->ContainsCharacter(L'한');

		case LANG_RU:
			return m_font_user->ContainsCharacter(L'й');

		case LANG_FR:
			return m_font_user->ContainsCharacter(L'ç');

		case LANG_PL:
			return m_font_user->ContainsCharacter(L'Ś');

		case LANG_IT:
			return m_font_user->ContainsCharacter(L'ò');

		case LANG_JA:
			return m_font_user->ContainsCharacter(L'両');
			
		case LANG_VI:
			return m_font_user->ContainsCharacter(L'ả');
		}
		return true;
	}

	BinaryBlob Renderer::getYahei()
	{
		return {
			(const uint8_t*)font_yahei::chunk_1,
			sizeof(font_yahei::chunk_1)
		};
	}

	BinaryBlob Renderer::getNanumGothic()
	{
		return {
			(const uint8_t*)font_nanumgothic::chunk_1,
			sizeof(font_nanumgothic::chunk_1)
		};
	}

	BinaryBlob Renderer::getBeVietnamProLight()
	{
		return {
			(const uint8_t*)font_bevietnamprolight::chunk_1,
			sizeof(font_bevietnamprolight::chunk_1)
		};
	}

	int Renderer::getFontForLanguage()
	{
		if (Lang::active_id == LANG_KO)
		{
			return 1; // NanumGothic
		}
		if (Lang::active_id == LANG_VI)
		{
			return 2; // Be Vietnam Pro Light
		}
		return 0; // Microsoft YaHei
	}

	void Renderer::switchToPresetFont(int id)
	{
		if (id == 0)
		{
			m_font_user = m_font_yahei;
			writeFont(Renderer::getYahei());
		}
		else if (id == 1)
		{
			m_font_user = m_font_nanumgothic;
			writeFont(Renderer::getNanumGothic());
		}
		else if (id == 2)
		{
			m_font_user = m_font_bevietnamprolight;
			writeFont(Renderer::getBeVietnamProLight());
		}
		else
		{
			SOUP_UNREACHABLE;
		}
	}

	void Renderer::writeFont(const BinaryBlob& blob)
	{
		std::ofstream out(std::move(Renderer::getThemePath().append(L"Font.spritefont")), std::ios::binary | std::ios::out);
		out.write((const char*)blob.data, blob.size);
	}

	void Renderer::loadUserFont()
	{
		const std::wstring font_path = (getThemePath() + L"Font.spritefont");

		if (std::filesystem::exists(font_path))
		{
			try
			{
				m_font_user = soup::make_shared<SpriteFont>(m_pDevice, font_path.c_str());
				setupFont(*m_font_user);
				return;
			}
			catch (const std::exception& e)
			{
				Util::toast(std::move(std::string("Failed to load Font.spritefont: ").append(e.what())), TOAST_ALL);
			}
		}

		m_font_user = m_font_yahei;
	}

	soup::SharedPtr<SpriteFont> Renderer::loadFont(const BinaryBlob& blob)
	{
		soup::SharedPtr<SpriteFont> ret;
		try
		{
			ret = soup::make_shared<SpriteFont>(m_pDevice, blob.data, blob.size);
			setupFont(*ret);
		}
		catch (const std::exception& e)
		{
			Util::toast(std::move(std::string("Failed to load in-memory font: ").append(e.what())), TOAST_ALL);
		}
		return ret;
	}

	void Renderer::setupFont(SpriteFont& font)
	{
		font.SetDefaultCharacter(L'?');
	}

#define RELOAD_TEXTURE(filename, varname) g_renderer.reloadTexture(theme_path, L ## filename, g_renderer.varname, bin:: ## varname, sizeof(bin:: ## varname));
#define RELOAD_TEXTURE_FROM_MEMORY(varname) g_renderer.reloadTextureFromMemory(g_renderer.varname, bin:: ## varname, sizeof(bin:: ## varname));
#define RELOAD_TEXTURE_CUSTOM(filename, varname, binname) g_renderer.reloadTexture(theme_path, L ## filename, g_renderer.varname, bin:: ## binname, sizeof(bin:: ## binname));

	void Renderer::reloadTextures(std::wstring&& theme_path)
	{
		Exceptional::createManagedExceptionalThread(__FUNCTION__, [theme_path{std::move(theme_path)}]
		{
			RELOAD_TEXTURE("Toggle Off", textureToggleOff);
			RELOAD_TEXTURE("Toggle On", textureToggleOn);
			RELOAD_TEXTURE("Toggle Off Auto", textureToggleOffAuto);
			RELOAD_TEXTURE("Toggle On Auto", textureToggleOnAuto);
			RELOAD_TEXTURE("Toggle Off List", textureToggleOffList);
			RELOAD_TEXTURE("Toggle On List", textureToggleOnList);
			RELOAD_TEXTURE("List", textureList);
			RELOAD_TEXTURE("Link", textureLink);
			RELOAD_TEXTURE("Enabled", textureEnabled);
			RELOAD_TEXTURE("Disabled", textureDisabled);
			RELOAD_TEXTURE("User", textureUser);
			RELOAD_TEXTURE("Friends", textureFriends);
			RELOAD_TEXTURE("Users", textureUsers);
			RELOAD_TEXTURE("Edit", textureEdit);
			RELOAD_TEXTURE("Search", textureSearch);
			RELOAD_TEXTURE("Header Loading", textureHeaderLoading);

			if (g_renderer.textureWantedStar.width == 0)
			{
				RELOAD_TEXTURE_FROM_MEMORY(textureWantedStar);
				RELOAD_TEXTURE_FROM_MEMORY(textureLock);
				RELOAD_TEXTURE_FROM_MEMORY(textureRs);
				RELOAD_TEXTURE_FROM_MEMORY(textureRsVerified);
				RELOAD_TEXTURE_FROM_MEMORY(textureRsCreated);
				RELOAD_TEXTURE_FROM_MEMORY(textureBlankbox);
				RELOAD_TEXTURE_FROM_MEMORY(textureNewline);
				RELOAD_TEXTURE_FROM_MEMORY(textureReset);
				RELOAD_TEXTURE_FROM_MEMORY(textureUni0000);
				RELOAD_TEXTURE_FROM_MEMORY(textureUni26A0);
				RELOAD_TEXTURE_FROM_MEMORY(textureUni2728);
				RELOAD_TEXTURE_FROM_MEMORY(textureUni2764);
				RELOAD_TEXTURE_FROM_MEMORY(textureUni1F4AF);
				RELOAD_TEXTURE_FROM_MEMORY(textureUni1F60A);
				RELOAD_TEXTURE_FROM_MEMORY(textureUni1F480);
				RELOAD_TEXTURE_FROM_MEMORY(textureUni1F525);
				RELOAD_TEXTURE_FROM_MEMORY(textureUni1F602);
				RELOAD_TEXTURE_FROM_MEMORY(textureUni1F629);
				RELOAD_TEXTURE_FROM_MEMORY(textureUni1F633);
			}

			auto tabs_path = std::wstring(theme_path).append(L"\\Tabs");
			if (!std::filesystem::exists(tabs_path))
			{
				std::filesystem::create_directory(tabs_path);
			}
			RELOAD_TEXTURE("Tabs\\Self", textureTabSelf);
			RELOAD_TEXTURE("Tabs\\Vehicle", textureTabVehicle);
			RELOAD_TEXTURE("Tabs\\Online", textureTabOnline);
			RELOAD_TEXTURE_CUSTOM("Tabs\\Players", textureTabPlayers, textureUsers);
			RELOAD_TEXTURE("Tabs\\World", textureTabWorld);
			RELOAD_TEXTURE("Tabs\\Game", textureTabGame);
			RELOAD_TEXTURE("Tabs\\Stand", textureTabStand);

			auto custom_path = std::wstring(theme_path).append(L"\\Custom");
			if (!std::filesystem::exists(custom_path))
			{
				std::filesystem::create_directory(custom_path);
			}
			RELOAD_TEXTURE_CUSTOM("Custom\\00", textureCustom00, textureUni1F602);
			RELOAD_TEXTURE_CUSTOM("Custom\\01", textureCustom01, textureUni1F525);
			RELOAD_TEXTURE_CUSTOM("Custom\\02", textureCustom02, textureUni1F633);
			RELOAD_TEXTURE_CUSTOM("Custom\\03", textureCustom03, textureUni2764);
			RELOAD_TEXTURE_CUSTOM("Custom\\04", textureCustom04, textureUni1F480);
			RELOAD_TEXTURE_CUSTOM("Custom\\05", textureCustom05, textureUni1F60A);
			RELOAD_TEXTURE_CUSTOM("Custom\\06", textureCustom06, textureUni2728);
			RELOAD_TEXTURE_CUSTOM("Custom\\07", textureCustom07, textureUni1F629);
			RELOAD_TEXTURE_CUSTOM("Custom\\08", textureCustom08, textureUni1F602);
			RELOAD_TEXTURE_CUSTOM("Custom\\09", textureCustom09, textureUni1F525);
			RELOAD_TEXTURE_CUSTOM("Custom\\0A", textureCustom0A, textureUni1F633);
			RELOAD_TEXTURE_CUSTOM("Custom\\0B", textureCustom0B, textureUni2764);
			RELOAD_TEXTURE_CUSTOM("Custom\\0C", textureCustom0C, textureUni1F480);
			RELOAD_TEXTURE_CUSTOM("Custom\\0D", textureCustom0D, textureUni1F60A);
			RELOAD_TEXTURE_CUSTOM("Custom\\0E", textureCustom0E, textureUni2728);
			RELOAD_TEXTURE_CUSTOM("Custom\\0F", textureCustom0F, textureUni1F629);
			RELOAD_TEXTURE_CUSTOM("Custom\\10", textureCustom10, textureUni1F602);
			RELOAD_TEXTURE_CUSTOM("Custom\\11", textureCustom11, textureUni1F525);
			RELOAD_TEXTURE_CUSTOM("Custom\\12", textureCustom12, textureUni1F633);
			RELOAD_TEXTURE_CUSTOM("Custom\\13", textureCustom13, textureUni2764);
			RELOAD_TEXTURE_CUSTOM("Custom\\14", textureCustom14, textureUni1F480);
			RELOAD_TEXTURE_CUSTOM("Custom\\15", textureCustom15, textureUni1F60A);
			RELOAD_TEXTURE_CUSTOM("Custom\\16", textureCustom16, textureUni2728);
			RELOAD_TEXTURE_CUSTOM("Custom\\17", textureCustom17, textureUni1F629);
			RELOAD_TEXTURE_CUSTOM("Custom\\18", textureCustom18, textureUni1F602);
			RELOAD_TEXTURE_CUSTOM("Custom\\19", textureCustom19, textureUni1F525);
			RELOAD_TEXTURE_CUSTOM("Custom\\1A", textureCustom1A, textureUni1F633);
			RELOAD_TEXTURE_CUSTOM("Custom\\1B", textureCustom1B, textureUni2764);
			RELOAD_TEXTURE_CUSTOM("Custom\\1C", textureCustom1C, textureUni1F480);
			RELOAD_TEXTURE_CUSTOM("Custom\\1D", textureCustom1D, textureUni1F60A);
			RELOAD_TEXTURE_CUSTOM("Custom\\1E", textureCustom1E, textureUni2728);
			RELOAD_TEXTURE_CUSTOM("Custom\\1F", textureCustom1F, textureUni1F629);
		});
	}

	void Renderer::reloadTexture(const std::wstring& theme_path, const wchar_t* const name, Texture& texture, const char* const data, const size_t data_size)
	{
		auto texture_path = std::wstring(theme_path).append(name).append(L".png");
		if (Util::bin_file(texture_path, data, data_size)
			|| !reloadTexture(texture_path, texture)
			)
		{
			reloadTextureFromMemory(texture, data, data_size);
		}
	}

	bool Renderer::reloadTexture(const std::wstring& file, Texture& texture)
	{
		ID3D11Resource* rsrc = nullptr;
		try
		{
			ThrowIfFailed(CreateWICTextureFromFile(m_pDevice, file.c_str(), &rsrc, texture.srv.ReleaseAndGetAddressOf()));
		}
		catch (std::exception& e)
		{
			Util::toast(std::move(StringUtils::utf16_to_utf8(file).append(" failed to load: ").append(e.what())), TOAST_ALL);
			return false;
		}
		texture.measure(rsrc);
		return true;
	}

	void Renderer::reloadTextureFromMemory(Texture& texture, const char* const data, const size_t data_size)
	{
		ID3D11Resource* rsrc;
		if (!SUCCEEDED(CreateWICTextureFromMemory(m_pDevice, (const uint8_t*)data, data_size, &rsrc, texture.srv.ReleaseAndGetAddressOf())))
		{
			texture.width = 0;
			texture.height = 0;
			return;
		}
		texture.measure(rsrc);
	}

	bool Renderer::loadTexture(const std::wstring& file, TextureArray& arr)
	{
		Texture tex;
		if (!reloadTexture(file, tex))
		{
			return false;
		}
		if (arr.textures.empty())
		{
			arr.width = tex.width;
			arr.height = tex.height;
		}
		arr.textures.emplace_back(std::move(tex.srv));
		return true;
	}

	void Renderer::releaseDevices()
	{
		SOUP_IF_UNLIKELY (drawing)
		{
			Util::toast(LIT_OBF("Discarding request to release DirectX resources because draw is in progress."), TOAST_ALL);
			return;
		}

		inited = false;
		m_batch.reset();
		if (m_stateSaver)
		{
			m_stateSaver->releaseSavedState();
		}
		m_effect.reset();
		m_inputLayout.Reset();
		postProcess.reset();
		m_font_user.reset();
		m_font_yahei.reset();
		m_font_nanumgothic.reset();
		m_font_bevietnamprolight.reset();
		SafeRelease(m_pRenderTargetView);
		SafeRelease(m_pContext);
		SafeRelease(m_pDevice);
	}

	void Renderer::enterDrawContext() const
	{
		m_effect->Apply(m_pContext);

		m_pContext->IASetInputLayout(m_inputLayout.Get());

		enterSpriteDrawContext(m_pCommonState->AlphaBlend());
		m_batch->Begin();
	}

	void Renderer::leaveDrawContext() const
	{
		m_batch->End();
		leaveSpriteDrawContext();
	}

	void Renderer::enterSpriteDrawContextNonPremultiplied() const
	{
		enterSpriteDrawContext(m_pCommonState->NonPremultiplied());
	}

	void Renderer::leaveSpriteDrawContextNonPremultiplied() const noexcept
	{
		leaveSpriteDrawContext();

		// If we don't do this, blend state remains as NonPremultiplied.
		enterDrawContext();
		leaveDrawContext();
	}

	void Renderer::enterSpriteDrawContext(ID3D11BlendState* blendState) const
	{
		__try
		{
			m_pSpriteBatch->Begin(SpriteSortMode::SpriteSortMode_Deferred, blendState, nullptr, nullptr, m_pCommonState->CullNone());
		}
		__EXCEPTIONAL() // "Cannot nest Begin calls on a single SpriteBatch"
		{
			leaveSpriteDrawContext();
			enterSpriteDrawContext(blendState);
		}
	}

	void Renderer::leaveSpriteDrawContext() const noexcept
	{
		__try
		{
			m_pSpriteBatch->End();
		}
		__EXCEPTIONAL() // "Begin must be called before End"
		{
		}
	}

	bool Renderer::areTabsEnabled() const noexcept
	{
		return tabs_pos != NONE;
	}

	bool Renderer::doesGameplayStateAllowMenuNavigateInput() const noexcept
	{
		if (gameplayState == PLAYING || gameplayState == PAUSED || gameplayState == SUSPENDED)
		{
			return true;
		}
		if (gameplayState == MENUING)
		{
			return Input::keyboard_and_mouse;
		}
		return false;
	}

	bool Renderer::doesGameplayStateConflictWithVk(int vk) const noexcept
	{
		if (gameplayState == MENUING)
		{
			switch(vk)
			{
			case VK_BACK:
			case VK_RETURN:
			case VK_UP:
			case VK_DOWN:
			case VK_LEFT:
			case VK_RIGHT:
				return true;
			}
		}
		return false;
	}

	bool Renderer::doesGameplayStateAllowRendering() const noexcept
	{
		return gameplayState != OVERLAY;
	}

	bool Renderer::doesGameplayStateAllowScriptExecution() const noexcept
	{
		return gameplayState != SUSPENDED;
	}

	bool Renderer::isUltrawide() const noexcept
	{
		// Relying on non-ultrawide resolutions having widths that are more than half the height.
		return client_size.x > client_size.y * 2.0f;
	}

	bool Renderer::isSuperwide() const noexcept
	{
		return client_size.x > client_size.y * 3.0f;
	}

	/* static */ float Renderer::square_x_max() noexcept
	{
		return 1.0f;
	}

	float Renderer::square_y_max() const noexcept
	{
		return 1.0f / client_ratio;
	}

	DirectX::SimpleMath::Vector2 Renderer::sizeC2H(DirectX::SimpleMath::Vector2 v) const noexcept
	{
		v.x /= (client_size.x - (hudCorrectionC.x * 2.0f));
		v.y /= (client_size.y - (hudCorrectionC.y * 2.0f));
		v.x *= 1920.0f;
		v.y *= 1080.0f;
		return v;
	}

	DirectX::SimpleMath::Vector2 Renderer::sizeC2H(float x, float y) const noexcept
	{
		return sizeC2H(DirectX::SimpleMath::Vector2(x, y));
	}

	DirectX::SimpleMath::Vector2 Renderer::posC2H(DirectX::SimpleMath::Vector2 v) const noexcept
	{
		v -= hudCorrectionC;
		return sizeC2H(v);
	}

	DirectX::SimpleMath::Vector2 Renderer::posC2H(float x, float y) const noexcept
	{
		return posC2H(DirectX::SimpleMath::Vector2(x, y));
	}

	DirectX::SimpleMath::Vector2 Renderer::posC2H(const POINT& p) const noexcept
	{
		return posC2H(float(p.x), float(p.y));
	}

	DirectX::SimpleMath::Vector2 Renderer::sizeH2C(DirectX::SimpleMath::Vector2 v) const noexcept
	{
		v.x /= 1920.0f;
		v.y /= 1080.0f;
		v.x *= (client_size.x - (hudCorrectionC.x * 2.0f));
		v.y *= (client_size.y - (hudCorrectionC.y * 2.0f));
		return v;
	}

	DirectX::SimpleMath::Vector2 Renderer::sizeH2C(double x, double y) const noexcept
	{
		x /= 1920.0;
		y /= 1080.0;
		x *= (client_size.x - (hudCorrectionC.x * 2.0));
		y *= (client_size.y - (hudCorrectionC.y * 2.0));
		return DirectX::SimpleMath::Vector2((float)x, (float)y);
	}

	DirectX::SimpleMath::Vector2 Renderer::posH2C(DirectX::SimpleMath::Vector2 v) const noexcept
	{
		v = sizeH2C(v);
		v += hudCorrectionC;
		return v;
	}

	DirectX::SimpleMath::Vector2 Renderer::posH2C(double x, double y) const noexcept
	{
		auto v = sizeH2C(x, y);
		v += hudCorrectionC;
		return v;
	}

	DirectX::SimpleMath::Vector2 Renderer::C2S(DirectX::SimpleMath::Vector2 v) const noexcept
	{
		v.x /= client_size.x;
		v.y /= client_size.y;
		v.y /= client_ratio;
		v.y += ((1.0f - square_y_max()) * 0.5f);
		return v;
	}

	DirectX::SimpleMath::Vector2 Stand::Renderer::C2S(float x, float y) const noexcept
	{
		return C2S(DirectX::SimpleMath::Vector2(x, y));
	}

	DirectX::SimpleMath::Vector2 Renderer::S2C(DirectX::SimpleMath::Vector2 v) const noexcept
	{
		v.y -= ((1.0f - square_y_max()) * 0.5f);
		v.y *= client_ratio;
		v.y *= client_size.y;
		v.x *= client_size.x;
		return v;
	}

	DirectX::SimpleMath::Vector2 Renderer::S2C(float x, float y) const noexcept
	{
		return S2C(DirectX::SimpleMath::Vector2(x, y));
	}

	DirectX::SimpleMath::Vector2 Renderer::HP2H(DirectX::SimpleMath::Vector2 v) const noexcept
	{
		return DirectX::SimpleMath::Vector2(
			v.x * HUD_WIDTH,
			v.y * HUD_HEIGHT
		);
	}

	DirectX::SimpleMath::Vector2 Renderer::HP2H(float x, float y) const noexcept
	{
		return HP2H(DirectX::SimpleMath::Vector2(x, y));
	}

	void Renderer::HP2H(double& x, double& y) const noexcept
	{
		x *= HUD_WIDTH;
		y *= HUD_HEIGHT;
	}

	DirectX::SimpleMath::Vector2 Renderer::H2HP(DirectX::SimpleMath::Vector2 v) const noexcept
	{
		return DirectX::SimpleMath::Vector2(
			v.x / HUD_WIDTH,
			v.y / HUD_HEIGHT
		);
	}

	DirectX::SimpleMath::Vector2 Renderer::CP2C(DirectX::SimpleMath::Vector2 v) const noexcept
	{
		return DirectX::SimpleMath::Vector2(
			v.x * client_size.x,
			v.y * client_size.y
		);
	}

	DirectX::SimpleMath::Vector2 Renderer::CP2C(float x, float y) const noexcept
	{
		return CP2C(DirectX::SimpleMath::Vector2(x, y));
	}

	VertexPositionColor Renderer::S2VPC(float x, float y, const DirectX::SimpleMath::Color& colour) const noexcept
	{
		y -= 0.5f;
		y *= client_ratio;
		y += 0.5f;
		y *= 2.0f;
		y -= 1.0f;
		y *= -1.0f;
		return VertexPositionColor({ (x * 2.0f) - 1.0f, y, 0.0f }, propagateAlpha(colour));
	}

	/* static */ VertexPositionColor Renderer::CP2VPC(float x, float y, const DirectX::SimpleMath::Color& colour) noexcept
	{
		return VertexPositionColor({ (x * 2.0f) - 1.0f, (y * -2.0f) + 1.0f, 0.0f }, propagateAlpha(colour));
	}

	DirectX::SimpleMath::Color Renderer::convert_to_dx_colour(int r, int g, int b, int a) noexcept
	{
		return DirectX::SimpleMath::Color(float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f, float(a) / 255.0f);
	}

	DirectX::SimpleMath::Color Renderer::propagateAlpha(const DirectX::SimpleMath::Color& c) noexcept
	{
		return DirectX::SimpleMath::Color(c.R() * c.A(), c.G() * c.A(), c.B() * c.A(), c.A());
	}

	void Renderer::propagateAlpha(DirectX::SimpleMath::Color& c) noexcept
	{
		c.R(c.R() * c.A());
		c.G(c.G() * c.A());
		c.B(c.B() * c.A());
	}

	void Renderer::drawTriangleS(float a_x, float a_y, const DirectX::SimpleMath::Color& a_colour, float b_x, float b_y, const DirectX::SimpleMath::Color& b_colour, float c_x, float c_y, const DirectX::SimpleMath::Color& c_colour)
	{
		m_batch->DrawTriangle(
			S2VPC(a_x, a_y, a_colour),
			S2VPC(b_x, b_y, b_colour),
			S2VPC(c_x, c_y, c_colour)
		);
	}

	void Renderer::drawTriangleS(float a_x, float a_y, float b_x, float b_y, float c_x, float c_y, const DirectX::SimpleMath::Color& colour)
	{
		m_batch->DrawTriangle(
			S2VPC(a_x, a_y, colour),
			S2VPC(b_x, b_y, colour),
			S2VPC(c_x, c_y, colour)
		);
	}

	void Renderer::drawTriangleThisTickS(DirectX::SimpleMath::Vector2&& pos1, DirectX::SimpleMath::Vector2&& pos2, DirectX::SimpleMath::Vector2&& pos3, DirectX::SimpleMath::Color&& colour)
	{
		SOUP_ASSERT(ExecCtx::get().isScript());
		extras_buffer.emplace_back(std::make_unique<DrawTriangleData>(
			std::move(pos1),
			std::move(pos2),
			std::move(pos3),
			std::move(colour)
		));
	}

	void Renderer::drawTriangleThisTickC(const DirectX::SimpleMath::Vector2& pos1, const DirectX::SimpleMath::Vector2& pos2, const DirectX::SimpleMath::Vector2& pos3, DirectX::SimpleMath::Color&& colour)
	{
		return drawTriangleThisTickS(
			C2S(pos1),
			C2S(pos2),
			C2S(pos3),
			std::move(colour)
		);
	}

	void Renderer::drawTriangleThisTickHP(float a_x, float a_y, float b_x, float b_y, float c_x, float c_y, DirectX::SimpleMath::Color&& colour)
	{
		return drawTriangleThisTickC(
			posH2C(HP2H(a_x, a_y)),
			posH2C(HP2H(b_x, b_y)),
			posH2C(HP2H(c_x, c_y)),
			std::move(colour)
		);
	}

	void Renderer::drawTriangleThisTickCP(float a_x, float a_y, float b_x, float b_y, float c_x, float c_y, DirectX::SimpleMath::Color&& colour)
	{
		return drawTriangleThisTickC(
			CP2C(a_x, a_y),
			CP2C(b_x, b_y),
			CP2C(c_x, c_y),
			std::move(colour)
		);
	}

	void Renderer::drawHollowTriangleS(float a_x, float a_y, const DirectX::SimpleMath::Color& a_colour, float b_x, float b_y, const DirectX::SimpleMath::Color& b_colour, float c_x, float c_y, const DirectX::SimpleMath::Color& c_colour)
	{
		drawLineS(a_x, a_y, a_colour, b_x, b_y, b_colour);
		drawLineS(b_x, b_y, b_colour, c_x, c_y, c_colour);
		drawLineS(c_x, c_y, c_colour, a_x, a_y, a_colour);
	}

	void Renderer::drawHollowTriangleS(float a_x, float a_y, float b_x, float b_y, float c_x, float c_y, const DirectX::SimpleMath::Color& colour)
	{
		drawHollowTriangleS(a_x, a_y, colour, b_x, b_y, colour, c_x, c_y, colour);
	}

	void Renderer::drawRectByPointsS(float left_x, float top_y, float right_x, float bottom_y, const DirectX::SimpleMath::Color& top_left_colour, const DirectX::SimpleMath::Color& top_right_colour, const DirectX::SimpleMath::Color& bottom_left_colour, const DirectX::SimpleMath::Color& bottom_right_colour)
	{
		drawTriangleS(
			left_x, top_y, top_left_colour,
			right_x, top_y, top_right_colour,
			left_x, bottom_y, bottom_left_colour
		);
		drawTriangleS(
			right_x, top_y, top_right_colour,
			left_x, bottom_y, bottom_left_colour,
			right_x, bottom_y, bottom_right_colour
		);
	}

	void Renderer::drawRectByPointsS(float left_x, float top_y, float right_x, float bottom_y, const DirectX::SimpleMath::Color& colour)
	{
		drawTriangleS(
			left_x, top_y, colour,
			right_x, top_y, colour,
			left_x, bottom_y, colour
		);
		drawTriangleS(
			right_x, top_y, colour,
			left_x, bottom_y, colour,
			right_x, bottom_y, colour
		);
	}

	void Renderer::drawRectByPointsS(const DirectX::SimpleMath::Vector2& pos1, const DirectX::SimpleMath::Vector2& pos2, const DirectX::SimpleMath::Color& colour)
	{
		drawRectByPointsS(pos1.x, pos1.y, pos2.x, pos2.y, colour);
	}

	void Renderer::drawCircleS(float x, float y, const Circle& circle, const DirectX::SimpleMath::Color& inner_colour, const DirectX::SimpleMath::Color& outer_colour)
	{
		for (size_t i = 0; i < circle.points.size() - 1; i++)
		{
			drawTriangleS(
				x, y, inner_colour,
				x + circle.points.at(i).x, y + circle.points.at(i).y, outer_colour,
				x + circle.points.at(i + 1).x, y + circle.points.at(i + 1).y, outer_colour
			);
		}
	}

	void Renderer::drawCircleS(float x, float y, const Circle& circle, const DirectX::SimpleMath::Color& colour)
	{
		drawCircleS(x, y, circle, colour, colour);
	}

	void Renderer::drawCornerCircleS(float x, float y, const Circle& circle, const size_t offset, const DirectX::SimpleMath::Color& inner_colour, const DirectX::SimpleMath::Color& outer_colour)
	{
		const size_t corner_points = (circle.points.size() / 4);
		const size_t offset_i = (corner_points * offset);
		for (size_t i = offset_i; i < (offset_i + corner_points); i++)
		{
			drawTriangleS(
				x, y, inner_colour,
				x + circle.points.at(i).x, y + circle.points.at(i).y, outer_colour,
				x + circle.points.at(i + 1).x, y + circle.points.at(i + 1).y, outer_colour
			);
		}
	}

	void Renderer::drawCornerCircleS(float x, float y, const Circle& circle, const size_t offset, const DirectX::SimpleMath::Color& colour)
	{
		drawCornerCircleS(x, y, circle, offset, colour, colour);
	}

	void Renderer::drawHueCircleS(float x, float y, const Circle& circle)
	{
		for (size_t i = 0; i < circle.points.size() - 1; i++)
		{
			auto col1 = DirectX::SimpleMath::Color(ColourUtil::hsv_to_rgb(DirectX::SimpleMath::Vector3((float(i) / circle.points.size() * 360.0f) + 90.0f, 1.0f, 1.0f)));
			auto col2 = DirectX::SimpleMath::Color(ColourUtil::hsv_to_rgb(DirectX::SimpleMath::Vector3((float(i + 1) / circle.points.size() * 360.0f) + 90.0f, 1.0f, 1.0f)));
			drawTriangleS(
				x, y, DirectX::SimpleMath::Color(Colors::White),
				x + circle.points.at(i).x, y + circle.points.at(i).y, col1,
				x + circle.points.at(i + 1).x, y + circle.points.at(i + 1).y, col2
			);
		}
	}

	void Renderer::drawWireframeCircleS(float x, float y, const Circle& circle, const DirectX::SimpleMath::Color& inner_colour, const DirectX::SimpleMath::Color& outer_colour)
	{
		for (size_t i = 0; i < circle.points.size() - 1; i++)
		{
			drawHollowTriangleS(
				x, y, inner_colour,
				x + circle.points.at(i).x, y + circle.points.at(i).y, outer_colour,
				x + circle.points.at(i + 1).x, y + circle.points.at(i + 1).y, outer_colour
			);
		}
	}

	void Renderer::drawWireframeCircleS(float x, float y, const Circle& circle, const DirectX::SimpleMath::Color& colour)
	{
		drawWireframeCircleS(x, y, circle, colour, colour);
	}

	void Renderer::drawHollowCircleS(float x, float y, const Circle& circle, const DirectX::SimpleMath::Color& colour)
	{
		for (size_t i = 0; i < circle.points.size() - 1; i++)
		{
			drawLineS(
				x + circle.points.at(i).x, y + circle.points.at(i).y, colour,
				x + circle.points.at(i + 1).x, y + circle.points.at(i + 1).y, colour
			);
		}
	}

	void Renderer::drawCircleThisTickS(float x, float y, float radius, int part, const DirectX::SimpleMath::Color& colour)
	{
		SOUP_ASSERT(ExecCtx::get().isScript());
		extras_buffer.emplace_back(std::make_unique<DrawCircleData>(
			x,
			y,
			radius,
			part,
			colour
		));
	}

	int Renderer::createTexture(const char* texFileName)
	{
		int id;
		EXCEPTIONAL_LOCK(create_sprite_mtx)
		id = ++next_script_texture_id;
		SOUP_ASSERT(next_script_texture_id != 0); // This being 0 means we rolled over the int, which... by that point we're probably out of VRAM, but whatever.
		CreateTextureArray.emplace(id, StringUtils::utf8_to_utf16(texFileName));
		EXCEPTIONAL_UNLOCK(create_sprite_mtx)
		return id;
	}

	bool Renderer::isTextureStillInCreateQueue(int id)
	{
		bool res;
		EXCEPTIONAL_LOCK(create_sprite_mtx)
		res = (CreateTextureArray.find(id) != CreateTextureArray.end());
		EXCEPTIONAL_UNLOCK(create_sprite_mtx)
		return res;
	}

	Texture* Renderer::getScriptTexture(int id)
	{
		Texture* res = nullptr;
		EXCEPTIONAL_LOCK(g_renderer.script_textures_mtx)
		if (auto e = g_renderer.script_textures.find(id); e != g_renderer.script_textures.end())
		{
			res = &e->second;
		}
		EXCEPTIONAL_UNLOCK(g_renderer.script_textures_mtx)
		return res;
	}

	void Renderer::drawTextureTimedCP(int id, int time, float sizeX, float sizeY, float centerX, float centerY, float posX, float posY, float rotation, float r, float g, float b, float a)
	{
		EXCEPTIONAL_LOCK(timed_sprites_mtx)
		for (auto i = timed_sprites.begin(); i != timed_sprites.end(); )
		{
			if (i->draw_data.tex_id == id)
			{
				timed_sprites.erase(i);
				break;
			}
			++i;
		}
		timed_sprites.emplace_back(TimedSprite{
			get_current_time_millis() + time,
			DrawSpriteData(id, sizeX, sizeY, centerX, centerY, posX, posY, rotation, r, g, b, a)
		});
		EXCEPTIONAL_UNLOCK(timed_sprites_mtx)
	}

	void Renderer::drawTextureThisTickCP(int id, float sizeX, float sizeY, float centerX, float centerY, float posX, float posY, float rotation, float r, float g, float b, float a)
	{
		SOUP_ASSERT(ExecCtx::get().isScript());
		extras_buffer.emplace_back(std::make_unique<DrawSpriteData>(
			id,
			sizeX,
			sizeY,
			centerX,
			centerY,
			posX,
			posY,
			rotation,
			r,
			g,
			b,
			a
		));
	}

	void Renderer::drawSpriteByPointsThisTickHP(int id, float x1, float y1, float x2, float y2, const DirectX::SimpleMath::Color& colour)
	{
		SOUP_ASSERT(ExecCtx::get().isScript());
		auto pos1H = HP2H(x1, y1);
		auto pos2H = HP2H(x2, y2);
		extras_buffer.emplace_back(std::make_unique<DrawSpriteByPointsData>(
			id,
			pos1H.x, pos1H.y,
			pos2H.x, pos2H.y,
			colour
		));
	}

	void Renderer::drawSpriteByPointsC(ID3D11ShaderResourceView* texture, const RECT& rect, const DirectX::SimpleMath::Color& colour)
	{
		if (texture != nullptr)
		{
			m_pSpriteBatch->Draw(texture, rect, propagateAlpha(colour));
		}
	}

	void Renderer::drawSpriteByPointsC(ID3D11ShaderResourceView* texture, const LONG x1, const LONG y1, const LONG x2, const LONG y2, const DirectX::SimpleMath::Color& colour)
	{
		drawSpriteByPointsC(texture, RECT{
			x1,
			y1,
			x2,
			y2,
		}, colour);
	}

	void Renderer::drawSpriteC(ID3D11ShaderResourceView* texture, const LONG x, const LONG y, const LONG width, const LONG height, const DirectX::SimpleMath::Color& colour)
	{
		drawSpriteByPointsC(texture, x, y, x + width, y + height, colour);
	}

	void Renderer::drawSpriteC(ID3D11ShaderResourceView* texture, const DirectX::SimpleMath::Vector2& pos, const DirectX::SimpleMath::Vector2& size, const DirectX::SimpleMath::Color& colour)
	{
		drawSpriteByPointsC(texture, LONG(pos.x), LONG(pos.y), LONG(pos.x + size.x), LONG(pos.y + size.y), colour);
	}

	void Renderer::drawSpriteByPointsH(ID3D11ShaderResourceView* texture, float x1, float y1, float x2, float y2, const DirectX::SimpleMath::Color& colour)
	{
		auto pos1 = posH2C(x1, y1);
		auto pos2 = posH2C(x2, y2);
		drawSpriteByPointsC(texture, LONG(pos1.x), LONG(pos1.y), LONG(pos2.x), LONG(pos2.y), colour);
	}

	void Renderer::drawSpriteH(ID3D11ShaderResourceView* texture, float x, float y, float width, float height, const DirectX::SimpleMath::Color& colour)
	{
		drawSpriteByPointsH(texture, x, y, x + width, y + height, colour);
	}

	Texture* Renderer::getSpriteByMenuName(const Label& menu_name)
	{
		if (!menu_name.isLiteral())
		{
			switch (menu_name.getLocalisationHash())
			{
			case ATSTRINGHASH("ON"):
				return &textureEnabled;

			case ATSTRINGHASH("DOFF"):
			case ATSTRINGHASH("NOONE"):
				return &textureDisabled;

			case ATSTRINGHASH("PS_F_S"):
			case ATSTRINGHASH("PSC_M"):
			case ATSTRINGHASH("ISSUER"):
				return &textureUser;

			case ATSTRINGHASH("PS_S"):
			case ATSTRINGHASH("PSC_FM"):
			case ATSTRINGHASH("ISSUER_N_ME"):
				return &textureFriends;

			case ATSTRINGHASH("PSC_CFM"):
			case ATSTRINGHASH("EVERYONE"):
				return &textureUsers;
			}
		}
		return nullptr;
	}

	bool Renderer::drawSpriteH(ID3D11ShaderResourceView* texture, const LONG x, const LONG y, const LONG width, const LONG height, const DirectX::SimpleMath::Color& colour, const POINT& cursor_pos)
	{
		const LONG x1 = x;
		const LONG y1 = y;
		const LONG x2 = x + width;
		const LONG y2 = y + height;
		drawSpriteByPointsH(texture, float(x1), float(y1), float(x2), float(y2), colour);
		return cursor_pos.x > x1 && x2 >= cursor_pos.x && cursor_pos.y > y1 && y2 >= cursor_pos.y;
	}

	void Renderer::drawRectByPointsC(const DirectX::SimpleMath::Vector2& pos1, const DirectX::SimpleMath::Vector2& pos2, const DirectX::SimpleMath::Color& colour)
	{
		return drawRectByPointsS(C2S(pos1), C2S(pos2), colour);
	}

	void Renderer::drawRectByPointsH(double x1, double y1, double x2, double y2, const DirectX::SimpleMath::Color& colour)
	{
		return drawRectByPointsC(posH2C(x1, y1), posH2C(x2, y2), colour);
	}

	void Renderer::drawRectC(const DirectX::SimpleMath::Vector2& pos, const DirectX::SimpleMath::Vector2& size, const DirectX::SimpleMath::Color& colour)
	{
		return drawRectByPointsC(pos, pos + size, colour);
	}

	void Renderer::drawRectC(const float x, const float y, const float width, const float height, const DirectX::SimpleMath::Color& colour)
	{
		return drawRectC(DirectX::SimpleMath::Vector2(x, y), DirectX::SimpleMath::Vector2(width, height), colour);
	}

	void Renderer::drawRectH(double x, double y, double width, double height, const DirectX::SimpleMath::Color& colour)
	{
		return drawRectByPointsH(x, y, x + width, y + height, colour);
	}

	bool Renderer::drawRectH(const float x, const float y, float width, float height, const DirectX::SimpleMath::Color& colour, const DirectX::SimpleMath::Vector2& cursorPosH)
	{
		width += x;
		height += y;
		auto pos1 = posH2C(x, y);
		auto pos2 = posH2C(width, height);
		drawRectByPointsC(pos1, pos2, colour);
		return cursorPosH.x > x && width >= cursorPosH.x && cursorPosH.y > y && height >= cursorPosH.y;
	}

	void Renderer::drawBorderC(const DirectX::SimpleMath::Vector2& pos, const DirectX::SimpleMath::Vector2& size, const float border_width, const DirectX::SimpleMath::Color& border_colour, bool rounded)
	{
		if (rounded)
		{
			drawRoundedBorderC(pos.x, pos.y, size.x, size.y, border_width, border_colour);
		}
		else
		{
			drawBorderC(pos.x, pos.y, size.x, size.y, border_width, border_colour);
		}
	}

	void Renderer::drawBorderH(const float x, const float y, const float width, const float height, const float border_width, const DirectX::SimpleMath::Color& border_colour, bool rounded)
	{
		if (rounded)
		{
			drawRoundedBorderH(x, y, width, height, border_width, border_colour);
		}
		else
		{
			drawBorderH(x, y, width, height, border_width, border_colour);
		}
	}

	void Renderer::drawRoundedBorderH(const float x, const float y, const float width, const float height, const float border_width, const DirectX::SimpleMath::Color& border_colour)
	{
		auto pos1S = C2S(posH2C(x, y));
		auto pos2S = C2S(posH2C(x + width, y + height));
		auto borderWidthS = C2S(sizeH2C(border_width, border_width));

		Circle::inst_100.setRadius(borderWidthS.x);
		drawCornerCircleS(pos2S.x, pos2S.y, Circle::inst_100, 0, border_colour);
		drawCornerCircleS(pos1S.x, pos2S.y, Circle::inst_100, 1, border_colour);
		drawCornerCircleS(pos1S.x, pos1S.y, Circle::inst_100, 2, border_colour);
		drawCornerCircleS(pos2S.x, pos1S.y, Circle::inst_100, 3, border_colour);

		drawRectH(x, y - border_width, width, border_width, border_colour);
		drawRectH(x + width, y, border_width, height, border_colour);
		drawRectH(x, y + height, width, border_width, border_colour);
		drawRectH(x - border_width, y, border_width, height, border_colour);
	}

	void Renderer::drawRoundedBorderC(const float x, const float y, const float width, const float height, const float border_width, const DirectX::SimpleMath::Color& border_colour)
	{
		auto pos1S = C2S(x, y);
		auto pos2S = C2S(x + width, y + height);
		auto borderWidthS = C2S(border_width, border_width);

		Circle::inst_100.setRadius(borderWidthS.x);
		drawCornerCircleS(pos2S.x, pos2S.y, Circle::inst_100, 0, border_colour);
		drawCornerCircleS(pos1S.x, pos2S.y, Circle::inst_100, 1, border_colour);
		drawCornerCircleS(pos1S.x, pos1S.y, Circle::inst_100, 2, border_colour);
		drawCornerCircleS(pos2S.x, pos1S.y, Circle::inst_100, 3, border_colour);

		drawRectC(x, y - border_width, width, border_width, border_colour);
		drawRectC(x + width, y, border_width, height, border_colour);
		drawRectC(x, y + height, width, border_width, border_colour);
		drawRectC(x - border_width, y, border_width, height, border_colour);
	}

	void Renderer::drawBorderH(const float x, const float y, const float width, const float height, const float border_width, const DirectX::SimpleMath::Color& border_colour)
	{
		drawRectH(x - border_width, y - border_width, width + border_width, border_width, border_colour);
		drawRectH(x + width, y - border_width, border_width, height + border_width, border_colour);
		drawRectH(x, y + height, width + border_width, border_width, border_colour);
		drawRectH(x - border_width, y, border_width, height + border_width, border_colour);
	}

	void Renderer::drawBorderC(const float x, const float y, const float width, const float height, const float border_width, const DirectX::SimpleMath::Color& border_colour)
	{
		drawRectC(x - border_width, y - border_width, width + border_width, border_width, border_colour);
		drawRectC(x + width, y - border_width, border_width, height + border_width, border_colour);
		drawRectC(x, y + height, width + border_width, border_width, border_colour);
		drawRectC(x - border_width, y, border_width, height + border_width, border_colour);
	}

	void Renderer::drawInnerBorderH(const float x, const float y, const float width, const float height, const float border_width, const DirectX::SimpleMath::Color& border_colour)
	{
		drawRectH(x, y, width - border_width, border_width, border_colour); // top
		drawRectH(x + width - border_width, y, border_width, height - border_width, border_colour); // right
		drawRectH(x + border_width, y + height - border_width, width - border_width, border_width, border_colour); // bottom
		drawRectH(x, y + border_width, border_width, height - border_width, border_colour); // left
	}

	void Renderer::drawHudspaceBorder()
	{
		constexpr LONG border_width = 1;
		/*LONG border_width = g_menu_grid.border_width;
		if (border_width < 1)
		{
			border_width = 1;
		}*/
		drawBorderH(0 + border_width, 0 + border_width, HUD_WIDTH - (border_width * 2), HUD_HEIGHT - (border_width * 2), border_width, minigameColour);
		//drawBorderH(0, 0, HUD_WIDTH, HUD_HEIGHT, border_width, minigameColour);
	}

	void Renderer::drawRectByPointsThisTickS(DirectX::SimpleMath::Vector2&& pos1, DirectX::SimpleMath::Vector2&& pos2, const DirectX::SimpleMath::Color& colour)
	{
		SOUP_ASSERT(ExecCtx::get().isScript());
		extras_buffer.emplace_back(std::make_unique<DrawRectData>(
			std::move(pos1),
			std::move(pos2),
			colour
		));
	}

	void Renderer::drawRectByPointsThisTickC(const DirectX::SimpleMath::Vector2& pos1, const DirectX::SimpleMath::Vector2& pos2, const DirectX::SimpleMath::Color& colour)
	{
		drawRectByPointsThisTickS(
			C2S(pos1),
			C2S(pos2),
			colour
		);
	}

	void Renderer::drawRectByPointsThisTickCP(const float x1, const float y1, const float x2, const float y2, const DirectX::SimpleMath::Color& colour)
	{
		drawRectByPointsThisTickC(
			CP2C(x1, y1),
			CP2C(x2, y2),
			colour
		);
	}

	void Renderer::drawRectByPointsThisTickHP(double x1, double y1, double x2, double y2, const DirectX::SimpleMath::Color& colour)
	{
		HP2H(x1, y1);
		HP2H(x2, y2);
		drawRectByPointsThisTickC(
			posH2C(x1, y1),
			posH2C(x2, y2),
			colour
		);
	}

	void Renderer::drawRectThisTickHP(double x, double y, double width, double height, const DirectX::SimpleMath::Color& colour)
	{
		drawRectByPointsThisTickHP(x, y, x + width, y + height, colour);
	}

	void Renderer::drawRectThisTickCP(const float x, const float y, const float width, const float height, const DirectX::SimpleMath::Color& colour)
	{
		drawRectByPointsThisTickCP(x, y, x + width, y + height, colour);
	}

	const Texture* Renderer::characterToTexture(uint32_t codepoint) const
	{
		switch (codepoint)
		{
		case 0x0000: return &textureUni0000;
		case 0x26A0: return &textureUni26A0;
		case 0x2728: return &textureUni2728;
		case 0x2764: return &textureUni2764;
		case 0x1F4AF: return &textureUni1F4AF;
		case 0x1F60A: return &textureUni1F60A;
		case 0x1F480: return &textureUni1F480;
		case 0x1F525: return &textureUni1F525;
		case 0x1F602: return &textureUni1F602;
		case 0x1F629: return &textureUni1F629;
		case 0x1F633: return &textureUni1F633;

		case UnicodePrivateUse::rs[0]: return &textureRs;
		case UnicodePrivateUse::wanted_star[0]: return &textureWantedStar;
		case UnicodePrivateUse::lock[0]: return &textureLock;
		case UnicodePrivateUse::rs_verified[0]: return &textureRsVerified;
		case UnicodePrivateUse::rs_created[0]: return &textureRsCreated;
		case UnicodePrivateUse::blank_box[0]: return &textureBlankbox;
		case UnicodePrivateUse::newline[0]: return &textureNewline;
		case UnicodePrivateUse::reset[0]: return &textureReset;

		case UnicodePrivateUse::tab_self[0]: return &textureTabSelf;
		case UnicodePrivateUse::tab_vehicle[0]: return &textureTabVehicle;
		case UnicodePrivateUse::tab_online[0]: return &textureTabOnline;
		case UnicodePrivateUse::tab_players[0]: return &textureTabPlayers;
		case UnicodePrivateUse::tab_world[0]: return &textureTabWorld;
		case UnicodePrivateUse::tab_game[0]: return &textureTabGame;
		case UnicodePrivateUse::tab_stand[0]: return &textureTabStand;

		case UnicodePrivateUse::custom_00[0]: return &textureCustom00;
		case UnicodePrivateUse::custom_01[0]: return &textureCustom01;
		case UnicodePrivateUse::custom_02[0]: return &textureCustom02;
		case UnicodePrivateUse::custom_03[0]: return &textureCustom03;
		case UnicodePrivateUse::custom_04[0]: return &textureCustom04;
		case UnicodePrivateUse::custom_05[0]: return &textureCustom05;
		case UnicodePrivateUse::custom_06[0]: return &textureCustom06;
		case UnicodePrivateUse::custom_07[0]: return &textureCustom07;
		case UnicodePrivateUse::custom_08[0]: return &textureCustom08;
		case UnicodePrivateUse::custom_09[0]: return &textureCustom09;
		case UnicodePrivateUse::custom_0A[0]: return &textureCustom0A;
		case UnicodePrivateUse::custom_0B[0]: return &textureCustom0B;
		case UnicodePrivateUse::custom_0C[0]: return &textureCustom0C;
		case UnicodePrivateUse::custom_0D[0]: return &textureCustom0D;
		case UnicodePrivateUse::custom_0E[0]: return &textureCustom0E;
		case UnicodePrivateUse::custom_0F[0]: return &textureCustom0F;
		case UnicodePrivateUse::custom_10[0]: return &textureCustom10;
		case UnicodePrivateUse::custom_11[0]: return &textureCustom11;
		case UnicodePrivateUse::custom_12[0]: return &textureCustom12;
		case UnicodePrivateUse::custom_13[0]: return &textureCustom13;
		case UnicodePrivateUse::custom_14[0]: return &textureCustom14;
		case UnicodePrivateUse::custom_15[0]: return &textureCustom15;
		case UnicodePrivateUse::custom_16[0]: return &textureCustom16;
		case UnicodePrivateUse::custom_17[0]: return &textureCustom17;
		case UnicodePrivateUse::custom_18[0]: return &textureCustom18;
		case UnicodePrivateUse::custom_19[0]: return &textureCustom19;
		case UnicodePrivateUse::custom_1A[0]: return &textureCustom1A;
		case UnicodePrivateUse::custom_1B[0]: return &textureCustom1B;
		case UnicodePrivateUse::custom_1C[0]: return &textureCustom1C;
		case UnicodePrivateUse::custom_1D[0]: return &textureCustom1D;
		case UnicodePrivateUse::custom_1E[0]: return &textureCustom1E;
		case UnicodePrivateUse::custom_1F[0]: return &textureCustom1F;
		}
		if (codepoint >= 0x100000 && codepoint <= 0x10FFFD)
		{
			return g_renderer.getScriptTexture(codepoint - 0x100000);
		}
		return nullptr;
	}

	bool Renderer::isCharacterTextureColourable(uint32_t codepoint) const
	{
		switch (codepoint)
		{
		case 0x0000:
		case UnicodePrivateUse::rs[0]:
		case UnicodePrivateUse::lock[0]:
		case UnicodePrivateUse::rs_verified[0]:
		case UnicodePrivateUse::rs_created[0]:
		case UnicodePrivateUse::blank_box[0]:
		case UnicodePrivateUse::newline[0]:
		case UnicodePrivateUse::reset[0]:
			return true;

		case UnicodePrivateUse::tab_self[0]:
		case UnicodePrivateUse::tab_vehicle[0]:
		case UnicodePrivateUse::tab_online[0]:
		case UnicodePrivateUse::tab_players[0]:
		case UnicodePrivateUse::tab_world[0]:
		case UnicodePrivateUse::tab_game[0]:
		case UnicodePrivateUse::tab_stand[0]:
			return tab_textures_colourable;
		}
		return false;
	}

	void Renderer::findGlyph(const SpriteFont*& outFont, const SpriteFont::Glyph*& outGlyph, const SpriteFont& font_pref, wchar_t character) const
	{
		if (font_pref.ContainsCharacter(character))
		{
			outFont = &font_pref;
			outGlyph = font_pref.FindGlyph(character);
		}
		else if (m_font_yahei->ContainsCharacter(character))
		{
			outFont = &*m_font_yahei;
			outGlyph = m_font_yahei->FindGlyph(character);
		}
		else if (m_font_nanumgothic->ContainsCharacter(character))
		{
			outFont = &*m_font_nanumgothic;
			outGlyph = m_font_nanumgothic->FindGlyph(character);
		}
		else if (m_font_bevietnamprolight->ContainsCharacter(character))
		{
			outFont = &*m_font_bevietnamprolight;
			outGlyph = m_font_bevietnamprolight->FindGlyph(character);
		}
		else
		{
			outFont = &font_pref;
			outGlyph = nullptr;
		}
	}

	void Renderer::prepareTextForRendering(const SpriteFont& font, std::wstring& text)
	{
		const SpriteFont* outFont;
		const SpriteFont::Glyph* outGlyph;
		for (wchar_t& c : text)
		{
			findGlyph(outFont, outGlyph, font, c);
			if (outGlyph != nullptr)
			{
				continue;
			}
			if (c == L'「')
			{
				c = L'[';
			}
			else if (c == L'」')
			{
				c = L']';
			}
			else
			{
				::Sainan::transform_homoglyph(c);
			}
		}
	}

	static constexpr float texture_x_offset = 7.0f;

	void Renderer::drawTextRawC(soup::SharedPtr<SpriteFont> font, const DirectX::SimpleMath::Vector2& base_pos, const wchar_t* text, size_t len, const float scale, const DirectX::SimpleMath::Color& _colour)
	{
		if (font)
		{
			XMVECTOR scale_vec = XMVectorReplicate(scale);
			if (drawTextDebugRects)
			{
				DirectX::SimpleMath::Color invertedColour;
				_colour.Negate(invertedColour);
				drawRectByPointsC(base_pos, base_pos + getTextSize(font, text, len, scale), invertedColour);
			}
			DirectX::SimpleMath::Color colour = propagateAlpha(_colour);
			DirectX::SimpleMath::Vector2 pos{ base_pos };
			wchar_t text_buf[2] = { 0, 0 };
			const float scaled_line_spacing = (font->GetLineSpacing() * scale);
			const SpriteFont* outFont;
			const SpriteFont::Glyph* glyph;
			for (; len-- != 0; ++text)
			{
				const wchar_t character = *text;
				switch (character)
				{
				case '\r':
					break;

				case '\n':
					pos.x = base_pos.x;
					pos.y += scaled_line_spacing;
					break;

				default:
					{
						uint32_t codepoint = character;
						SOUP_IF_UNLIKELY (UTF16_IS_HIGH_SURROGATE(codepoint))
						{
							++text;
							--len;
							SOUP_IF_LIKELY (UTF16_IS_LOW_SURROGATE(*text))
							{
								codepoint = soup::unicode::utf16_to_utf32(codepoint, *text);
							}
							else
							{
								--text;
								++len;
							}
						}
						if (codepoint == 0xFE0E || codepoint == 0xFE0F)
						{
							// Variation selector, ignore.
							continue;
						}
						auto* texture = characterToTexture(codepoint);
						if (texture != nullptr)
						{
							pos.x += texture_x_offset * scale;
							const float render_height = texture->negotiateRenderHeight(scaled_line_spacing);
							const float render_width = texture->getRenderWidth(render_height);
							DirectX::SimpleMath::Color sprite_colour = colour;
							if (!isCharacterTextureColourable(codepoint))
							{
								sprite_colour.R(1.0f);
								sprite_colour.G(1.0f);
								sprite_colour.B(1.0f);
								propagateAlpha(sprite_colour);
							}
							drawSpriteC(*texture, LONG(pos.x), LONG(pos.y + ((scaled_line_spacing - render_height) / 2)), LONG(render_width), LONG(render_height), sprite_colour);
							pos.x += render_width;
						}
						else
						{
							findGlyph(outFont, glyph, *font, character);
							SOUP_IF_UNLIKELY (glyph == nullptr)
							{
								glyph = font->FindGlyph(L'?');
							}
							text_buf[0] = character;
							outFont->DrawString(m_pSpriteBatch.get(), text_buf, pos, colour, 0.0f, {}, scale_vec);
							pos.x += glyph->XOffset * scale;
							if (pos.x < base_pos.x)
							{
								pos.x = base_pos.x;
							}
							pos.x += (float(glyph->Subrect.right) - float(glyph->Subrect.left) + glyph->XAdvance) * scale;
						}
					}
					break;
				}
			}
		}
	}

	void Renderer::drawTextRawC(soup::SharedPtr<SpriteFont> font, const float x, const float y, const wchar_t* text, size_t len, const float scale, const DirectX::SimpleMath::Color& colour)
	{
		drawTextRawC(std::move(font), DirectX::SimpleMath::Vector2(x, y), text, len, scale, colour);
	}

	void Renderer::drawTextC(soup::SharedPtr<SpriteFont> font, const float x, const float y, std::wstring&& text, const float scale, const DirectX::SimpleMath::Color& colour)
	{
		prepareTextForRendering(*font, text);
		drawTextRawC(std::move(font), x, y, text.data(), text.size(), scale, colour);
	}

	void Renderer::drawTextH(const float x, const float y, std::wstring&& text, const float scale, const DirectX::SimpleMath::Color& colour)
	{
		prepareTextForRendering(*m_font_user, text);
		drawTextRawC(m_font_user, posH2C(x, y), text.data(), text.size(), scale * resolution_text_scale, colour);
	}

	struct TextAndSize
	{
		float width;
		float height;
		std::wstring text;
	};

	void Renderer::drawTextC(float x, float y, std::wstring&& text, const Alignment alignment, const float scale, const DirectX::SimpleMath::Color& colour, soup::SharedPtr<SpriteFont> font, const bool force_in_bounds)
	{
		const auto size = getTextSize(font, text, scale);
		switch (alignment & 0b1100)
		{
		case 0b0100: // Centre
			y -= (size.y * 0.5f);
			break;

		case 0b1000: // Bottom
			y -= size.y;
			break;
		}
		if (force_in_bounds)
		{
			if (y < 0.0f)
			{
				y = 0.0f;
			}
			else if (y + size.y > client_size.y)
			{
				y = client_size.y - size.y;
			}
		}
		std::vector<TextAndSize> lines;
		size_t nl_pos;
		while ((nl_pos = text.find(L'\n')) != std::string::npos)
		{
			auto line = text.substr(0, nl_pos);
			const auto line_size = getTextSize(font, line, scale);
			lines.emplace_back(TextAndSize{ line_size.x, line_size.y, std::move(line) });
			text.erase(0, nl_pos + 1);
		}
		if (!text.empty())
		{
			const auto line_size = getTextSize(font, text, scale);
			lines.emplace_back(TextAndSize{ line_size.x, line_size.y, std::move(text) });
		}
		const uint8_t x_alignment = alignment & 0b11;
		if (force_in_bounds)
		{
			switch (x_alignment)
			{
			default:
			case 0b00: // Left
				if (x + size.x > client_size.x)
				{
					x = client_size.x - size.x;
				}
				break;

			case 0b01: // Centre
				if (x - (size.x * 0.5f) < 0.0f)
				{
					x = (size.x * 0.5f);
				}
				else if (x + (size.x * 0.5f) > client_size.x)
				{
					x = client_size.x - (size.x * 0.5f);
				}
				break;

			case 0b10: // Right
				if (x - size.x < 0.0f)
				{
					x = size.x;
				}
				break;
			}
		}
		for (auto& entry : lines)
		{
			switch (x_alignment)
			{
			default: // Left
				drawTextC(font, x, y, std::move(entry.text), scale, colour);
				break;

			case 0b01: // Centre
				drawTextC(font, x - (entry.width * 0.5f), y, std::move(entry.text), scale, colour);
				break;

			case 0b10: // Right
				drawTextC(font, x - entry.width, y, std::move(entry.text), scale, colour);
				break;
			}
			y += entry.height;
		}
	}

	void Renderer::drawTextH(float x, float y, std::wstring&& text, const Alignment alignment, const float scale, const DirectX::SimpleMath::Color& colour, const bool force_in_bounds)
	{
		const auto size = getTextSize(text, scale);
		switch (alignment & 0b1100)
		{
		case 0b0100: // Centre
			y -= (size.y * 0.5f);
			break;

		case 0b1000: // Bottom
			y -= size.y;
			break;
		}
		if (force_in_bounds)
		{
			if (y < 0.0f)
			{
				y = 0.0f;
			}
			else if (y + size.y > float(HUD_HEIGHT))
			{
				y = float(HUD_HEIGHT) - size.y;
			}
		}
		std::vector<TextAndSize> lines;
		size_t nl_pos;
		while ((nl_pos = text.find(L'\n')) != std::string::npos)
		{
			auto line = text.substr(0, nl_pos);
			const auto line_size = getTextSize(line, scale);
			lines.emplace_back(TextAndSize{ line_size.x, line_size.y, std::move(line) });
			text.erase(0, nl_pos + 1);
		}
		if (!text.empty())
		{
			const auto line_size = getTextSize(text, scale);
			lines.emplace_back(TextAndSize{ line_size.x, line_size.y, std::move(text) });
		}
		const uint8_t x_alignment = alignment & 0b11;
		if (force_in_bounds)
		{
			switch (x_alignment)
			{
			default:
			case 0b00: // Left
				if (x + size.x > float(HUD_WIDTH))
				{
					x = float(HUD_WIDTH) - size.x;
				}
				break;

			case 0b01: // Centre
				if (x - (size.x * 0.5f) < 0.0f)
				{
					x = (size.x * 0.5f);
				}
				else if (x + (size.x * 0.5f) > float(HUD_WIDTH))
				{
					x = float(HUD_WIDTH) - (size.x * 0.5f);
				}
				break;

			case 0b10: // Right
				if (x - size.x < 0.0f)
				{
					x = size.x;
				}
				break;
			}
		}
		for (auto& entry : lines)
		{
			switch (x_alignment)
			{
			default: // Left
				drawTextH(x, y, std::move(entry.text), scale, colour);
				break;

			case 0b01: // Centre
				drawTextH(x - (entry.width * 0.5f), y, std::move(entry.text), scale, colour);
				break;

			case 0b10: // Right
				drawTextH(x - entry.width, y, std::move(entry.text), scale, colour);
				break;
			}
			y += entry.height;
		}
	}

	void Renderer::drawTextH(float x, float y, const wchar_t* text, const DirectX::SimpleMath::Color& colour, const TextSettings& settings)
	{
		auto v = posH2C(x + settings.x_offset, y + settings.y_offset);
		drawTextC(m_font_user, v.x, v.y, text, settings.scale * resolution_text_scale, colour);
	}

	void Renderer::drawTextHNoOffset(float x, float y, const wchar_t* text, const DirectX::SimpleMath::Color& colour, const TextSettings& settings)
	{
		auto v = sizeH2C(x + settings.x_offset, y + settings.y_offset);
		drawTextC(m_font_user, v.x, v.y, text, settings.scale * resolution_text_scale, colour);
	}

	void Renderer::drawTextH(float x, float y, const std::wstring& text, const DirectX::SimpleMath::Color& colour, const TextSettings& settings)
	{
		drawTextH(x, y, text.c_str(), colour, settings);
	}

	void Renderer::drawTextHNoOffset(float x, float y, const std::wstring& text, const DirectX::SimpleMath::Color& colour, const TextSettings& settings)
	{
		drawTextHNoOffset(x, y, text.c_str(), colour, settings);
	}

	void Renderer::drawTextH(float x, float y, std::wstring&& text, const DirectX::SimpleMath::Color& colour, const TextSettings& settings, const Alignment alignment, const bool force_in_bounds)
	{
		auto v = posH2C(x + settings.x_offset, y + settings.y_offset);
		drawTextC(v.x, v.y, std::move(text), alignment, settings.scale * resolution_text_scale, colour, m_font_user, force_in_bounds);
	}

	void Renderer::drawTextHNoOffset(float x, float y, std::wstring&& text, const DirectX::SimpleMath::Color& colour, const TextSettings& settings, const Alignment alignment, const bool force_in_bounds)
	{
		auto v = sizeH2C(x + settings.x_offset, y + settings.y_offset);
		drawTextC(v.x, v.y, std::move(text), alignment, settings.scale * resolution_text_scale, colour, m_font_user, force_in_bounds);
	}

	void Renderer::drawTextSmallH(float x, float y, const wchar_t* text, const DirectX::SimpleMath::Color& colour)
	{
		drawTextH(x, y, text, colour, small_text);
	}

	void Renderer::drawTextSmallH(float x, float y, const std::wstring& text, const DirectX::SimpleMath::Color& colour)
	{
		drawTextSmallH(x, y, text.c_str(), colour);
	}

	void Renderer::drawTextSmallH(float x, float y, std::wstring&& text, const DirectX::SimpleMath::Color& colour, const Alignment alignment)
	{
		drawTextH(x + small_text.x_offset, y + small_text.y_offset, std::move(text), alignment, small_text.scale * resolution_text_scale, colour);
	}

	DirectX::SimpleMath::Vector2 Renderer::getTextSize(const std::wstring& text, const float scale) const
	{
		return getTextSize(m_font_user, text, scale);
	}

	DirectX::SimpleMath::Vector2 Renderer::getTextSize(soup::SharedPtr<SpriteFont> font, const std::wstring& text, const float scale) const
	{
		return getTextSize(font, text.data(), text.size(), scale);
	}

	DirectX::SimpleMath::Vector2 Renderer::getTextSize(soup::SharedPtr<SpriteFont> font, const wchar_t* text, size_t len, const float scale) const
	{
		DirectX::SimpleMath::Vector2 size{};

		if (font)
		{
			const float scaled_line_spacing = font->GetLineSpacing() * scale;
			float x = 0.0f;
			size.y = scaled_line_spacing;
			const SpriteFont* outFont;
			const SpriteFont::Glyph* glyph;
			for (; len-- != 0; ++text)
			{
				const wchar_t character = *text;
				switch (character)
				{
				case '\r':
					break;

				case '\n':
					if (x > size.x)
					{
						size.x = x;
					}
					x = 0.0f;
					size.y += scaled_line_spacing;
					break;

				default:
					{
						uint32_t codepoint = character;
						SOUP_IF_UNLIKELY (UTF16_IS_HIGH_SURROGATE(codepoint))
						{
							++text;
							--len;
							SOUP_IF_LIKELY (UTF16_IS_LOW_SURROGATE(*text))
							{
								codepoint = soup::unicode::utf16_to_utf32(codepoint, *text);
							}
							else
							{
								--text;
								++len;
							}
						}
						if (codepoint == 0xFE0E || codepoint == 0xFE0F)
						{
							// Variation selector, ignore.
							continue;
						}
						auto* texture = characterToTexture(codepoint);
						if (texture != nullptr)
						{
							x += texture_x_offset * scale;
							x += texture->getRenderWidth(texture->negotiateRenderHeight(scaled_line_spacing));
						}
						else
						{
							findGlyph(outFont, glyph, *font, character);
							SOUP_IF_UNLIKELY (glyph == nullptr)
							{
								glyph = font->FindGlyph(L'?');
							}
							x += (glyph->XOffset * scale);
							if (x < 0.0f)
							{
								x = 0.0f;
							}
							x += (float(glyph->Subrect.right) - float(glyph->Subrect.left) + glyph->XAdvance) * scale;
						}
					}
					break;
				}
			}
			if (x > size.x)
			{
				size.x = x;
			}
		}

		return size;
	}

	float Renderer::getTextWidth(const std::wstring& text, const float scale) const
	{
		return getTextSize(text, scale).x;
	}

	float Renderer::getTextHeight(const std::wstring& text, const float scale) const
	{
		if (m_font_user)
		{
			const float scaled_line_spacing = m_font_user->GetLineSpacing() * scale;
			if (text.size() == 1
				|| (text.size() == 2 && UTF16_IS_HIGH_SURROGATE(text.at(0)))
				)
			{
				uint32_t codepoint = text.at(0);
				SOUP_IF_UNLIKELY (UTF16_IS_HIGH_SURROGATE(codepoint))
				{
					SOUP_IF_LIKELY (text.size() > 1 && UTF16_IS_LOW_SURROGATE(text.at(1)))
					{
						codepoint = soup::unicode::utf16_to_utf32(codepoint, text.at(1));
					}
				}
				auto* texture = characterToTexture(codepoint);
				if (texture != nullptr)
				{
					return texture->negotiateRenderHeight(scaled_line_spacing);
				}
			}
			float height = scaled_line_spacing;
			for (const wchar_t& c : text)
			{
				if (c == L'\n')
				{
					height += scaled_line_spacing;
				}
			}
			return height;
		}
		return 0.0f;
	}

	bool Renderer::trimTextH(std::wstring& text, const float scale, float width) const
	{
		if (getTextWidth(text, scale) <= width)
		{
			return false;
		}
		size_t len = text.length();
		if (len <= 1)
		{
			return false;
		}
		text.append(L".");
		len += 1;
		size_t offset = 2;
		auto text_width = getTextWidth(text, scale);
		while (text_width > width)
		{
			if (len < 5)
			{
				break;
			}
			if (UTF16_IS_LOW_SURROGATE(text.at(len - offset)))
			{
				text_width -= getTextWidth(text.substr((len - offset) - 1, 2), scale);
				text.erase((len - offset) - 1, 2);
				len -= 2;
			}
			else
			{
				text_width -= getTextWidth(text.substr(len - offset, 1), scale);
				text.erase(len - offset, 1);
				len -= 1;
			}
			if (text.at(len - offset) == L' ')
			{
				text_width -= getTextWidth(text.substr(len - offset, 1), scale);
				text.erase(len - offset, 1);
				if (offset == 2)
				{
					text_width += getTextWidth(L"..");
					text.append(L"..");
					offset = 4;
					len++; // len = len - 1 + 2;
				}
				else
				{
					len--;
				}
			}
		}
		return true;
	}

	std::wstring Renderer::trimTextH(const std::string& text, const float scale, const float width) const
	{
		std::wstring wstr = StringUtils::utf8_to_utf16(text);
		trimTextH(wstr, scale, width);
		return wstr;
	}

	std::wstring Renderer::wrapTextH(const std::wstring& text, const float scale, float width) const
	{
		// https://stackoverflow.com/questions/15986473/how-do-i-implement-word-wrap
		const auto space_width = getTextWidth(L"a a", scale) - getTextWidth(L"aa", scale);
		std::wstring wstr;
		size_t offset = 0;
		size_t nl_pos;
		while ((nl_pos = text.find(L'\n', offset)) != std::string::npos)
		{
			auto line = text.substr(offset, nl_pos - offset);
			wrapLineH(wstr, line, scale, width, space_width);
			offset = nl_pos + 1;
			wstr.push_back(L'\n');
		}
		if (offset != text.size())
		{
			auto line = text.substr(offset);
			wrapLineH(wstr, line, scale, width, space_width);
		}
		return wstr;
	}

	void Renderer::wrapLineH(std::wstring& wstr, std::wstring& line, const float scale, const float width, const float space_width) const
	{
		float line_width = 0.0;
		size_t space_pos;
		while ((space_pos = line.find(L' ')) != std::string::npos)
		{
			wrapWordH(wstr, line.substr(0, space_pos), scale, width, space_width, line_width);
			line.erase(0, space_pos + 1);
		}
		if (!line.empty())
		{
			wrapWordH(wstr, line, scale, width, space_width, line_width);
		}
	}

	void Renderer::wrapWordH(std::wstring& wstr, const std::wstring& word, const float scale, const float width, const float space_width, float& line_width) const
	{
		const auto word_width = getTextWidth(word, scale);
		if (line_width + word_width <= width)
		{
			wstr.append(word).push_back(L' ');
			line_width += word_width + space_width;
		}
		else
		{
			if (!wstr.empty())
			{
				wstr.push_back(L'\n');
				line_width = 0.0;
			}
			if (word_width > width)
			{
				size_t i = 1;
				std::wstring trimmed_word;
				std::wstring next_char = word.substr(0, 1);
				if (UTF16_IS_HIGH_SURROGATE(next_char.back()))
				{
					next_char.push_back(word.at(i++));
				}
				float next_char_width = getTextWidth(next_char, scale);
				float trimmed_word_plus_next_char_width = next_char_width;
				while (trimmed_word_plus_next_char_width < width)
				{
					trimmed_word.append(next_char);
					line_width += next_char_width;
					next_char = word.substr(i++, 1);
					if (UTF16_IS_HIGH_SURROGATE(next_char.back()))
					{
						next_char.push_back(word.at(i++));
					}
					next_char_width = getTextWidth(next_char, scale);
					trimmed_word_plus_next_char_width += next_char_width;
				}
				wstr.append(trimmed_word);
				i -= next_char.size();
				if (i != 0)
				{
					wrapWordH(wstr, word.substr(i), scale, width, space_width, line_width);
				}
			}
			else
			{
				wstr.append(word).push_back(L' ');
				line_width = word_width + space_width;
			}
		}
	}

	void Renderer::drawLineS(const float x1, const float y1, const DirectX::SimpleMath::Color& colour1, const float x2, const float y2, const DirectX::SimpleMath::Color& colour2)
	{
		m_batch->DrawLine(S2VPC(x1, y1, colour1), S2VPC(x2, y2, colour2));
	}

	void Renderer::drawLineS(const float x1, const float y1, const float x2, const float y2, const DirectX::SimpleMath::Color& colour)
	{
		drawLineS(x1, y1, colour, x2, y2, colour);
	}

	void Renderer::drawLineCP(const float x1, const float y1, const float x2, const float y2, const DirectX::SimpleMath::Color& colour1, const DirectX::SimpleMath::Color& colour2)
	{
		m_batch->DrawLine(CP2VPC(x1, y1, colour1), CP2VPC(x2, y2, colour2));
	}

	void Renderer::drawLineCP(const float x1, const float y1, const float x2, const float y2, const DirectX::SimpleMath::Color& colour)
	{
		drawLineCP(x1, y1, x2, y2, colour, colour);
	}

	void Renderer::drawLineCP(const rage::Vector2& pos1, const rage::Vector2& pos2, const DirectX::SimpleMath::Color& colour)
	{
		if (pos1.isInBounds() && pos2.isInBounds())
		{
			drawLineCP(pos1.x, pos1.y, pos2.x, pos2.y, colour);
		}
	}

	void Renderer::drawLineH(const float x1, const float y1, const float x2, const float y2, const DirectX::SimpleMath::Color& colour)
	{
		drawLineCP(C2CP<rage::Vector2>(posH2C(x1, y1)), C2CP<rage::Vector2>(posH2C(x2, y2)), colour);
	}

	void Renderer::drawTextThisTickC(float x, float y, std::wstring&& text, Alignment alignment, float scale, const DirectX::SimpleMath::Color& colour, const soup::SharedPtr<SpriteFont>& font, bool force_in_bounds, const v3& draw_origin_3d, uint8_t drop_shadow)
	{
		SOUP_ASSERT(ExecCtx::get().isScript());
		SOUP_IF_UNLIKELY (!font)
		{
			return;
		}
		extras_buffer.emplace_back(std::make_unique<DrawTextData>(
			x,
			y,
			std::move(text),
			alignment,
			scale,
			colour,
			font,
			force_in_bounds,
			draw_origin_3d,
			drop_shadow
		));
	}

	void Renderer::drawTextThisTickCP(float x, float y, std::wstring&& text, Alignment alignment, float scale, const DirectX::SimpleMath::Color& colour, const soup::SharedPtr<SpriteFont>& font, bool force_in_bounds, const v3& draw_origin_3d, uint8_t drop_shadow)
	{
		drawTextThisTickC(x * client_size.x, y * client_size.y, std::move(text), alignment, scale, colour, font, force_in_bounds, draw_origin_3d, drop_shadow);
	}

	void Renderer::drawTextThisTickH(float x, float y, std::wstring&& text, Alignment alignment, float scale, const DirectX::SimpleMath::Color& colour, const soup::SharedPtr<SpriteFont>& font, bool force_in_bounds, const v3& draw_origin_3d, uint8_t drop_shadow)
	{
		auto v = posH2C(x, y);
		return drawTextThisTickC(v.x, v.y, std::move(text), alignment, scale * resolution_text_scale, colour, font, force_in_bounds, draw_origin_3d, drop_shadow);
	}

	void Renderer::drawTextThisTickHP(float x, float y, std::wstring&& text, Alignment alignment, float scale, const DirectX::SimpleMath::Color& colour, const soup::SharedPtr<SpriteFont>& font, bool force_in_bounds, const v3& draw_origin_3d, uint8_t drop_shadow)
	{
		return drawTextThisTickH(x * HUD_WIDTH, y * HUD_HEIGHT, std::move(text), alignment, scale, colour, font, force_in_bounds, draw_origin_3d, drop_shadow);
	}

	void Renderer::drawCentredTextThisTick(const std::string& text)
	{
		drawCentredTextThisTick(StringUtils::utf8_to_utf16(text));
	}

	void Renderer::drawCentredTextThisTick(std::wstring text)
	{
		SOUP_ASSERT(ExecCtx::get().isScript());
		extras_buffer.emplace_back(std::make_unique<DrawCentredTextData>(std::move(text)));
	}

	void Renderer::drawUnscaled3dTextThisTickH(const v3& pos, std::wstring&& text, float scale, DirectX::SimpleMath::Color& colour)
	{
		const rage::Vector2 screen_pos = pos.getScreenPos();
		if (screen_pos.isInBounds())
		{
			drawTextThisTickCP(screen_pos.x, screen_pos.y, std::move(text), ALIGN_CENTRE, scale * resolution_text_scale, colour, m_font_user, false);
		}
	}

	void Renderer::drawUnscaled3dTextThisTickH(const v3& pos, const std::wstring& text, float scale, DirectX::SimpleMath::Color& colour)
	{
		drawUnscaled3dTextThisTickH(pos, std::move(std::wstring(text)), scale, colour);
	}

	void Renderer::draw3dTextThisTickH(const v3& pos, std::wstring&& text, float scale)
	{
		draw3dTextThisTickH(pos, std::move(text), scale, hudColour);
	}

	void Renderer::draw3dTextThisTickH(const v3& pos, const std::wstring& text, const float scale)
	{
		draw3dTextThisTickH(pos, std::move(std::wstring(text)), scale);
	}

	void Renderer::draw3dTextThisTickH(const v3& pos, std::wstring&& text, float scale, const DirectX::SimpleMath::Color& colour)
	{
		const rage::Vector2 screen_pos = pos.getScreenPos();
		if (screen_pos.isInBounds())
		{
			scale *= (1.0f / pos.distance(CAM::GET_FINAL_RENDERED_CAM_COORD())) * 2.0f;
			scale *= (1.0f / CAM::GET_FINAL_RENDERED_CAM_FOV()) * 100.0f;
			drawTextThisTickCP(screen_pos.x, screen_pos.y, std::move(text), ALIGN_CENTRE, scale * resolution_text_scale, colour, m_font_user, false);
		}
	}

	void Renderer::drawLineThisTickCP(const float x1, const float y1, const float x2, const float y2, const DirectX::SimpleMath::Color& colour1, const DirectX::SimpleMath::Color& colour2)
	{
		SOUP_ASSERT(ExecCtx::get().isScript());
		extras_buffer.emplace_back(std::make_unique<DrawLineData>(x1, y1, x2, y2, colour1, colour2));
	}

	void Renderer::drawLineThisTickCP(const float x1, const float y1, const float x2, const float y2, const DirectX::SimpleMath::Color& colour)
	{
		return drawLineThisTickCP(x1, y1, x2, y2, colour, colour);
	}

	void Renderer::drawLineThisTickCP(const rage::Vector2& pos1, const rage::Vector2& pos2, const DirectX::SimpleMath::Color& colour)
	{
		if (pos1.isInBounds() && pos2.isInBounds())
		{
			drawLineThisTickCP(pos1.x, pos1.y, pos2.x, pos2.y, colour);
		}
	}

	void Renderer::drawLineThisTickHP(const float x1, const float y1, const float x2, const float y2, const DirectX::SimpleMath::Color& colour1, const DirectX::SimpleMath::Color& colour2)
	{
		auto pos1CP = C2CP(posH2C(HP2H(x1, y1)));
		auto pos2CP = C2CP(posH2C(HP2H(x2, y2)));
		drawLineThisTickCP(pos1CP.x, pos1CP.y, pos2CP.x, pos2CP.y, colour1, colour2);
	}

	void Renderer::drawBgBlurThisTickC(soup::WeakRef<EphemeralBackgroundBlur>&& wr, float x, float y, float width, float height, uint8_t passes)
	{
		SOUP_ASSERT(ExecCtx::get().isScript());
		extras_buffer.emplace_back(std::make_unique<DrawBgBlurData>(std::move(wr), x, y, width, height, passes));
	}

#if DRAW_HTML
	void Renderer::drawHtmlThisTickH(int x, int y, unsigned int width, unsigned int height, std::string&& html)
	{
		SOUP_ASSERT(ExecCtx::get().isScript());
		extras_buffer.emplace_back(std::make_unique<DrawHtmlData>(x, y, width, height, std::move(html)));
	}
#endif

	void Renderer::drawCentredTextThisFrame(const std::wstring& text)
	{
		centred_text_this_frame.append(text);
		centred_text_this_frame.push_back(L'\n');
	}

	void Renderer::draw()
	{
		SOUP_IF_UNLIKELY (drawing)
		{
			Util::toast(LIT_OBF("Still drawing but asked to draw again?!"), TOAST_ALL);
			return;
		}
		drawing = true;

		++Metrics::draws_this_second;

		__try
		{
			AntiAntiCheat::onPresent();
		}
		__EXCEPTIONAL()
		{
		}

		if (!creating_textures)
		{
			EXCEPTIONAL_LOCK(create_sprite_mtx)
			if (!CreateTextureArray.empty())
			{
				creating_textures = true;
				Exceptional::createManagedThread([]
				{
					EXCEPTIONAL_LOCK(g_renderer.create_sprite_mtx)
					for (auto it = g_renderer.CreateTextureArray.begin(); it != g_renderer.CreateTextureArray.end(); ++it)
					{
						Texture t;
						ID3D11Resource* rsrc = nullptr;
						if (SUCCEEDED(CreateWICTextureFromFile(g_renderer.m_pDevice, it->second.c_str(), &rsrc, t.srv.ReleaseAndGetAddressOf(), 0)))
						{
							t.measure(rsrc);
							EXCEPTIONAL_LOCK(g_renderer.script_textures_mtx)
							g_renderer.script_textures.emplace(it->first, std::move(t));
							EXCEPTIONAL_UNLOCK(g_renderer.script_textures_mtx)
							g_renderer.ReloadArray.emplace_back(ReloadTex(it->first, it->second));
						}
						else
						{
							Util::toast(std::move(std::string("Failed to load texture ").append(fmt::to_string(it->first)).append(" from ").append  (StringUtils::utf16_to_utf8(it->second))), TOAST_ALL);
						}
					}
					g_renderer.CreateTextureArray.clear();
					EXCEPTIONAL_UNLOCK(g_renderer.create_sprite_mtx)
					g_renderer.creating_textures = false;
				});
			}
			EXCEPTIONAL_UNLOCK(create_sprite_mtx)
		}

		m_stateSaver->saveCurrentState(m_pDevice, m_pContext);

		for (const auto& func : asi_present_callbacks)
		{
			func(pointers::swapchain);
			m_stateSaver->restoreSavedState();
		}

		if ((gameplayState == MENUING || gameplayState == PLAYING) && lastTickTime < get_current_time_millis() - 100 && g_gui.root_state > GUI_MINIMAL)
		{
			gameplayState = SUSPENDED;
		}
		if (g_gui.init_time == 0)
		{
			if (!g_gui.user_understands_navigation)
			{
				enterDrawContext();
				drawTextC(client_size.x * 0.5f, client_size.y * 0.5f, LANG_GET_W("TUT_LD"), ALIGN_CENTRE, info_scale * resolution_text_scale, hudColour, m_font_user);
				leaveDrawContext();
			}
		}
		else
		{
			if (g_gui.root_state != GUI_NONE)
			{
				__try
				{
					g_gui.evaluateKeyboardInput(TC_RENDERER);
				}
				__EXCEPTIONAL()
				{
				}
			}
			if (doesGameplayStateAllowRendering() /*&& (!present_callbacks.empty() || !extra_text.empty() || g_gui.m_opened)*/)
			{
				enterSpriteDrawContextNonPremultiplied();
				EXCEPTIONAL_LOCK(timed_sprites_mtx)
				for (auto i = timed_sprites.begin(); i != timed_sprites.end(); )
				{
					i->draw_data.draw();
					if (IS_DEADLINE_REACHED(i->deadline))
					{
						i = timed_sprites.erase(i);
					}
					else
					{
						++i;
					}
				}
				EXCEPTIONAL_UNLOCK(timed_sprites_mtx)
				leaveSpriteDrawContextNonPremultiplied();
				enterDrawContext();
				if (doesGameplayStateAllowScriptExecution())
				{
					EXCEPTIONAL_LOCK(extras_mtx)
					for (const auto& extra : extras)
					{
						switch (extra->type)
						{
						case DrawData::TEXT:
						{
							auto text = static_cast<const DrawTextData*>(extra.get());
							float xC = text->client_x;
							float yC = text->client_y;
							SOUP_IF_UNLIKELY (!text->draw_origin_3d.isNull())
							{
								auto drawOriginCP = text->draw_origin_3d.getScreenPos();
								auto drawOriginC = CP2C(drawOriginCP.x, drawOriginCP.y);
								xC += drawOriginC.x;
								yC += drawOriginC.y;
							}
							for (auto i = text->drop_shadow; i; --i)
							{
								const float f = 0.5f * (1.0f - (((float)i + 1.0f) / ((float)text->drop_shadow + 1.0f)));
								drawTextC(xC + i, yC + i, std::wstring(text->text), text->alignment, text->client_scale, DirectX::SimpleMath::Color(text->colour.R() * f, text->colour.G() * f, text->colour.B() * f), text->font, false);
							}
							drawTextC(xC, yC, std::wstring(text->text), text->alignment, text->client_scale, text->colour, text->font, text->force_in_bounds);
						}
						break;

						case DrawData::CENTRED_TEXT:
							drawCentredTextThisFrame(static_cast<const DrawCentredTextData*>(extra.get())->text);
							break;

						case DrawData::LINE:
						{
							auto line = static_cast<const DrawLineData*>(extra.get());
							drawLineCP(line->x1, line->y1, line->x2, line->y2, line->colour1, line->colour2);
						}
						break;

						case DrawData::RECT:
						{
							auto rect = static_cast<const DrawRectData*>(extra.get());
							drawRectByPointsS(rect->pos1S, rect->pos2S, rect->colour);
						}
						break;

						case DrawData::TRIANGLE:
						{
							auto triangle = static_cast<const DrawTriangleData*>(extra.get());
							m_batch->DrawTriangle(
								S2VPC(triangle->pos1S.x, triangle->pos1S.y, triangle->colour),
								S2VPC(triangle->pos2S.x, triangle->pos2S.y, triangle->colour),
								S2VPC(triangle->pos3S.x, triangle->pos3S.y, triangle->colour)
							);
						}
						break;

						case DrawData::SPRITE:
							leaveDrawContext();
							enterSpriteDrawContextNonPremultiplied();
							__try
							{
								static_cast<const DrawSpriteData*>(extra.get())->draw();
							}
							__EXCEPTIONAL()
							{
							}
							leaveSpriteDrawContextNonPremultiplied();
							enterDrawContext();
							break;

						case DrawData::SPRITE_BY_POINTS:
							leaveDrawContext();
							enterSpriteDrawContextNonPremultiplied();
							__try
							{
								auto data = static_cast<const DrawSpriteByPointsData*>(extra.get());
								if (auto tex = getScriptTexture(data->tex_id))
								{
									drawSpriteByPointsH(*tex, data->x1, data->y1, data->x2, data->y2, data->colour);
								}
							}
							__EXCEPTIONAL()
							{
							}
							leaveSpriteDrawContextNonPremultiplied();
							enterDrawContext();
							break;

						case DrawData::CIRCLE:
							{
								auto data = static_cast<const DrawCircleData*>(extra.get());
								Circle::inst_100.setRadius(data->radius);
								if (data->part < 0 || data->part >= 4)
								{
									drawCircleS(data->x, data->y, Circle::inst_100, data->colour);
								}
								else
								{
									drawCornerCircleS(data->x, data->y, Circle::inst_100, data->part, data->colour);
								}
							}
							break;

						case DrawData::BGBLUR:
							{
								auto data = static_cast<const DrawBgBlurData*>(extra.get());
								if (auto bgblur = data->wr.getPointer())
								{
									bgblur->drawC(data->x, data->y, data->width, data->height, data->passes);
								}
							}
							break;

#if DRAW_HTML
						case DrawData::HTML:
							{
								auto data = static_cast<const DrawHtmlData*>(extra.get());
								RenderTargetWithOffsetH rt(data->x, data->y);
								auto doc = soup::lyoDocument::fromMarkup(data->html);
								doc->flatten(data->width, data->height).draw(rt);
							}
							break;
#endif
						}
					}
					EXCEPTIONAL_UNLOCK(extras_mtx)
				}
				__try
				{
					if (!g_tb_screenshot_mode.isEnabled())
					{
						g_notify_grid.draw();
					}
				}
				__EXCEPTIONAL()
				{
				}
				__try
				{
					Pong::onPresent();
					rePresentEvent::trigger();
					SOUP_IF_UNLIKELY (g_renderer.throttling_rendering)
					{
						drawCentredTextThisFrame(LANG_GET_W("PTX_DRAW_T"));
						g_renderer.throttling_rendering = false;
					}
					if (g_gui.opened)
					{
						SOUP_IF_UNLIKELY (is_session_transition_active(false))
						{
							if (!g_gui.status_text.empty())
							{
								drawCentredTextThisFrame(g_gui.status_text);
							}
						}
						drawCentredText();
						g_menu_grid.draw();
						SOUP_IF_UNLIKELY (Tutorial::state != TUT_DONE)
						{
							g_tutorial_grid.draw();
						}
					}
					else
					{
						SOUP_IF_UNLIKELY (is_session_transition_active(false))
						{
							if (!g_gui.status_text.empty())
							{
								drawCentredTextThisFrame(g_gui.status_text);
							}
							if (!g_gui.status_message.empty() && g_gui.show_status_message)
							{
								drawCentredTextThisFrame(g_gui.status_message);
							}
						}
						drawCentredText();
						SOUP_IF_UNLIKELY (Tutorial::state == TUT_OPEN)
						{
							g_tutorial_grid.draw();
						}
					}
					SOUP_IF_UNLIKELY (g_commandbox.active)
					{
						g_commandbox_grid.draw();
					}
				}
				__EXCEPTIONAL()
				{
				}
				// Wrap this in a try-except so in the case it does throw, we still restore the state.
				__try
				{
					leaveDrawContext();
				}
				__EXCEPTIONAL()
				{
				}
			}
		}
		m_stateSaver->restoreSavedState();

		drawing = false;
	}

	void Renderer::drawCentredText()
	{
		if (!centred_text_this_frame.empty())
		{
			if (!g_tb_screenshot_mode.isEnabled()
				|| g_tb_screenshot_mode.allow_centred_text
				)
			{
				centred_text_this_frame.pop_back();
				drawTextC(client_size.x * 0.5f, client_size.y * 0.5f, wrapTextH(std::move(centred_text_this_frame), info_scale, HUD_WIDTH / 2), ALIGN_CENTRE, g_renderer.info_scale * resolution_text_scale, hudColour, m_font_user);
			}
			centred_text_this_frame.clear();
		}
	}

	DirectX::SimpleMath::Vector2 Renderer::getCursorPosH(bool allow_click_drag) const
	{
		if (g_gui.mouse_mode != MouseMode::NONE && GetForegroundWindow() == pointers::hwnd)
		{
			if (mouse_squeaks && !allow_click_drag)
			{
				return cursorPosWhenClickStartedH;
			}
			CURSORINFO ci;
			ci.cbSize = sizeof(CURSORINFO);
			if (GetCursorInfo(&ci))
			{
				ScreenToClient(pointers::hwnd, &ci.ptScreenPos);
				return posC2H(ci.ptScreenPos);
			}
		}
		return { FLT_MAX, FLT_MAX };
	}

	enum class RootNameTruncationType : uint8_t
	{
		FULL,
		STOCK,
		STAND,
		TRUNCATE,
	};

	std::wstring Renderer::getAddressText(Command* const command, const float width) const
	{
		std::wstring address;
		if (command != nullptr)
		{
			if (address_current_list_only)
			{
				address = command->getMenuNameForAddress();
				trimTextH(address, g_menu_grid.addressbar_text.scale, width);
				return address;
			}
			int8_t path_segments = 0;
			CommandList* tab = g_gui.getCurrentMenuTab();
			CommandList* list = command->getParentWithContextMenuPassthrough(tab);
			if (show_root_name)
			{
				while (list != nullptr)
				{
					list = list->getParentWithContextMenuPassthrough(tab);
					path_segments++;
				}
			}
			else
			{
				if (list == nullptr)
				{
					// We're in the root list but root name is supposed to be hidden, so just show nothing
					return {};
				}
				while (list != g_gui.root_list.get())
				{
					list = list->getParentWithContextMenuPassthrough(tab);
					path_segments++;
					if (list == nullptr)
					{
						break;
					}
				}
			}
			auto og_path_segments = path_segments;
			auto root_name_truncation_type = RootNameTruncationType::FULL;
			std::wstring truncated_root_name;
			do
			{
				address = command->getMenuNameForAddress();
				const CommandList* list = command->getParentWithContextMenuPassthrough(tab);
				auto i = path_segments;
				while (list != nullptr && i > 0)
				{
					address.insert(0, address_separator);
					if (list == g_gui.root_list.get() && !truncated_root_name.empty())
					{
						address.insert(0, truncated_root_name);
					}
					else
					{
						address.insert(0, list->getMenuNameForAddress());
					}
					list = list->getParentWithContextMenuPassthrough(tab);
					i--;
				}
				if (path_segments != og_path_segments)
				{
					address.insert(0, address_separator);
					address.insert(0, L"...");
				}
				if (getTextWidth(address, g_menu_grid.addressbar_text.scale) <= width)
				{
					if ((path_segments == og_path_segments - 1) && root_name_truncation_type != RootNameTruncationType::STAND && root_name_truncation_type != RootNameTruncationType::TRUNCATE)
					{
						switch (root_name_truncation_type)
						{
						case RootNameTruncationType::FULL:
							root_name_truncation_type = RootNameTruncationType::STOCK;
							truncated_root_name = L"" STAND_RN_NAMEVERSION;
							break;

						case RootNameTruncationType::STOCK:
							if (tab->menu_name.isLiteralString("Stand"))
							{
								root_name_truncation_type = RootNameTruncationType::TRUNCATE;
								truncated_root_name.clear();
								goto truncate_root_name;
							}
							root_name_truncation_type = RootNameTruncationType::STAND;
							truncated_root_name = L"" STAND_NAME;
							break;
						}
						path_segments = og_path_segments;
						continue;
					truncate_root_name:;
					}
					break;
				}
				if (--path_segments < 0)
				{
					address = command->getMenuNameForAddress();
					trimTextH(address, g_menu_grid.addressbar_text.scale, width);
					break;
				}
			} while (true);
		}
		return address;
	}

	void Renderer::stop_input()
	{
		if (g_gui.keyboard_pressing != MENUKEY_NONE)
		{
			g_gui.inputStop();
		}
		m_unreleased_menu_key = 0;
	}

#define PROCESS_MENUKEY(scheme_key, menu_key) \
if (pressing.overlapsWith(Input::scheme. ## scheme_key ##)) \
{ \
	if (pressing.vk() != m_unreleased_menu_key \
		&& g_gui.keyboard_pressing != menu_key \
		&& !doesGameplayStateConflictWithVk(pressing.vk()) \
		) \
	{ \
		m_unreleased_menu_key = pressing.vk(); \
		g_gui.onKeyPressStart(menu_key, INPUTTYPE_INDIFFERENT); \
	} \
	return; \
}

	bool Renderer::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (!wndproc_jobs.empty())
		{
			for (const auto& f : wndproc_jobs)
			{
				f();
			}
			wndproc_jobs.clear();
		}

		if (msg == WM_CHAR)
		{
			if (g_commandbox.active)
			{
				bool is_open_key = false;
				if (!released_command_box)
				{
					auto key_to_check = (unsigned int)(wchar_t)wparam;
					if (key_to_check == 21) // Ctrl+U
					{
						key_to_check = 'U';
					}
					else
					{
						key_to_check = std::toupper(key_to_check);
					}
					for (const auto& hk : Input::scheme.key_command_box)
					{
						if (hk.vk() < 'A' || hk.vk() > 'Z'
							|| hk.vk() == key_to_check
							)
						{
							is_open_key = true;
							break;
						}
					}
				}
				if (!is_open_key)
				{
					if (wparam == '\t')
					{
						g_command_name_tab_completion_provider.onKeyDown();
					}
					else
					{
						g_commandbox.processChar((wchar_t)wparam);
					}
				}
			}
		}
		else if (msg == WM_KEYUP || msg == WM_SYSKEYUP || msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN)
		{
			const UINT scancode = ((lparam >> 16) & 0b11111111);
			const BOOL extended = ((lparam >> 24) & 1);
			const BOOL prev_down = ((lparam >> 30) & 0b1);
			const BOOL is_up_trans = (msg == WM_KEYUP || msg == WM_SYSKEYUP);
			for (const auto& keyboard_handler : keyboard_handlers)
			{
				keyboard_handler((DWORD)wparam, (lparam & 0b111111111111111), scancode, extended, (msg == WM_SYSKEYUP || msg == WM_SYSKEYDOWN), prev_down, is_up_trans);
			}
			unsigned int vk = MapVirtualKeyExA(scancode, MAPVK_VSC_TO_VK, GetKeyboardLayout(0));
			if (vk != 0)
			{
				if (extended)
				{
					switch (vk)
					{
					case VK_CONTROL:
						vk = VK_RCONTROL;
						break;

					case VK_MENU:
						vk = VK_RMENU;
						break;

					case VK_RETURN:
						vk = VK_NUMPAD_RETURN;
						break;
					}
				}
				else
				{
					switch (vk)
					{
					case VK_CONTROL:
						vk = VK_LCONTROL;
						break;

					case VK_MENU:
						vk = VK_LMENU;
						break;

					case VK_INSERT:
						vk = VK_NUMPAD0;
						break;

					case VK_END:
						vk = VK_NUMPAD1;
						break;

					case VK_DOWN:
						vk = VK_NUMPAD2;
						break;

					case VK_NEXT:
						vk = VK_NUMPAD3;
						break;

					case VK_LEFT:
						vk = VK_NUMPAD4;
						break;

					case VK_CLEAR:
						vk = VK_NUMPAD5;
						break;

					case VK_RIGHT:
						vk = VK_NUMPAD6;
						break;

					case VK_HOME:
						vk = VK_NUMPAD7;
						break;

					case VK_UP:
						vk = VK_NUMPAD8;
						break;

					case VK_PRIOR:
						vk = VK_NUMPAD9;
						break;

					case VK_DELETE:
						vk = VK_DECIMAL;
						break;
					}
				}
				switch (vk)
				{
				case VK_NUMPAD0:
				case VK_NUMPAD1:
				case VK_NUMPAD2:
				case VK_NUMPAD3:
				case VK_NUMPAD4:
				case VK_NUMPAD5:
				case VK_NUMPAD6:
				case VK_NUMPAD7:
				case VK_NUMPAD8:
				case VK_NUMPAD9:
				case VK_ADD:
				//case VK_SUBTRACT:
				//case VK_MULTIPLY:
				//case VK_DIVIDE:
					if (!Input::user_has_numpad)
					{
						Input::user_has_numpad = true;
						if (Tutorial::state == TUT_OPEN)
						{
							Tutorial::update();
						}
					}
					break;
				}
				const bool repeat = (is_up_trans != prev_down);
				if (vk == VK_SHIFT)
				{
					if (keyUpdown(VK_RSHIFT, (GetKeyState(VK_RSHIFT) & 0x8000), repeat))
					{
						return true;
					}
					if (keyUpdown(VK_LSHIFT, (GetKeyState(VK_LSHIFT) & 0x8000), repeat))
					{
						return true;
					}
				}
				else
				{
					if (keyUpdown(vk, (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN), repeat))
					{
						return true;
					}
				}
			}
		}
		else if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK)
		{
			mouse_squeaks = true;
			cursorPosWhenClickStartedH = getCursorPosH(true);
			keyDown(Hotkey::fromPressing(VK_LBUTTON));
		}
		else if (msg == WM_LBUTTONUP)
		{
			mouse_squeaks = false;
			keyUp(Hotkey::fromPressing(VK_LBUTTON));
		}
		else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK)
		{
			keyDown(Hotkey::fromPressing(VK_RBUTTON));
			if (g_gui.opened && g_gui.mouse_mode != MouseMode::NONE && doesGameplayStateAllowMenuNavigateInput())
			{
				g_gui.last_input_type = INPUTTYPE_MOUSE_CLICK;
				g_gui.inputBack(TC_WNDPROC, true);
			}
		}
		else if (msg == WM_RBUTTONUP)
		{
			keyUp(Hotkey::fromPressing(VK_RBUTTON));
		}
		else if (msg == WM_MBUTTONDOWN)
		{
			keyDown(Hotkey::fromPressing(VK_MBUTTON));
		}
		else if (msg == WM_MBUTTONUP)
		{
			keyUp(Hotkey::fromPressing(VK_MBUTTON));
		}
		else if (msg == WM_XBUTTONDOWN)
		{
			const WORD btn = HIWORD(wparam);
			if (btn & XBUTTON1)
			{
				keyDown(Hotkey::fromPressing(VK_XBUTTON1));
			}
			if (btn & XBUTTON2)
			{
				keyDown(Hotkey::fromPressing(VK_XBUTTON2));
			}
		}
		else if (msg == WM_XBUTTONUP)
		{
			const WORD btn = HIWORD(wparam);
			if (btn & XBUTTON1)
			{
				keyUp(Hotkey::fromPressing(VK_XBUTTON1));
			}
			if (btn & XBUTTON2)
			{
				keyUp(Hotkey::fromPressing(VK_XBUTTON2));
			}
		}
		else if (msg == WM_APPCOMMAND)
		{
			switch (GET_APPCOMMAND_LPARAM(lparam))
			{
			case APPCOMMAND_MEDIA_PLAY_PAUSE:
				keyPress(VK_MEDIA_PLAY_PAUSE);
				break;

			case APPCOMMAND_MEDIA_STOP:
				keyPress(VK_MEDIA_STOP);
				break;

			case APPCOMMAND_MEDIA_NEXTTRACK:
				keyPress(VK_MEDIA_NEXT_TRACK);
				break;

			case APPCOMMAND_MEDIA_PREVIOUSTRACK:
				keyPress(VK_MEDIA_PREV_TRACK);
				break;
			}
		}
		else if (msg == WM_ACTIVATE)
		{
			if (!(wparam & 1))
			{
				// We're about to lose focus, assume all keys are released now.
				stop_input();
				g_commandbox.enter_released = true;
				g_commandbox.escape_released = true;
				releaseAllHoldModeHotkeys();
			}
		}
		return false;
	}

	bool Renderer::keyUpdown(unsigned int vk, bool is_down, bool repeat)
	{
		Hotkey pressing = Hotkey::fromPressing(vk, repeat);
		if (is_down)
		{
			keyDown(pressing);
		}
		else
		{
			if (keyUp(pressing))
			{
				return true;
			}
		}
		return false;
	}

	void Renderer::keyPress(const unsigned int vk)
	{
		keyDown(vk);
		keyUp(vk);
	}

	void Renderer::keyDown(Hotkey pressing)
	{
		if (g_commandbox.active)
		{
			if (released_command_box)
			{
				g_commandbox.processKeydown(pressing.vk());
			}
		}
		else if (gameplayState == CHATTING)
		{
			if (pressing.overlapsWith(tab_complete_keys))
			{
				g_player_name_tab_completion_provider.onKeyDown();
			}
			else if (pressing.overlapsWith(paste_keys))
			{
				if (chat_message_length_when_paste_key_was_first_down == -1)
				{
					chat_message_length_when_paste_key_was_first_down = (*pointers::chat_box)->message_length;
				}
			}
		}
		else if (doesGameplayStateAllowMenuNavigateInput())
		{
			if (pressing.overlapsWith(Input::scheme.key_menu_open_close))
			{
				if (released_open_close)
				{
					released_open_close = false;
					if (g_gui.isAwaitingSetHotkeyInput())
					{
						g_gui.abortSetHotkey();
					}
					else
					{
						g_gui.inputOpenCloseMenu(TC_WNDPROC);
					}
				}
			}
			else if (pressing.overlapsWith(Input::scheme.key_command_box)
				&& !g_gui.isAwaitingSetHotkeyInput()
				&& Tutorial::state >= TUT_DONE
				)
			{
				released_command_box = false;
				g_gui.queueSoundJob([]
				{
					AUDIO::PLAY_SOUND_FRONTEND(-1, "SELECT", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
				});
				g_gui.showCommandBox();
			}
			else
			{
				if (g_gui.isAwaitingSetHotkeyInput())
				{
					if (!isModifierKey(pressing.vk()))
					{
						if (pressing.overlapsWith(Input::scheme.key_menu_context))
						{
							g_gui.abortSetHotkey();
						}
						else
						{
							unreleased_hotkey_candiate = pressing.vk();
						}
					}
				}
				else
				{
					if (!g_gui.hotkeys_disabled
						&& !pressing.isRepeat()
						&& (!g_gui.opened || !(pressing.overlapsWith(Input::scheme.key_menu_click) || pressing.overlapsWith(Input::scheme.key_menu_up) || pressing.overlapsWith(Input::scheme.key_menu_down) || pressing.overlapsWith(Input::scheme.key_menu_left) || pressing.overlapsWith(Input::scheme.key_menu_right)))
						)
					{
						FiberPool::queueJob([pressing]
						{
							auto hold_mode_matches = g_gui.triggerHotkeyDown(pressing);
							for (auto& hk : hold_mode_matches)
							{
								if (std::find(g_renderer.unreleased_hold_mode_hotkeys.cbegin(), g_renderer.unreleased_hold_mode_hotkeys.cend(), hk) == g_renderer.unreleased_hold_mode_hotkeys.end())
								{
									g_renderer.unreleased_hold_mode_hotkeys.emplace_back(std::move(hk));
								}
							}
						});
					}
				}
				PROCESS_MENUKEY(key_menu_click, MENUKEY_CLICK);
				PROCESS_MENUKEY(key_menu_up, MENUKEY_UP);
				PROCESS_MENUKEY(key_menu_down, MENUKEY_DOWN);
				PROCESS_MENUKEY(key_menu_left, MENUKEY_LEFT);
				PROCESS_MENUKEY(key_menu_right, MENUKEY_RIGHT);
				if (g_gui.opened)
				{
					PROCESS_MENUKEY(key_menu_root_up, MENUKEY_ROOT_UP);
					PROCESS_MENUKEY(key_menu_root_down, MENUKEY_ROOT_DOWN);
					PROCESS_MENUKEY(key_menu_back, MENUKEY_BACK);
					if (pressing.overlapsWith(Input::scheme.key_menu_context))
					{
						if (released_context)
						{
							released_context = false;
							ContextMenu::toggleIfAvailable(TC_WNDPROC);
						}
						return;
					}
				}
			}
		}
	}

	bool Renderer::keyUp(Hotkey pressing)
	{
		if (pressing.overlapsWith(Input::scheme.key_menu_open_close))
		{
			released_open_close = true;
		}
		else if (pressing.overlapsWith(Input::scheme.key_command_box))
		{
			released_command_box = true;
		}
		else if (pressing.overlapsWith(Input::scheme.key_menu_context))
		{
			released_context = true;
		}
		else if (pressing.vk() == VK_RETURN || pressing.vk() == VK_NUMPAD_RETURN)
		{
			g_commandbox.enter_released = true;
		}
		else if (pressing.vk() == VK_ESCAPE)
		{
			g_commandbox.escape_released = true;
		}

		if (doesGameplayStateAllowMenuNavigateInput())
		{
			for (auto i = unreleased_hold_mode_hotkeys.begin(); i != unreleased_hold_mode_hotkeys.end(); )
			{
				auto hotkey = *i;
				if (hotkey.isPressedAsync())
				{
					++i;
				}
				else
				{
					i = unreleased_hold_mode_hotkeys.erase(i);

					FiberPool::queueJob([hotkey]
					{
						g_gui.triggerHotkeyUp(hotkey);
					});
				}
			}
		}
		else
		{
			releaseAllHoldModeHotkeys();
		}

		if (g_gui.isAwaitingSetHotkeyInput())
		{
			if (pressing.vk() == unreleased_hotkey_candiate)
			{
				g_gui.onSetHotkeyInput(pressing);
				unreleased_hotkey_candiate = 0;
			}
			else if (unreleased_hotkey_candiate == 0 && isModifierKey(pressing.vk()))
			{
				g_gui.onSetHotkeyInput(Hotkey(pressing.vk()));
			}
		}

		if (m_unreleased_menu_key == pressing.vk())
		{
			stop_input();
		}
		else if (pressing.vk() == VK_HOME)
		{
			if (g_commandbox.active)
			{
				return true;
			}
			gameplayState = OVERLAY;
		}
		else if (pressing.isAnyVkEqual(tab_complete_keys) && g_player_name_tab_completion_provider.canHandleKeyUp())
		{
			g_player_name_tab_completion_provider.onKeyUp();
		}
		else if (pressing.vk() == VK_TAB && g_command_name_tab_completion_provider.canHandleKeyUp())
		{
			g_command_name_tab_completion_provider.onKeyUp();
		}
		else if (pressing.isAnyVkEqual(paste_keys) && chat_message_length_when_paste_key_was_first_down != -1)
		{
			Chat::removeFromDraft((*pointers::chat_box)->message_length - chat_message_length_when_paste_key_was_first_down);
			chat_message_length_when_paste_key_was_first_down = -1;
			std::optional<std::wstring> text = Util::getClipboardTextUTF16();
			if (text.has_value())
			{
				Chat::addToDraft(text.value());
			}
		}
		return pressing.vk() == VK_NUMPAD7 && suppress_numpad_7_messages;
	}

	void Renderer::releaseAllHoldModeHotkeys()
	{
		if (!unreleased_hold_mode_hotkeys.empty())
		{
			for (const auto& hotkey : unreleased_hold_mode_hotkeys)
			{
				FiberPool::queueJob([hotkey]
				{
					g_gui.triggerHotkeyUp(hotkey);
				});
			}
			unreleased_hold_mode_hotkeys.clear();
		}
	}

	bool Renderer::isModifierKey(const unsigned int vk)
	{
		return vk == VK_CONTROL || vk == VK_SHIFT || vk == VK_MENU;
	}

	void Renderer::queueWndProcJob(void f())
	{
		wndproc_jobs.emplace_back(f);
	}

	void Renderer::ensureWndProcContext(void f())
	{
		if (ExecCtx::get().tc == TC_WNDPROC)
		{
			f();
		}
		else
		{
			wndproc_jobs.emplace_back(f);
		}
	}
}
