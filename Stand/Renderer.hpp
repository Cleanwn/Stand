#pragma once

#include <set>

#include <soup/Promise.hpp>

#include "fwddecl.hpp"
#include "dx_common.hpp"

#include "Alignment.hpp"
#include "asi_decl.hpp"
#include "BinaryBlob.hpp"
#include "Direction.hpp"
#include "function_types.hpp"
#include "Hotkey.hpp"
#include "Spinlock.hpp"
#include "StateSaver.hpp"
#include "StringCastable.hpp"
#include "TextSettings.hpp"
#include "Texture.hpp"
#include "TextureArray.hpp"
#include "vector.hpp"

#define DRAW_HTML false

namespace Stand
{
	/* 2D Coordinate Systems! Yes, plural!

	 * C = Client coordinate system, {0, 0} to whatever the client's resolution is.
	 * CP = X & Y are percentages from 0.0 to 1.0 based on the limits of the client coordinate system. 
	 * H = HUD coordinate system, superimposed 1920x1080, guarantees that every coordinate is within visible bounds.
	 * HP = X & Y are percentages from 0.0 to 1.0 based on the limits of the HUD coordinate system.
	 * S = Square coordinate system, superimposed 1:1 that goes from 0.0 to 1.0 on each axis,
	       guarantees that every part of the screen can be addressed, but can also go out of screen bounds,
		   and is best only used for points.

	 * If no coordinate system is provided, that's some legacy shit that's best rewritten. */

	struct ReloadTex
	{
		int id;
		std::wstring name;

		ReloadTex(int id, std::wstring name)
			: id(id), name(name)
		{
		}
	};

	enum GameplayState : uint8_t
	{
		OVERLAY,
		COMMANDBOX,
		PAUSED,
		SUSPENDED,
		CHATTING,
		MENUING,
		PLAYING
	};

	struct DrawData
	{
		enum Type : uint8_t
		{
			TEXT,
			CENTRED_TEXT,
			LINE,
			RECT,
			TRIANGLE,
			SPRITE,
			SPRITE_BY_POINTS,
			CIRCLE,
			BGBLUR,
#if DRAW_HTML
			HTML,
#endif
		};

		const Type type;

		explicit DrawData(Type&& type)
			: type(std::move(type))
		{
		}

		virtual ~DrawData() = default;
	};

	struct DrawTextData : public DrawData
	{
		const float client_x;
		const float client_y;
		const std::wstring text;
		const Alignment alignment;
		const float client_scale;
		const DirectX::SimpleMath::Color colour;
		const soup::SharedPtr<SpriteFont> font;
		const bool force_in_bounds;
		const v3 draw_origin_3d;
		const uint8_t drop_shadow;

		explicit DrawTextData(float client_x, float client_y, std::wstring&& text, Alignment alignment, float client_scale, const DirectX::SimpleMath::Color& colour, const soup::SharedPtr<SpriteFont>& font, bool force_in_bounds, const v3& draw_origin_3d, uint8_t drop_shadow)
			: DrawData(TEXT), client_x(client_x), client_y(client_y), text(std::move(text)), alignment(alignment), client_scale(client_scale), colour(colour), font(font), force_in_bounds(force_in_bounds), draw_origin_3d(draw_origin_3d), drop_shadow(drop_shadow)
		{
		}
	};

	struct DrawCentredTextData : public DrawData
	{
		const std::wstring text;

		explicit DrawCentredTextData(std::wstring&& text)
			: DrawData(CENTRED_TEXT), text(std::move(text))
		{
		}
	};

	struct DrawLineData : public DrawData
	{
		const float x1;
		const float y1;
		const float x2;
		const float y2;
		const DirectX::SimpleMath::Color colour1;
		const DirectX::SimpleMath::Color colour2;

		explicit DrawLineData(const float x1, const float y1, const float x2, const float y2, const DirectX::SimpleMath::Color& colour1, const DirectX::SimpleMath::Color& colour2)
			: DrawData(LINE), x1(x1), y1(y1), x2(x2), y2(y2), colour1(colour1), colour2(colour2)
		{
		}
	};

	struct DrawRectData : public DrawData
	{
		const DirectX::SimpleMath::Vector2 pos1S;
		const DirectX::SimpleMath::Vector2 pos2S;
		const DirectX::SimpleMath::Color colour;

		explicit DrawRectData(DirectX::SimpleMath::Vector2&& pos1S, DirectX::SimpleMath::Vector2&& pos2S, const DirectX::SimpleMath::Color& colour)
			: DrawData(RECT), pos1S(std::move(pos1S)), pos2S(std::move(pos2S)), colour(colour)
		{
		}
	};

	struct DrawTriangleData : public DrawData
	{
		const DirectX::SimpleMath::Vector2 pos1S;
		const DirectX::SimpleMath::Vector2 pos2S;
		const DirectX::SimpleMath::Vector2 pos3S;
		const DirectX::SimpleMath::Color colour;

		explicit DrawTriangleData(DirectX::SimpleMath::Vector2&& pos1S, DirectX::SimpleMath::Vector2&& pos2S, DirectX::SimpleMath::Vector2&& pos3S, DirectX::SimpleMath::Color&& colour)
			: DrawData(TRIANGLE), pos1S(std::move(pos1S)), pos2S(std::move(pos2S)), pos3S(std::move(pos3S)), colour(std::move(colour))
		{
		}
	};

