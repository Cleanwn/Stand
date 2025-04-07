#pragma once

#include "CommandList.hpp"

#include "ExecCtx.hpp"

namespace Stand
{
#pragma pack(push, 1)
	template <typename T>
	class CommandListWithOnTickAsActiveList : public T
	{
	protected:
		bool is_active = false;

		using T::T;

	public:
		void onActiveListUpdate() override
		{
			if (((CommandListWithOnTickAsActiveList<T>*)this)->isCurrentUiOrWebList())
			{
				if (!is_active)
				{
					is_active = true;
					auto wr = ((CommandListWithOnTickAsActiveList<T>*)this)->getWeakRef();
					((CommandListWithOnTickAsActiveList<T>*)this)->registerScriptTickEventHandler([wr]
					{
						SOUP_IF_LIKELY (auto ptr = wr.getPointer())
						{
							// It is possible that onTickAsActiveList invokes inputBack, focuses another list, and that list now wants to register a script tick event handler.
							// For that case, we signal that the lock can not be obtained right now.
							ExecCtx::get().script_tick_event_mtx_is_locked = true;
							ptr->as<CommandListWithOnTickAsActiveList<T>>()->onTickAsActiveList();
							ExecCtx::get().script_tick_event_mtx_is_locked = false;
							return ptr->as<CommandListWithOnTickAsActiveList<T>>()->is_active;
						}
						return false;
					});
				}
			}
			else
			{
				is_active = false;
			}
		}

	protected:
		virtual void onTickAsActiveList() = 0;		
	};
#pragma pack(pop)
}
