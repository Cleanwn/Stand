#pragma once

#include "CommandSlider.hpp"
#include "CommandStatBase.hpp"

namespace Stand
{
	template <class T = CommandSlider>
	class CommandStatSlider : public CommandStatBase<T, int>
	{
	public:
		using CommandStatBase<T, int>::CommandStatBase;

		void onChange(Click& click, int prev_value) final
		{
			this->ensureScriptThread(click, [this]
			{
				this->setStat(this->getStatHash());
				this->onUpdate();
			});
		}
	};
}