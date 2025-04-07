#include "ChatHistory.hpp"

#include "atArray.hpp"
#include "Chat.hpp"
#include "CScaleformStore.hpp"
#include "pointers.hpp"
#include "scaleform_gfx.hpp"
#include "ScaleformMovieStruct.hpp"

namespace Stand
{
	[[nodiscard]] static const ScaleformMovieStruct* findScaleformByName(const char* name)
	{
		for (const auto& scaleform : *pointers::scaleform_array)
		{
			//if (*scaleform.cFilename)
			if (strcmp(scaleform.cFilename, name) == 0)
			{
				return &scaleform;
			}
		}
		return nullptr;
	}

	static void addScaleformMessageToHistory(const GFxValue& entry)
	{
		GFxValue player, message, scope/*, hudColour*/;
		entry.GetMember("player", &player);
		entry.GetMember("message", &message);
		entry.GetMember("scope", &scope);
		//entry.GetMember("hudColour", &hudColour);

		std::string contents = message.GetString();
		const bool is_auto = Chat::isMarkedAsAuto(contents);
		if (is_auto)
		{
			Chat::removeAutoMark(contents);
		}

		ChatHistory::history.emplace_back(ChatHistory::Message{
			player.GetString(),
			std::move(contents),
			(strcmp(scope.GetString(), HUD::GET_FILENAME_FOR_AUDIO_CONVERSATION("MP_CHAT_ALL")) != 0),
			is_auto,
			0
		});
	}

	void ChatHistory::fromScaleform()
	{
		//ChatHistory::history.clear();

		if (auto scaleform = findScaleformByName("MULTIPLAYER_CHAT"))
		{
			if (auto view = pointers::scaleform_store->GetMovieView(scaleform->iMovieId))
			{
				GFxValue historyIndex;
				view->GetVariable(&historyIndex, "TIMELINE.historyIndex");

				GFxValue history[20];
				view->GetVariableArray("TIMELINE.history", 0, history, COUNT(history));

				auto i = (int)historyIndex.Value.NValue;
				for (int c = 0; c != COUNT(history); ++c)
				{
					if (history[i].GetType() == GFxValue::VT_Object)
					{
						addScaleformMessageToHistory(history[i]);
					}
					++i;
					i %= COUNT(history);
				}
			}
		}
	}
}
