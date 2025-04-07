#pragma once

#include "TextboxInterface.hpp"

namespace Stand
{
	class TabCompletionProvider
	{
	private:
		const TextboxInterface& tbif;
		size_t message_length_when_key_was_first_down = -1;
		std::wstring last_draft;
		std::vector<std::wstring> tab_completions;
		size_t tab_complete_index = 0;

	public:
		TabCompletionProvider(const TextboxInterface& tbif)
			: tbif(tbif)
		{
		}

		[[nodiscard]] bool canHandleKeyUp() const noexcept;

		void onKeyDown();
		void onKeyUp();

		[[nodiscard]] virtual std::vector<std::wstring> getCompletions(std::wstring&& prefix) = 0;
	};
}

namespace Stand
{
	struct PlayerNameTabCompletionProvider : public TabCompletionProvider
	{
		PlayerNameTabCompletionProvider()
			: TabCompletionProvider(g_chatbox_interface)
		{
		}

		[[nodiscard]] std::vector<std::wstring> getCompletions(std::wstring&& prefix) final;
	};
	inline PlayerNameTabCompletionProvider g_player_name_tab_completion_provider{};
}

namespace Stand
{
	struct CommandNameTabCompletionProvider : public TabCompletionProvider
	{
		CommandNameTabCompletionProvider()
			: TabCompletionProvider(g_commandbox_interface)
		{
		}

		[[nodiscard]] std::vector<std::wstring> getCompletions(std::wstring&& prefix) final;
	};
	inline CommandNameTabCompletionProvider g_command_name_tab_completion_provider{};
}
