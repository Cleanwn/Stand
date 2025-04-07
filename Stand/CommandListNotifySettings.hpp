#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListNotifySettings : public CommandList
	{
	public:
		CommandList* timing;
		bool is_active = false;

		explicit CommandListNotifySettings(CommandList* parent, CommandColourCustom* primaryColour, CommandColourCustom* bgColour);

		void onActiveListUpdate() final;

	private:
		static void showPersistentToast();
		static void hidePersistentToast();
	public:
		void setToaster(ThreadContext thread_context, Toaster* toaster);
		void updateGameColour(ThreadContext thread_context);
	};
}
