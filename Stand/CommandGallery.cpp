#include "CommandGallery.hpp"

#include "CommandAction.hpp"
#include "CommandListAction.hpp"
#include "CommandListSelect.hpp"
#include "CommandSlider.hpp"
#include "CommandSliderFloat.hpp"
#include "CommandTextsliderStateful.hpp"
#include "CommandToggle.hpp"
#include "CommandToggleNoCorrelation.hpp"

namespace Stand
{
	template <typename T>
	class CommandWithNopOnClick : public T
	{
	public:
		using T::T;

		void onClick(Click&) final
		{
		}
	};

	template <typename T>
	class CommandWithNopOnWebClick : public T
	{
	public:
		using T::T;

		void onClick(Click& click) final
		{
			if (!click.isWeb())
			{
				T::onClick(click);
			}
		}
	};

	CommandGallery::CommandGallery(CommandList* const parent)
		: CommandList(parent, LIT("Command Gallery"))
	{
		createChild<CommandList>(LIT("menu.list"), CMDNAMES("gallerylist"));
		createChild<CommandAction>(LIT("menu.action"), CMDNAMES("galleryaction"));
		createChild<CommandToggle>(LIT("menu.toggle"), CMDNAMES("gallerytoggle"));
		//createChild<CommandToggleNoCorrelation>(LIT("CommandToggleNoCorrelation"), CMDNAMES("gallerytogglenocorrelation"));
		createChild<CommandWithNopOnWebClick<CommandSlider>>(LIT("menu.slider"), CMDNAMES("galleryslider"), NOLABEL, INT_MIN, INT_MAX, 0);
		createChild<CommandWithNopOnWebClick<CommandSliderFloat>>(LIT("menu.slider_float"), CMDNAMES("gallerysliderfloat"), NOLABEL, INT_MIN, INT_MAX, 0);
		createChild<CommandWithNopOnClick<CommandSlider>>(LIT("menu.click_slider"), CMDNAMES("galleryclickslider"), NOLABEL, INT_MIN, INT_MAX, 0, 1, CMDFLAGS_SLIDER, COMMANDPERM_USERONLY, true);
		createChild<CommandWithNopOnClick<CommandSliderFloat>>(LIT("menu.click_slider_float"), CMDNAMES("galleryclicksliderfloat"), NOLABEL, INT_MIN, INT_MAX, 0, 1, CMDFLAGS_SLIDER, COMMANDPERM_USERONLY, true);
		createChild<CommandListSelect>(LIT("menu.list_select"), CMDNAMES("gallerylistselect"), NOLABEL, std::vector<CommandListActionItemData>{
			{ 1, LIT("Uno"), CMDNAMES("uno") },
			{ 2, LIT("Dos"), CMDNAMES("dos") },
			{ 3, LIT("Tres"), CMDNAMES("tres") },
		}, 1);
		createChild<CommandListAction>(LIT("menu.list_action"), CMDNAMES("gallerylistaction"), NOLABEL, std::vector<CommandListActionItemData>{
			{ 1, LIT("Uno"), CMDNAMES("uno") },
			{ 2, LIT("Dos"), CMDNAMES("dos") },
			{ 3, LIT("Tres"), CMDNAMES("tres") },
		});
		createChild<CommandWithNopOnClick<CommandTextslider>>(LIT("menu.textslider"), {}, NOLABEL, std::vector<CommandTextsliderData>{
			{ 1, LIT("Uno") },
			{ 2, LIT("Dos") },
			{ 3, LIT("Tres") },
		});
		createChild<CommandWithNopOnClick<CommandTextsliderStateful>>(LIT("menu.textslider_stateful"), {}, NOLABEL, std::vector<CommandTextsliderData>{
			{ 1, LIT("Uno") },
			{ 2, LIT("Dos") },
			{ 3, LIT("Tres") },
		});
	}
}