	struct DrawSpriteData : public DrawData
	{
		int tex_id;
		float sizeX;
		float sizeY;
		float centerX;
		float centerY;
		float posX;
		float posY;
		float rotation;
		float r;
		float g;
		float b;
		float a;

		explicit DrawSpriteData(int tex_id, float sizeX, float sizeY, float centerX, float centerY, float posX, float posY, float rotation, float r, float g, float b, float a)
			: DrawData(SPRITE), tex_id(tex_id), sizeX(sizeX), sizeY(sizeY), centerX(centerX), centerY(centerY), posX(posX), posY(posY), rotation(rotation), r(r), g(g), b(b), a(a)
		{
		}

		explicit DrawSpriteData(const DrawSpriteData& b)
			: DrawSpriteData(b.tex_id, b.sizeX, b.sizeY, b.centerX, b.centerY, b.posX, b.posY, b.rotation, b.r, b.g, b.b, b.a)
		{
		}

		void operator=(const DrawSpriteData& b)
		{
			this->tex_id = b.tex_id;
			this->sizeX = b.sizeX;
			this->sizeY = b.sizeY;
			this->centerX = b.centerX;
			this->centerY = b.centerY;
			this->posX = b.posX;
			this->posY = b.posY;
			this->rotation = b.rotation;
			this->r = b.r;
			this->g = b.g;
			this->b = b.b;
			this->a = b.a;
		}

		void draw() const;
	};

	struct TimedSprite
	{
		time_t deadline;
		DrawSpriteData draw_data;

		explicit TimedSprite(time_t&& deadline, DrawSpriteData&& draw_data)
			: deadline(std::move(deadline)), draw_data(std::move(draw_data))
		{
		}

		void operator=(const TimedSprite& b)
		{
			deadline = b.deadline;
			draw_data = b.draw_data;
		}
	};

	struct DrawSpriteByPointsData : public DrawData
	{
		int tex_id;
		float x1;
		float y1;
		float x2;
		float y2;
		DirectX::SimpleMath::Color colour;

		explicit DrawSpriteByPointsData(int tex_id, float x1, float y1, float x2, float y2, const DirectX::SimpleMath::Color& colour)
			: DrawData(SPRITE_BY_POINTS), tex_id(tex_id), x1(x1), y1(y1), x2(x2), y2(y2), colour(colour)
		{
		}
	};

	struct Circle
	{
		float radius;
		std::vector<DirectX::SimpleMath::Vector2> points;

		Circle(size_t resolution);

		static Circle inst_50;
		static Circle inst_100;

		void setRadius(float r);
	};

	struct DrawCircleData : public DrawData
	{
		float x;
		float y;
		float radius;
		int part;
		DirectX::SimpleMath::Color colour;

		explicit DrawCircleData(float x, float y, float radius, int part, const DirectX::SimpleMath::Color& colour)
			: DrawData(CIRCLE), x(x), y(y), radius(radius), part(part), colour(colour)
		{
		}
	};

	struct DrawBgBlurData : public DrawData
	{
		soup::WeakRef<EphemeralBackgroundBlur> wr;
		float x;
		float y;
		float width;
		float height;
		uint8_t passes;

		explicit DrawBgBlurData(soup::WeakRef<EphemeralBackgroundBlur>&& wr, float x, float y, float width, float height, uint8_t passes)
			: DrawData(BGBLUR), wr(std::move(wr)), x(x), y(y), width(width), height(height), passes(passes)
		{
		}
	};

#if DRAW_HTML
	struct DrawHtmlData : public DrawData
	{
		int x;
		int y;
		unsigned int width;
		unsigned int height;
		std::string html;

		explicit DrawHtmlData(int x, int y, unsigned int width, unsigned int height, std::string&& html)
			: DrawData(HTML), x(x), y(y), width(width), height(height), html(std::move(html))
		{
		}
	};
#endif

	enum class HeaderState : uint8_t
	{
		SHOW,
		HIDE,
		DOWNLOADING,
		LOADING,
	};

	using PresentCallback = void(*)();

	class Renderer
	{
	private:
		time_t last_draw = 0;
	public:
		Spinlock create_sprite_mtx = {};
		int next_script_texture_id = 0;
		std::unordered_map<int, std::wstring> CreateTextureArray = {};
		bool creating_textures = false;
		Spinlock timed_sprites_mtx = {};
		std::vector<TimedSprite> timed_sprites = {};
		Spinlock script_textures_mtx = {};
		std::unordered_map<int, Texture> script_textures = {};
	private:
		std::vector<ReloadTex> ReloadArray = {};

	public:
		bool inited = false;
		ID3D11Device* m_pDevice = nullptr;
		ID3D11DeviceContext* m_pContext = nullptr;
		std::unique_ptr<SpriteBatch> m_pSpriteBatch;
		std::unique_ptr<DirectX::BasicPostProcess> postProcess;
	private:
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
		std::unique_ptr<StateSaver> m_stateSaver;
	public:
		std::unique_ptr<DirectX::CommonStates> m_pCommonState;
	private:
		std::unique_ptr<DirectX::BasicEffect> m_effect;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	public:
		ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
		ID3D11Texture2D* m_pRenderTargetTexture = nullptr;
		soup::SharedPtr<DirectX::SpriteFont> m_font_user;
		soup::SharedPtr<DirectX::SpriteFont> m_font_yahei;
		soup::SharedPtr<DirectX::SpriteFont> m_font_nanumgothic;
		soup::SharedPtr<DirectX::SpriteFont> m_font_bevietnamprolight;

