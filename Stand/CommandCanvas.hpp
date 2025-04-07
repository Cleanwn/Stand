#pragma once

#include "CommandListSelect.hpp"

#include <soup/UniquePtr.hpp>

#include "FeatureCanvas.hpp"

namespace Stand
{
	class CommandCanvas : public CommandListSelect
	{
	private:
		soup::UniquePtr<FeatureCanvas> canvas_window;
		soup::UniquePtr<FeatureCanvas> canvas_direct;
		
	public:
		explicit CommandCanvas(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL);

		void onChange(Click& click, long long prev_value) final;

	protected:
		void init(FeatureCanvas& canvas);
		static void drawFunc(void* userdata, soup::RenderTarget& rt);
		virtual void draw(soup::RenderTarget& rt) = 0;
	};
}
