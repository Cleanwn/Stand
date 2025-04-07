#include "CommandColourCustom.hpp"

#include <soup/Rgb.hpp>

#include "Colour.hpp"
#include "ColourUtil.hpp"
#include "CommandColourHex.hpp"
#include "CommandColourSlider.hpp"
#include "CommandCurrentCustomColourHex.hpp"
#include "CommandDivider.hpp"
#include "CommandLambdaAction.hpp"
#include "get_next_arg.hpp"
#include "Gui.hpp"
#include "MenuGrid.hpp"
#include "Renderer.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandColourCustom::CommandColourCustom(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, int default_r, int default_g, int default_b, int default_a, commandflags_t flags)
		: CommandList(parent, std::move(menu_name), std::move(command_names), std::move(help_text), flags, COMMAND_LIST_COLOUR)
	{
		h = new CommandColourSlider(this, LOC("C_H"), getSuffixedCommandNames(CMDNAME("hue")), CommandColourSlider::HSV, 0, 359, 0);
		s = new CommandColourSlider(this, LOC("C_S"), getSuffixedCommandNames(CMDNAME("saturation")), CommandColourSlider::HSV, 0, 100, 0);
		v = new CommandColourSlider(this, LOC("C_V"), getSuffixedCommandNames(CMDNAME("value")), CommandColourSlider::HSV, 0, 100, 0);
		if (default_a != -1)
		{
			a = new CommandColourSlider(this, LOC("OPCTY"), getSuffixedCommandNames(CMDNAME("opacity")), CommandColourSlider::OPACITY, 0, 255, default_a);
		}
		r = new CommandColourSlider(this, LOC("C_R"), getSuffixedCommandNames(CMDNAME("red")), CommandColourSlider::RGB, 0, 255, default_r);
		g = new CommandColourSlider(this, LOC("C_G"), getSuffixedCommandNames(CMDNAME("green")), CommandColourSlider::RGB, 0, 255, default_g);
		b = new CommandColourSlider(this, LOC("C_B"), getSuffixedCommandNames(CMDNAME("blue")), CommandColourSlider::RGB, 0, 255, default_b);
	}

	CommandColourCustom::CommandColourCustom(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const DirectX::SimpleMath::Color& default_rgba, bool transparency, commandflags_t flags)
		: CommandColourCustom(parent, std::move(menu_name), std::move(command_names), std::move(help_text), (int)(default_rgba.x * 255.0f), (int)(default_rgba.y * 255.0f), (int)(default_rgba.z * 255.0f), transparency ? (int)(default_rgba.w * 255.0f) : -1, flags)
	{
	}

	void CommandColourCustom::populate()
	{
		createChild<CommandDivider>(LOC("CR_HSV"));
		this->children.emplace_back(h);
		this->children.emplace_back(s);
		this->children.emplace_back(v);
		if (a)
		{
			createChild<CommandDivider>(LOC("OPCTY"));
			this->children.emplace_back(a);
		}
		createChild<CommandDivider>(LOC("CR_RGB"));
		this->children.emplace_back(r);
		this->children.emplace_back(g);
		this->children.emplace_back(b);
		updateHSVRaw(); // don't want `processVisibilityUpdate` while populating because it's somewhat expensive
		default_value = getRGBAInt();
		createChild<CommandDivider>(LOC("OTHR"));
		SOUP_IF_LIKELY (!command_names.empty())
		{
			createChild<CommandColourHex>();
		}
		createChild<CommandCurrentCustomColourHex>(this);
	}

	void CommandColourCustom::populateWithOptionalCopyFrom(CommandColourCustom* const copy_from)
	{
		populate();
		if (copy_from != nullptr)
		{
			createCopyFrom(copy_from);
		}
	}

	void CommandColourCustom::populate(CommandColourCustom* const copy_from)
	{
		populate();
		createCopyFrom(copy_from);
	}

	void CommandColourCustom::createCopyFrom(CommandColourCustom* const copy_from)
	{
		createChild<CommandLambdaAction>(LIT(LANG_FMT("CPYF", copy_from->menu_name.getLocalisedUtf8())), {}, NOLABEL, [this, copy_from](Click& click)
		{
			this->set(click, copy_from->getRGBA());
		});
	}

	void CommandColourCustom::onChange(Click& click)
	{
	}

	void CommandColourCustom::updateRGB()
	{
		const DirectX::SimpleMath::Vector3 rgb = ColourUtil::hsv_to_rgb(getHSV());
		Click click(CLICK_BULK, TC_OTHER);
		this->r->setValue(click, (int)(rgb.x * 255.0f));
		this->g->setValue(click, (int)(rgb.y * 255.0f));
		this->b->setValue(click, (int)(rgb.z * 255.0f));
	}

	void CommandColourCustom::updateHSV()
	{
		Click click(CLICK_BULK, TC_OTHER);
		updateHSV(click);
	}

	void CommandColourCustom::updateHSV(Click& click)
	{
		const DirectX::SimpleMath::Vector3 hsv = ColourUtil::rgb_to_hsv(getRGB());
		this->h->setValue(click, (int)hsv.x);
		this->s->setValue(click, (int)(hsv.y * 100.0f));
		this->v->setValue(click, (int)(hsv.z * 100.0f));
	}

	void CommandColourCustom::updateHSVRaw()
	{
		const DirectX::SimpleMath::Vector3 hsv = ColourUtil::rgb_to_hsv(getRGB());
		this->h->value = (int)hsv.x;
		this->s->value = (int)(hsv.y * 100.0f);
		this->v->value = (int)(hsv.z * 100.0f);
	}

	void CommandColourCustom::getRGB(Colour& colour) const
	{
		colour.r = this->r->value;
		colour.g = this->g->value;
		colour.b = this->b->value;
	}

	void CommandColourCustom::getRGB(int& r, int& g, int& b) const
	{
		r = this->r->value;
		g = this->g->value;
		b = this->b->value;
	}

	DirectX::SimpleMath::Vector3 CommandColourCustom::getRGB() const
	{
		return {
			(float)this->r->value / 255.0f,
			(float)this->g->value / 255.0f,
			(float)this->b->value / 255.0f
		};
	}

	DirectX::SimpleMath::Vector3 CommandColourCustom::getHSV() const
	{
		return {
			(float)this->h->value,
			(float)this->s->value / 100.0f,
			(float)this->v->value / 100.0f
		};
	}

	float CommandColourCustom::getAlpha() const
	{
		return (this->a == nullptr ? 1.0f : float(this->a->value) / 255.0f);
	}

	DirectX::SimpleMath::Color CommandColourCustom::getRGBA() const
	{
		return {
			(float)this->r->value / 255.0f,
			(float)this->g->value / 255.0f,
			(float)this->b->value / 255.0f,
			getAlpha()
		};
	}

	uint32_t CommandColourCustom::getRGBAInt() const
	{
		uint32_t rgba = this->r->value;
		rgba <<= 8;
		rgba |= this->g->value;
		rgba <<= 8;
		rgba |= this->b->value;
		rgba <<= 8;
		if (this->a)
		{
			rgba |= this->a->value;
		}
		return rgba;
	}

	std::string CommandColourCustom::getHex() const
	{
		return Util::to_padded_hex_string(getRGBAInt());
	}

	void CommandColourCustom::set(Click& click, uint32_t rgba)
	{
		Click click_ = click.derive(CLICK_BULK);
		if (this->a != nullptr)
		{
			this->a->setValue(click_, rgba & 0xFF);
		}
		rgba >>= 8;
		this->b->setValue(click_, rgba & 0xFF);
		rgba >>= 8;
		this->g->setValue(click_, rgba & 0xFF);
		rgba >>= 8;
		this->r->setValue(click_, rgba/* & 0xFF*/);
		updateHSV(click_);
		processChange(click);
	}

	void CommandColourCustom::set(Click& click, int r, int g, int b)
	{
		this->r->setValue(click, r);
		this->g->setValue(click, g);
		this->b->setValue(click, b);
		updateHSV();
		updateState(click.type);
	}

	void CommandColourCustom::set(Click& click, const DirectX::SimpleMath::Color& rgba)
	{
		Click click_ = click.derive(CLICK_BULK);
		this->r->setValue(click_, (int)(rgba.R() * 255.0f));
		this->g->setValue(click_, (int)(rgba.G() * 255.0f));
		this->b->setValue(click_, (int)(rgba.B() * 255.0f));
		if (this->a != nullptr)
		{
			this->a->setValue(click_, (int)(rgba.A() * 255.0f));
		}
		updateHSV();
		processChange(click);
	}

	void CommandColourCustom::processChange(Click& click)
	{
		updateState(click.type);
		onChange(click);
	}

	void CommandColourCustom::updateState(ClickType click_type)
	{
		if (click_type != CLICK_BULK && supportsStateOperations())
		{
			g_gui.on_command_state_change();
		}
		if (g_renderer.preview_colour_in_list)
		{
			processVisualUpdate(true);
		}
	}

	void CommandColourCustom::onCommand(Click& click, std::wstring& args)
	{
		auto arg = get_next_arg(args);
		if (arg.empty())
		{
			CommandList::onClick(click);
		}
		else
		{
			try
			{
				setState(click, StringUtils::utf16_to_utf8(arg));
			}
			catch (const std::exception&)
			{
				click.setResponse(LOC("INVARG"));
				std::wstring prefill{ cmdNameToUtf16(command_names.at(0)) };
				prefill.push_back(L' ');
				prefill.append(arg);
				click.showCommandBoxIfPossible(std::move(prefill));
			}
		}
	}

	std::string CommandColourCustom::getState() const
	{
		return Util::to_padded_hex_string(getRGBAInt());
	}

	std::string CommandColourCustom::getDefaultState() const
	{
		return Util::to_padded_hex_string(default_value);
	}

	void CommandColourCustom::setState(Click& click, const std::string& _state)
	{
		if (!_state.empty())
		{
			std::string state(_state);
			soup::Rgb::expandHex(state);
			static_assert(sizeof(unsigned long) == sizeof(uint32_t));
			uint32_t rgba = std::stoul(state, nullptr, 16);
			if (6 >= state.size())
			{
				rgba <<= 8;
				rgba |= 0xFF;
			}
			set(click, rgba);
		}
		else
		{
			// If we have stateful children, our state entry may exist with an empty value, so we just have to deal with this.
			applyDefaultState();
		}
	}

	void CommandColourCustom::applyDefaultState()
	{
		Click click(CLICK_BULK, TC_APPLYDEFAULTSTATE);
		set(click, default_value);
	}

	std::vector<CommandName> CommandColourCustom::getRainbowCommandNames() const
	{
		std::vector<CommandName> res{};
		if (!command_names.empty())
		{
			res.emplace_back(std::move(CommandName(command_names.at(0)).append(CMDNAME("rainbow"))));
		}
		return res;
	}
}