	public:
		Texture textureToggleOff;
		Texture textureToggleOn;
		Texture textureToggleOffAuto;
		Texture textureToggleOnAuto;
		Texture textureToggleOffList;
		Texture textureToggleOnList;
		Texture textureList;
		Texture textureLink;
		Texture textureEnabled;
		Texture textureDisabled;
		Texture textureUser;
		Texture textureFriends;
		Texture textureUsers;
		Texture textureEdit;
		Texture textureSearch;
		Texture textureHeaderLoading;

		Texture textureWantedStar;
		Texture textureLock;
		Texture textureRs;
		Texture textureRsVerified;
		Texture textureRsCreated;
		Texture textureBlankbox;
		Texture textureNewline;
		Texture textureReset;
		Texture textureUni0000;
		Texture textureUni26A0;
		Texture textureUni2728;
		Texture textureUni2764;
		Texture textureUni1F4AF;
		Texture textureUni1F60A;
		Texture textureUni1F480;
		Texture textureUni1F525;
		Texture textureUni1F602;
		Texture textureUni1F629;
		Texture textureUni1F633;

		Texture textureTabSelf;
		Texture textureTabVehicle;
		Texture textureTabOnline;
		Texture textureTabPlayers;
		Texture textureTabWorld;
		Texture textureTabGame;
		Texture textureTabStand;

		Texture textureCustom00;
		Texture textureCustom01;
		Texture textureCustom02;
		Texture textureCustom03;
		Texture textureCustom04;
		Texture textureCustom05;
		Texture textureCustom06;
		Texture textureCustom07;
		Texture textureCustom08;
		Texture textureCustom09;
		Texture textureCustom0A;
		Texture textureCustom0B;
		Texture textureCustom0C;
		Texture textureCustom0D;
		Texture textureCustom0E;
		Texture textureCustom0F;
		Texture textureCustom10;
		Texture textureCustom11;
		Texture textureCustom12;
		Texture textureCustom13;
		Texture textureCustom14;
		Texture textureCustom15;
		Texture textureCustom16;
		Texture textureCustom17;
		Texture textureCustom18;
		Texture textureCustom19;
		Texture textureCustom1A;
		Texture textureCustom1B;
		Texture textureCustom1C;
		Texture textureCustom1D;
		Texture textureCustom1E;
		Texture textureCustom1F;

	public:
		HeaderState header_state = HeaderState::HIDE;
		uint16_t header_progress = 0;
		uint16_t header_goal = 0;
		bool header_bgblur = false;
		std::shared_ptr<TextureArray> header;
		size_t header_i = 0;
		time_t header_last_frame = 0;
		time_t header_ms_passed = 0;
		time_t header_speed = 32;

		const wchar_t* address_separator = L" > ";

		BOOL m_fullscreenState = -1;
		DirectX::SimpleMath::Vector2 client_size = {};
		float client_ratio = 1.0f;
		float resolution_text_scale = 1.0f;
		DirectX::SimpleMath::Vector2 hudCorrectionC = {};
		std::set<AsiPresentCallback> asi_present_callbacks = {};
		DirectX::SimpleMath::Color focusRectColour = { 1.0f, 0.0f, 1.0f, 1.0f };
		DirectX::SimpleMath::Color bgRectColour = { 0.0f, 0.0f, 0.0f, 0.3019f };
		DirectX::SimpleMath::Color focusTextColour = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::SimpleMath::Color focusRightTextColour = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::SimpleMath::Color focusSpriteColour = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::SimpleMath::Color bgTextColour = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::SimpleMath::Color bgRightTextColour = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::SimpleMath::Color bgSpriteColour = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::SimpleMath::Color hudColour = { 1.0f, 0.0f, 1.0f, 1.0f };
		DirectX::SimpleMath::Color arColour = { 1.0f, 0.0f, 1.0f, 0.78431f };
		DirectX::SimpleMath::Color minigameColour = { 1.0f, 0.0f, 1.0f, 1.0f };
		DirectX::SimpleMath::Color notifyBorderColour = { 1.0f, 0.0f, 1.0f, 1.0f };
		DirectX::SimpleMath::Color notifyFlashColour = { 0.6196f, 0.0f, 0.6196f, 1.0f };
		DirectX::SimpleMath::Color notifyBgColour = { 0.0f, 0.0f, 0.0f, 0.3019f };
		bool drawTextDebugRects = false;
		TextSettings small_text{ float(12.0 * TEXT_HEIGHT_PX * 2.0), -2.0f, 2.0f };
		TextSettings commandbox_input_text{ float(15.0 * TEXT_HEIGHT_PX * 2.0), -2.0f, 2.0f };
		bool show_addressbar = true;
		bool address_current_list_only = false;
		bool show_root_name = true;
		int16_t addressbar_height = 24;

