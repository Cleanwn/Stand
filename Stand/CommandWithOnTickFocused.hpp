#pragma once

#include "Direction.hpp"
#include "ThreadContext.hpp"

namespace Stand
{
#pragma pack(push, 1)
	template <class T>
	class CommandWithOnTickFocused : public T
	{
	protected:
		bool focused = false;

		using T::T;

	public:
		void onFocus(ThreadContext thread_context, Direction momentum) override
		{
			T::onFocus(thread_context, momentum);

			focused = true;
		}

		void onBlur(ThreadContext thread_context, Direction momentum) override
		{
			T::onBlur(thread_context, momentum);

			focused = false;
		}

		void onTickInGameViewport() override
		{
			if (focused)
			{
				onTickFocused();
			}

			return T::onTickInGameViewport();
		}

	protected:
		virtual void onTickFocused() = 0;
	};
#pragma pack(pop)
}
