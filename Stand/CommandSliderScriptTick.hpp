#pragma once

namespace Stand
{
	template <class T>
	class CommandSliderScriptTick : public T
	{
	protected:
		using T::T;

		void onChange(Click& click, int prev_value) final
		{
			auto dis = reinterpret_cast<CommandSliderScriptTick<T>*>(this);
			auto value = dis->value;
			if (value == dis->default_value)
			{
				dis->ensureScriptThread(click, [dis]
				{
					dis->onScriptTick();
				});
				return;
			}
			dis->registerScriptTickEventHandler(click, [dis, value]
			{
				if (value == dis->value)
				{
					dis->onScriptTick();
					return true;
				}
				return false;
			});
		}

		virtual void onScriptTick() = 0;
	};
}