		Direction tabs_pos = LEFT;
		int16_t tabs_width = 112;
		int16_t tabs_height = 32;
		TextSettings tabs_text{ float(15.0 * TEXT_HEIGHT_PX * 2.0), -2.0f, 2.0f };
		Alignment tabs_alignment = ALIGN_TOP_LEFT;

		int16_t command_height = 32;
		int16_t command_width = 450;
		TextSettings command_text{ float(15.0 * TEXT_HEIGHT_PX * 2.0), -2.0f, 2.0f };

		int16_t cursor_border_width = 0;
		DirectX::SimpleMath::Color cursor_border_colour = { 1.0f, 0.0f, 1.0f, 1.0f };
		bool cursor_border_rounded = false;
		uint8_t background_blur = 4;
		float info_x = 1886.0f;
		float info_y = 98.0f;
		Alignment info_alignment = ALIGN_TOP_RIGHT;
		float info_scale = float(19.0 * TEXT_HEIGHT_PX * 2.0);
		Alignment info_text_alignment = ALIGN_TOP_LEFT;
		uint8_t info_drop_shadow = 1;
		bool preview_colour_in_list = true;
		bool preview_colour_in_sprite = true;
		[[nodiscard]] const DirectX::SimpleMath::Color* getBgRectColourPtr() const;
		[[nodiscard]] const DirectX::SimpleMath::Color& getBgRectColour() const;
		[[nodiscard]] const DirectX::SimpleMath::Color* getFocusRectColourPtr() const;
		[[nodiscard]] const DirectX::SimpleMath::Color& getFocusRectColour() const;
		[[nodiscard]] const DirectX::SimpleMath::Color& getFocusTextColour() const;
		[[nodiscard]] const DirectX::SimpleMath::Color& getFocusRightTextColour() const;
		[[nodiscard]] const DirectX::SimpleMath::Color& getFocusSpriteColour() const;
		[[nodiscard]] const DirectX::SimpleMath::Color& getBgTextColour() const;
		[[nodiscard]] const DirectX::SimpleMath::Color& getBgRightTextColour() const;
		[[nodiscard]] const DirectX::SimpleMath::Color& getBgSpriteColour() const;
		[[nodiscard]] int16_t getCursorBorderWidth() const;
		[[nodiscard]] bool getCursorBorderRounded() const;
		[[nodiscard]] const DirectX::SimpleMath::Color& getCursorBorderColour() const;

		Spinlock extras_mtx = {};
		std::vector<std::unique_ptr<DrawData>> extras = {};
		std::vector<std::unique_ptr<DrawData>> extras_buffer = {};

		bool throttling_rendering = false;
		std::wstring centred_text_this_frame = {};
	private:
		bool released_open_close = true;
		bool released_command_box = true;
		bool released_context = true;
	public:
		bool mouse_squeaks = false;
		DirectX::SimpleMath::Vector2 cursorPosWhenClickStartedH = {};

		bool stream_proof = false;

		void initialiseDevices();
		[[nodiscard]] static std::wstring getThemePath();

		[[nodiscard]] bool isUserFontAcceptableForLanguage() const;
		[[nodiscard]] static BinaryBlob getYahei();
		[[nodiscard]] static BinaryBlob getNanumGothic();
		[[nodiscard]] static BinaryBlob getBeVietnamProLight();
		[[nodiscard]] static int getFontForLanguage();
		void switchToPresetFont(int id);
		static void writeFont(const BinaryBlob& blob);
		void loadUserFont();
		[[nodiscard]] soup::SharedPtr<SpriteFont> loadFont(const BinaryBlob& blob);
		static void setupFont(SpriteFont& font);

		static void reloadTextures(std::wstring&& theme_path);
		void reloadTexture(const std::wstring& theme_path, const wchar_t* const name, Texture& texture, const char* const data, const size_t data_size);
		bool reloadTexture(const std::wstring& file, Texture& texture);
		void reloadTextureFromMemory(Texture& texture, const char* const data, const size_t data_size);
		bool loadTexture(const std::wstring& file, TextureArray& arr);
		void releaseDevices();

		void enterDrawContext() const;
		void leaveDrawContext() const;

		void enterSpriteDrawContextNonPremultiplied() const;
		void leaveSpriteDrawContextNonPremultiplied() const noexcept;

	private:
		void enterSpriteDrawContext(ID3D11BlendState* blendState) const;
		void leaveSpriteDrawContext() const noexcept;

	public:
		[[nodiscard]] bool areTabsEnabled() const noexcept;

		GameplayState gameplayState = PLAYING;
		time_t lastTickTime;
		[[nodiscard]] bool doesGameplayStateAllowMenuNavigateInput() const noexcept;
		[[nodiscard]] bool doesGameplayStateConflictWithVk(int vk) const noexcept;
		[[nodiscard]] bool doesGameplayStateAllowRendering() const noexcept;
		[[nodiscard]] bool doesGameplayStateAllowScriptExecution() const noexcept;

		[[nodiscard]] bool isUltrawide() const noexcept;
		[[nodiscard]] bool isSuperwide() const noexcept;

