#include "CommandPlayerbar.hpp"

#include "CommandColourPointerDirectx.hpp"
#include "CommandDivider.hpp"
#include "CommandLambdaSlider.hpp"
#include "CommandListSelect.hpp"
#include "CommandSlider.hpp"
#include "CommandTogglePointer.hpp"

#include "CommandPlayerbarMain.hpp"

namespace Stand
{
	CommandPlayerbar::CommandPlayerbar(CommandList* const parent)
		: CommandList(parent, LOC("PBAR"))
	{
		for (auto& colour : tag_colours)
		{
			colour = DirectX::SimpleMath::Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		}
		setDefaultTagColour(FLAG_SELF, { 1.0f, 0.0f, 1.0f, 1.0f });
		setDefaultTagColour(FLAG_MOD, { 1.0f, 0.0f, 0.0f, 1.0f });
#if HAS_MODAMN
		setDefaultTagColour(FLAG_MODAMN, { 1.0f, 0.0f, 0.0f, 1.0f });
#endif
		setDefaultTagColour(FLAG_AMN, { 1.0f, 0.0f, 0.0f, 1.0f });
		setDefaultTagColour(FLAG_HOST, { 0.0f, 0.0f, 1.0f, 1.0f });
		setDefaultTagColour(FLAG_GOD, { 1.0f, 0.25f, 0.0f, 1.0f });
		setDefaultTagColour(FLAG_INT, { 1.0f, 0.83f, 0.0f, 1.0f });
		setDefaultTagColour(FLAG_TYPING, { 0.66f, 0.66f, 0.66f, 1.0f });
		setDefaultTagColour(FLAG_DEAD, { 0.0f, 0.0f, 0.0f, 1.0f });

		this->createChild<CommandPlayerbarMain>();
		this->createChild<CommandLambdaSlider<CommandSlider>>(LOC("TXTSCL"), CMDNAMES("playerbarscale"), NOLABEL, 1, SHRT_MAX, 12, 1, [this](int value, Click& click)
		{
			this->text_scale = float(value) * float(TEXT_HEIGHT_PX) * 2.0f;
		});
		this->text_scale = (12.0f * float(TEXT_HEIGHT_PX) * 2.0f);
		this->bgblur = this->createChild<CommandSlider>(LOC("BGBLR"), CMDNAMES("playerbarblur", "playerbarbgblur", "playerbarbackgroundblur"), LOC("BGBLR_H"), 0, 100, 0, 1);
		this->hide_if = this->createChild<CommandListSelect>(LOC("HDEIF"), {}, NOLABEL, std::vector<CommandListActionItemData>{
			{ 0, LOC("DOFF") },
			{ 1, LOC("PBAR_HI_1") },
			{ 2, LOC("PBAR_HI_2") },
		}, 1);
		this->show_if_alone = this->createChild<CommandToggle>(LOC("PBAR_ALNE"), { CMDNAME("playerbaralone") }, NOLABEL, true);
		this->show_tags = this->createChild<CommandToggle>(LOC("SHWTGS"), { CMDNAME("playerbartags") }, NOLABEL, true);
		this->hide_no_tags = this->createChild<CommandToggle>(LOC("HDEPNTG"), { CMDNAME("playerbarhidenotags") });
		this->createChild<CommandColourPointerDirectx>(&tag_colours[FLAG_COUNT], LOC("DFLTCLR"), CMDNAMES("playerbardefault"));
		this->createChild<CommandDivider>(LOC("TGCLRS"));
#define CREATE_LIST(id, name) createTagColourList(id, LOC(name));
		FOR_ALL_FLAGS(CREATE_LIST);
	}

	void CommandPlayerbar::setDefaultTagColour(playerflag_t id, DirectX::SimpleMath::Color&& colour)
	{
		tag_provides_colour[id] = true;
		tag_colours[id] = std::move(colour);
	}

	void CommandPlayerbar::createTagColourList(playerflag_t id, Label&& name)
	{
		auto command_names = combineCommandNames(CMDNAME("playerbar"), getCommandNamesFromMenuName(name));

		auto list = this->createChild<CommandList>(std::move(name));

		list->createChild<CommandColourPointerDirectx>(&tag_colours[id], LOC("CLR"), std::move(command_names));
		list->createChild<CommandTogglePointer>(&tag_provides_colour[id], LOC("UCLR"));
	}
}
