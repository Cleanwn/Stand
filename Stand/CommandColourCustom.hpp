#pragma once

#include "CommandList.hpp"

#include "dx_common.hpp"

namespace Stand
{
	class CommandColourCustom : public CommandList
	{
	private:
		uint32_t default_value;
	public:
		CommandSlider* r;
		CommandSlider* g;
		CommandSlider* b;
		CommandSlider* h;
		CommandSlider* s;
		CommandSlider* v;
		CommandSlider* a = nullptr;
		bool rainbow_active = false;

		explicit CommandColourCustom(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, int default_r, int default_g, int default_b, int default_a = -1, commandflags_t flags = CMDFLAGS_COLOUR_CUSTOM);
		explicit CommandColourCustom(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const DirectX::SimpleMath::Color& default_rgba, bool transparency = true, commandflags_t flags = CMDFLAGS_COLOUR_CUSTOM);

		void populate();
		void populateWithOptionalCopyFrom(CommandColourCustom* const copy_from);
		void populate(CommandColourCustom* const copy_from);
	private:
		void createCopyFrom(CommandColourCustom* const copy_from);

	public:
		virtual void onChange(Click& click);

		void updateRGB();
		void updateHSV();
		void updateHSV(Click& click);
	private:
		void updateHSVRaw();

	public:
		void getRGB(Colour& colour) const;
		void getRGB(int& r, int& g, int& b) const;
		[[nodiscard]] DirectX::SimpleMath::Vector3 getRGB() const;
		[[nodiscard]] DirectX::SimpleMath::Vector3 getHSV() const;
		[[nodiscard]] float getAlpha() const;
		[[nodiscard]] DirectX::SimpleMath::Color getRGBA() const;
		[[nodiscard]] uint32_t getRGBAInt() const;
		[[nodiscard]] std::string getHex() const;

		void set(Click& click, uint32_t rgba);
		void set(Click& click, int r, int g, int b);
		void set(Click& click, const DirectX::SimpleMath::Color& rgba);
		void processChange(Click& click);
		void updateState(ClickType click_type);

		void onCommand(Click& click, std::wstring& args) final;
		std::string getState() const final;
		std::string getDefaultState() const final;
		void setState(Click& click, const std::string& _state) final;
		void applyDefaultState() final;

		[[nodiscard]] std::vector<CommandName> getRainbowCommandNames() const;
	};
}