		[[nodiscard]] static float square_x_max() noexcept;
		[[nodiscard]] float square_y_max() const noexcept;
		[[nodiscard]] DirectX::SimpleMath::Vector2 sizeC2H(DirectX::SimpleMath::Vector2 v) const noexcept;
		[[nodiscard]] DirectX::SimpleMath::Vector2 sizeC2H(float x, float y) const noexcept;
		[[nodiscard]] DirectX::SimpleMath::Vector2 posC2H(DirectX::SimpleMath::Vector2 v) const noexcept;
		[[nodiscard]] DirectX::SimpleMath::Vector2 posC2H(float x, float y) const noexcept;
		[[nodiscard]] DirectX::SimpleMath::Vector2 posC2H(const POINT& p) const noexcept;
		[[nodiscard]] DirectX::SimpleMath::Vector2 sizeH2C(DirectX::SimpleMath::Vector2 v) const noexcept;
		[[nodiscard]] DirectX::SimpleMath::Vector2 sizeH2C(double x, double y) const noexcept;
		[[nodiscard]] DirectX::SimpleMath::Vector2 posH2C(DirectX::SimpleMath::Vector2 v) const noexcept;
		[[nodiscard]] DirectX::SimpleMath::Vector2 posH2C(double x, double y) const noexcept;
		[[nodiscard]] DirectX::SimpleMath::Vector2 C2S(DirectX::SimpleMath::Vector2 v) const noexcept;
		[[nodiscard]] DirectX::SimpleMath::Vector2 C2S(float x, float y) const noexcept;
		[[nodiscard]] DirectX::SimpleMath::Vector2 S2C(DirectX::SimpleMath::Vector2 v) const noexcept;
		[[nodiscard]] DirectX::SimpleMath::Vector2 S2C(float x, float y) const noexcept;
		[[nodiscard]] DirectX::SimpleMath::Vector2 HP2H(DirectX::SimpleMath::Vector2 v) const noexcept;
		[[nodiscard]] DirectX::SimpleMath::Vector2 HP2H(float x, float y) const noexcept;
		void HP2H(double& x, double& y) const noexcept;
		[[nodiscard]] DirectX::SimpleMath::Vector2 H2HP(DirectX::SimpleMath::Vector2 v) const noexcept;

		template <typename T = DirectX::SimpleMath::Vector2, typename T2 = DirectX::SimpleMath::Vector2>
		[[nodiscard]] T C2CP(T2 v) const noexcept
		{
			return T(
				v.x / client_size.x,
				v.y / client_size.y
			);
		}

		[[nodiscard]] DirectX::SimpleMath::Vector2 CP2C(DirectX::SimpleMath::Vector2 v) const noexcept;
		[[nodiscard]] DirectX::SimpleMath::Vector2 CP2C(float x, float y) const noexcept;
		[[nodiscard]] VertexPositionColor S2VPC(float x, float y, const DirectX::SimpleMath::Color& colour) const noexcept;
		[[nodiscard]] static VertexPositionColor CP2VPC(float x, float y, const DirectX::SimpleMath::Color& colour) noexcept;

		[[nodiscard]] static DirectX::SimpleMath::Color convert_to_dx_colour(int r, int g, int b, int a) noexcept;
		[[nodiscard]] static DirectX::SimpleMath::Color propagateAlpha(const DirectX::SimpleMath::Color& c) noexcept;
		static void propagateAlpha(DirectX::SimpleMath::Color& c) noexcept;

		void drawTriangleS(float a_x, float a_y, const DirectX::SimpleMath::Color& a_colour, float b_x, float b_y, const DirectX::SimpleMath::Color& b_colour, float c_x, float c_y, const DirectX::SimpleMath::Color& c_colour);
		void drawTriangleS(float a_x, float a_y, float b_x, float b_y, float c_x, float c_y, const DirectX::SimpleMath::Color& colour);
		void drawTriangleThisTickS(DirectX::SimpleMath::Vector2&& pos1, DirectX::SimpleMath::Vector2&& pos2, DirectX::SimpleMath::Vector2&& pos3, DirectX::SimpleMath::Color&& colour);
		void drawTriangleThisTickC(const DirectX::SimpleMath::Vector2& pos1, const DirectX::SimpleMath::Vector2& pos2, const DirectX::SimpleMath::Vector2& pos3, DirectX::SimpleMath::Color&& colour);
		void drawTriangleThisTickCP(float a_x, float a_y, float b_x, float b_y, float c_x, float c_y, DirectX::SimpleMath::Color&& colour);
		void drawTriangleThisTickHP(float a_x, float a_y, float b_x, float b_y, float c_x, float c_y, DirectX::SimpleMath::Color&& colour);
		void drawHollowTriangleS(float a_x, float a_y, const DirectX::SimpleMath::Color& a_colour, float b_x, float b_y, const DirectX::SimpleMath::Color& b_colour, float c_x, float c_y, const DirectX::SimpleMath::Color& c_colour);
		void drawHollowTriangleS(float a_x, float a_y, float b_x, float b_y, float c_x, float c_y, const DirectX::SimpleMath::Color& colour);
		void drawRectByPointsS(float left_x, float top_y, float right_x, float bottom_y, const DirectX::SimpleMath::Color& top_left_colour, const DirectX::SimpleMath::Color& top_right_colour, const DirectX::SimpleMath::Color& bottom_left_colour, const DirectX::SimpleMath::Color& bottom_right_colour);
		void drawRectByPointsS(float left_x, float top_y, float right_x, float bottom_y, const DirectX::SimpleMath::Color& colour);
		void drawRectByPointsS(const DirectX::SimpleMath::Vector2& pos1, const DirectX::SimpleMath::Vector2& pos2, const DirectX::SimpleMath::Color& colour);
		
		void drawCircleS(float x, float y, const Circle& circle, const DirectX::SimpleMath::Color& inner_colour, const DirectX::SimpleMath::Color& outer_colour);
		void drawCircleS(float x, float y, const Circle& circle, const DirectX::SimpleMath::Color& colour);
		void drawCornerCircleS(float x, float y, const Circle& circle, const size_t offset, const DirectX::SimpleMath::Color& inner_colour, const DirectX::SimpleMath::Color& outer_colour);
		void drawCornerCircleS(float x, float y, const Circle& circle, const size_t offset, const DirectX::SimpleMath::Color& colour);
		void drawHueCircleS(float x, float y, const Circle& circle);
		void drawWireframeCircleS(float x, float y, const Circle& circle, const DirectX::SimpleMath::Color& inner_colour, const DirectX::SimpleMath::Color& outer_colour);
		void drawWireframeCircleS(float x, float y, const Circle& circle, const DirectX::SimpleMath::Color& colour);
		void drawHollowCircleS(float x, float y, const Circle& circle, const DirectX::SimpleMath::Color& colour);
		void drawCircleThisTickS(float x, float y, float radius, int part, const DirectX::SimpleMath::Color& colour);

		int createTexture(const char* texFileName);
		[[nodiscard]] bool isTextureStillInCreateQueue(int id);
		[[nodiscard]] Texture* getScriptTexture(int id);
		void drawTextureTimedCP(int id, int time, float sizeX, float sizeY, float centerX, float centerY, float posX, float posY, float rotation, float r, float g, float b, float a);
		void drawTextureThisTickCP(int id, float sizeX, float sizeY, float centerX, float centerY, float posX, float posY, float rotation, float r, float g, float b, float a);
		void drawSpriteByPointsThisTickHP(int id, float x1, float y1, float x2, float y2, const DirectX::SimpleMath::Color& colour);
		void drawSpriteByPointsC(ID3D11ShaderResourceView* texture, const RECT& rect, const DirectX::SimpleMath::Color& colour);
		void drawSpriteByPointsC(ID3D11ShaderResourceView* texture, const LONG x1, const LONG y1, const LONG x2, const LONG y2, const DirectX::SimpleMath::Color& colour);
		void drawSpriteC(ID3D11ShaderResourceView* texture, const LONG x, const LONG y, const LONG width, const LONG height, const DirectX::SimpleMath::Color& colour = { 1.0f, 1.0f, 1.0f, 1.0f });
		void drawSpriteC(ID3D11ShaderResourceView* texture, const DirectX::SimpleMath::Vector2& pos, const DirectX::SimpleMath::Vector2& size, const DirectX::SimpleMath::Color& colour = { 1.0f, 1.0f, 1.0f, 1.0f });
		void drawSpriteByPointsH(ID3D11ShaderResourceView* texture, float x1, float y1, float x2, float y2, const DirectX::SimpleMath::Color& colour);
		void drawSpriteH(ID3D11ShaderResourceView* texture, float x, float y, float width, float height, const DirectX::SimpleMath::Color& colour = {1.0f, 1.0f, 1.0f, 1.0f});
		[[nodiscard]] Texture* getSpriteByMenuName(const Label& menu_name);
		[[nodiscard]] bool drawSpriteH(ID3D11ShaderResourceView* texture, const LONG x, const LONG y, const LONG width, const LONG height, const DirectX::SimpleMath::Color& colour, const POINT& cursor_pos);
		void drawRectByPointsC(const DirectX::SimpleMath::Vector2& pos1, const DirectX::SimpleMath::Vector2& pos2, const DirectX::SimpleMath::Color& colour);
		void drawRectByPointsH(double x1, double y1, double x2, double y2, const DirectX::SimpleMath::Color& colour);
		void drawRectC(const DirectX::SimpleMath::Vector2& pos, const DirectX::SimpleMath::Vector2& size, const DirectX::SimpleMath::Color& colour);
		void drawRectC(const float x, const float y, const float width, const float height, const DirectX::SimpleMath::Color& colour);
		void drawRectH(double x, double y, double width, double height, const DirectX::SimpleMath::Color& colour);
		[[nodiscard]] bool drawRectH(const float x, const float y, float width, float height, const DirectX::SimpleMath::Color& colour, const DirectX::SimpleMath::Vector2& cursorPosH);
		void drawBorderC(const DirectX::SimpleMath::Vector2& pos, const DirectX::SimpleMath::Vector2& size, const float border_width, const DirectX::SimpleMath::Color& border_colour, bool rounded);
		void drawBorderH(const float x, const float y, const float width, const float height, const float border_width, const DirectX::SimpleMath::Color& border_colour, bool rounded);
		void drawRoundedBorderH(const float x, const float y, const float width, const float height, const float border_width, const DirectX::SimpleMath::Color& border_colour);
		void drawRoundedBorderC(const float x, const float y, const float width, const float height, const float border_width, const DirectX::SimpleMath::Color& border_colour);
		void drawBorderH(const float x, const float y, const float width, const float height, const float border_width, const DirectX::SimpleMath::Color& border_colour);
		void drawBorderC(const float x, const float y, const float width, const float height, const float border_width, const DirectX::SimpleMath::Color& border_colour);
		void drawInnerBorderH(const float x, const float y, const float width, const float height, const float border_width, const DirectX::SimpleMath::Color& border_colour);
		void drawHudspaceBorder();
		void drawRectByPointsThisTickS(DirectX::SimpleMath::Vector2&& pos1, DirectX::SimpleMath::Vector2&& pos2, const DirectX::SimpleMath::Color& colour);
		void drawRectByPointsThisTickC(const DirectX::SimpleMath::Vector2& pos1, const DirectX::SimpleMath::Vector2& pos2, const DirectX::SimpleMath::Color& colour);
		void drawRectByPointsThisTickCP(const float x1, const float y1, const float x2, const float y2, const DirectX::SimpleMath::Color& colour);
		void drawRectByPointsThisTickHP(double x1, double y1, double x2, double y2, const DirectX::SimpleMath::Color& colour);
		void drawRectThisTickHP(double x, double y, double width, double height, const DirectX::SimpleMath::Color& colour);
		void drawRectThisTickCP(const float x, const float y, const float width, const float height, const DirectX::SimpleMath::Color& colour);

		[[nodiscard]] const Texture* characterToTexture(uint32_t codepoint) const;
		bool tab_textures_colourable = true;
		[[nodiscard]] bool isCharacterTextureColourable(uint32_t codepoint) const;
		void findGlyph(const SpriteFont*& outFont, const SpriteFont::Glyph*& outGlyph, const SpriteFont& font_pref, wchar_t character) const;
		void prepareTextForRendering(const SpriteFont& font, std::wstring& text);
		void drawTextRawC(soup::SharedPtr<SpriteFont> font, const DirectX::SimpleMath::Vector2& base_pos, const wchar_t* text, size_t len, const float scale, const DirectX::SimpleMath::Color& _colour);
		void drawTextRawC(soup::SharedPtr<SpriteFont> font, const float x, const float y, const wchar_t* text, size_t len, const float scale, const DirectX::SimpleMath::Color& colour);
		void drawTextC(soup::SharedPtr<SpriteFont> font, const float x, const float y, std::wstring&& text, const float scale, const DirectX::SimpleMath::Color& colour);
		void drawTextH(const float x, const float y, std::wstring&& text, const float scale, const DirectX::SimpleMath::Color& colour);
		void drawTextC(float x, float y, std::wstring&& text, const Alignment alignment, const float scale, const DirectX::SimpleMath::Color& colour, soup::SharedPtr<SpriteFont> font, const bool force_in_bounds = true);
		void drawTextH(float x, float y, std::wstring&& text, const Alignment alignment, const float scale, const DirectX::SimpleMath::Color& colour, const bool force_in_bounds = true);
		
		void drawTextH(float x, float y, const wchar_t* text, const DirectX::SimpleMath::Color& colour, const TextSettings& settings);
		void drawTextHNoOffset(float x, float y, const wchar_t* text, const DirectX::SimpleMath::Color& colour, const TextSettings& settings);
		void drawTextH(float x, float y, const std::wstring& text, const DirectX::SimpleMath::Color& colour, const TextSettings& settings);
		void drawTextHNoOffset(float x, float y, const std::wstring& text, const DirectX::SimpleMath::Color& colour, const TextSettings& settings);
		void drawTextH(float x, float y, std::wstring&& text, const DirectX::SimpleMath::Color& colour, const TextSettings& settings, const Alignment alignment, const bool force_in_bounds = true);
		void drawTextHNoOffset(float x, float y, std::wstring&& text, const DirectX::SimpleMath::Color& colour, const TextSettings& settings, const Alignment alignment, const bool force_in_bounds = true);

		void drawTextSmallH(float x, float y, const wchar_t* text, const DirectX::SimpleMath::Color& colour);
		void drawTextSmallH(float x, float y, const std::wstring& text, const DirectX::SimpleMath::Color& colour);
		void drawTextSmallH(float x, float y, std::wstring&& text, const DirectX::SimpleMath::Color& colour, const Alignment alignment);

		[[nodiscard]] DirectX::SimpleMath::Vector2 getTextSize(const std::wstring& text, const float scale = 1.0f) const;
		[[nodiscard]] DirectX::SimpleMath::Vector2 getTextSize(soup::SharedPtr<SpriteFont> font, const std::wstring& text, const float scale = 1.0f) const;
		[[nodiscard]] DirectX::SimpleMath::Vector2 getTextSize(soup::SharedPtr<SpriteFont> font, const wchar_t* text, size_t len, const float scale) const;
		[[nodiscard]] float getTextWidth(const std::wstring& text, const float scale = 1.0f) const;
		[[nodiscard]] float getTextHeight(const std::wstring& text, const float scale = 1.0f) const;
		bool trimTextH(std::wstring& text, const float scale, float width) const;
		[[nodiscard]] std::wstring trimTextH(const std::string& text, const float scale, const float width) const;
		[[nodiscard]] std::wstring wrapTextH(const std::wstring& text, const float scale, float width) const;
	private:
		void wrapLineH(std::wstring& wstr, std::wstring& line, const float scale, const float width, const float space_width) const;
		void wrapWordH(std::wstring& wstr, const std::wstring& word, const float scale, const float width, const float space_width, float& line_width) const;

	public:
		void drawLineS(const float x1, const float y1, const DirectX::SimpleMath::Color& colour1, const float x2, const float y2, const DirectX::SimpleMath::Color& colour2);
		void drawLineS(const float x1, const float y1, const float x2, const float y2, const DirectX::SimpleMath::Color& colour);
		void drawLineCP(const float x1, const float y1, const float x2, const float y2, const DirectX::SimpleMath::Color& colour1, const DirectX::SimpleMath::Color& colour2);
		void drawLineCP(const float x1, const float y1, const float x2, const float y2, const DirectX::SimpleMath::Color& colour);
		void drawLineCP(const rage::Vector2& pos1, const rage::Vector2& pos2, const DirectX::SimpleMath::Color& colour);
		void drawLineH(const float x1, const float y1, const float x2, const float y2, const DirectX::SimpleMath::Color& colour);

		void drawTextThisTickC(float x, float y, std::wstring&& text, Alignment alignment, float scale, const DirectX::SimpleMath::Color& colour, const soup::SharedPtr<SpriteFont>& font, bool force_in_bounds = true, const v3& draw_origin_3d = {}, uint8_t drop_shadow = 0);
		void drawTextThisTickCP(float x, float y, std::wstring&& text, Alignment alignment, float scale, const DirectX::SimpleMath::Color& colour, const soup::SharedPtr<SpriteFont>& font, bool force_in_bounds = true, const v3& draw_origin_3d = {}, uint8_t drop_shadow = 0);
		void drawTextThisTickH(float x, float y, std::wstring&& text, Alignment alignment, float scale, const DirectX::SimpleMath::Color& colour, const soup::SharedPtr<SpriteFont>& font, bool force_in_bounds = true, const v3& draw_origin_3d = {}, uint8_t drop_shadow = 0);
		void drawTextThisTickHP(float x, float y, std::wstring&& text, Alignment alignment, float scale, const DirectX::SimpleMath::Color& colour, const soup::SharedPtr<SpriteFont>& font, bool force_in_bounds = true, const v3& draw_origin_3d = {}, uint8_t drop_shadow = 0);
		void drawCentredTextThisTick(const std::string& text);
		void drawCentredTextThisTick(std::wstring text);
		void drawUnscaled3dTextThisTickH(const v3& pos, std::wstring&& text, const float scale, DirectX::SimpleMath::Color& colour);
		void drawUnscaled3dTextThisTickH(const v3& pos, const std::wstring& text, const float scale, DirectX::SimpleMath::Color& colour);
		void draw3dTextThisTickH(const v3& pos, std::wstring&& text, float scale);
		void draw3dTextThisTickH(const v3& pos, const std::wstring& text, const float scale);
		void draw3dTextThisTickH(const v3& pos, std::wstring&& text, float scale, const DirectX::SimpleMath::Color& colour);
		void drawLineThisTickCP(const float x1, const float y1, const float x2, const float y2, const DirectX::SimpleMath::Color& colour1, const DirectX::SimpleMath::Color& colour2);
		void drawLineThisTickCP(const float x1, const float y1, const float x2, const float y2, const DirectX::SimpleMath::Color& colour);
		void drawLineThisTickCP(const rage::Vector2& pos1, const rage::Vector2& pos2, const DirectX::SimpleMath::Color& colour);
		void drawLineThisTickHP(const float x1, const float y1, const float x2, const float y2, const DirectX::SimpleMath::Color& colour1, const DirectX::SimpleMath::Color& colour2);

		void drawBgBlurThisTickC(soup::WeakRef<EphemeralBackgroundBlur>&& wr, float x, float y, float width, float height, uint8_t passes);

#if DRAW_HTML
		void drawHtmlThisTickH(int x, int y, unsigned int width, unsigned int height, std::string&& html);
#endif

		void drawCentredTextThisFrame(const std::wstring& text);

		bool drawing = false;
		void draw();
		void drawCentredText();
		[[nodiscard]] DirectX::SimpleMath::Vector2 getCursorPosH(bool allow_click_drag) const;
		[[nodiscard]] std::wstring getAddressText(Command* const command, const float width) const;

	public:
		unsigned int m_unreleased_menu_key = 0;
		std::vector<Hotkey> unreleased_hold_mode_hotkeys = {};
		bool suppress_numpad_7_messages = false;
		std::set<KeyboardHandler> keyboard_handlers = {};
		void stop_input();
		std::vector<Hotkey> tab_complete_keys = {};
		std::vector<Hotkey> paste_keys = {};
		int32_t chat_message_length_when_paste_key_was_first_down = -1;
		bool wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		unsigned int unreleased_hotkey_candiate = 0;
	private:
		bool keyUpdown(unsigned int vk, bool is_down, bool repeat);
		void keyPress(const unsigned int vk);
		void keyDown(Hotkey pressing);
		bool keyUp(Hotkey pressing);
		void releaseAllHoldModeHotkeys();
	public:
		static bool isModifierKey(const unsigned int vk);

		std::vector<void(*)()> wndproc_jobs{};
		void queueWndProcJob(void f());
		void ensureWndProcContext(void f());
	};

	inline Renderer g_renderer{};
}
