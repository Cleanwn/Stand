#pragma once

#include "CommandAction.hpp"

#include <fstream>

#include "CommandListConcealer.hpp"
#include "CommandPlayer.hpp"
#include "Gui.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class CommandFeatureList : public CommandAction
	{
	public:
		explicit CommandFeatureList(CommandList* const parent)
			: CommandAction(parent, LOC("FTRLST"), { CMDNAME("featurelist") }, LOC("FTRLST_H"))
		{
		}

		void onClick(Click& click) final
		{
			std::ofstream file_out;
			file_out.open(std::wstring(_wgetenv(L"appdata")).append(L"\\Stand\\Features (").append(StringUtils::utf8_to_utf16(Lang::idToApiCode(Lang::active_id))).append(L").txt"), std::ios_base::out);
			recursivelyWriteFeatureList(file_out, "", g_gui.root_list.get());
			click.setResponse(LOC("FTRLST_T"));
		}

	private:
		void recursivelyWriteFeatureList(std::ofstream& file_out, std::string prefix, CommandList* const list, const std::string& censor_command_player_name = {})
		{
			cursor_t command_i = 0;
			cursor_t player_features = 0;
			bool first_item = true;
			list->forEachChildAsPhysical([&](CommandPhysical* command)
			{
				if ((command->flags & CMDFLAG_FEATURELIST_MASK) == CMDFLAG_FEATURELIST_FINISHLIST)
				{
					file_out << prefix;
					file_out << "<";
					const auto count = (cursor_t)list->children.size() - command_i;
					if (!first_item)
					{
						file_out << LANG_FMT("FTRLST_MOPTNS", count);
					}
					else if (count == 1)
					{
						file_out << LANG_GET("FTRLST_1OPTN");
					}
					else
					{
						file_out << LANG_FMT("FTRLST_OPTNS", count);
					}
					file_out << ">\n";
					CONSUMER_BREAK;
				}
				command_i++;
				if ((command->flags & CMDFLAG_FEATURELIST_MASK) == CMDFLAG_FEATURELIST_SKIP)
				{
					CONSUMER_CONTINUE;
				}
				first_item = false;
				if (command->type == COMMAND_LIST_PLAYER)
				{
					if (player_features < 2)
					{
						file_out << prefix;
						std::string censor_command_player_name{};
						if (player_features == 0)
						{
							file_out << command->menu_name.getLocalisedUtf8(); // All Players
						}
						else
						{
							file_out << LANG_GET("PLYFTRLST"); // Single Player
							file_out << (const char*)u8" — ";
							file_out << LANG_GET("CMD");
							file_out << ": ";
							std::string syntax = LANG_GET("PLYFTRLST");
							StringUtils::to_lower(syntax);
							file_out << syntax;

							censor_command_player_name = cmdNameToUtf8(command->as<CommandPlayer>()->getCommandNamesSuffix());
						}
						file_out << '\n';
						std::string prefix_{ prefix };
						prefix_.push_back('\t');
						recursivelyWriteFeatureList(file_out, std::move(prefix_), (CommandList*)command, censor_command_player_name);
						player_features++;
					}
					CONSUMER_CONTINUE;
				}
				file_out << prefix;
				file_out << command->menu_name.getLocalisedUtf8();
				if (command->isToggle())
				{
					if (command->command_names.empty())
					{
						file_out << " (";
						file_out << LANG_GET("JUSTON");
						file_out << "/";
						file_out << LANG_GET("OFF");
						file_out << ")";
					}
				}
				else if (command->isSlider())
				{
					if (command->canShowSliderRangeInFeatureList())
					{
						file_out << " (";
						auto slider = (CommandSlider*)command;
						file_out << StringUtils::utf16_to_utf8(slider->formatNumber(slider->min_value, false));
						file_out << ' ';
						file_out << LANG_GET("ARGRNGE");
						file_out << ' ';
						file_out << StringUtils::utf16_to_utf8(slider->formatNumber(slider->max_value, false));
						file_out << ')';
					}
				}
				std::string extra_text = command->getFeatureListExtraText();
				if (!extra_text.empty())
				{
					if (!censor_command_player_name.empty())
					{
						std::string censor_text = LANG_GET("PLYFTRLST");
						StringUtils::to_lower(censor_text);
						StringUtils::replace_all(extra_text, censor_command_player_name, censor_text);
					}
					file_out << extra_text;
				}
				if (command->canBeUsedByOtherPlayers() && !command->command_names.empty())
				{
					file_out << (const char*)u8" — ";
					file_out << LANG_FMT("FTRLST_OTHP", getCommandPermLabel(command->perm).getLocalisedUtf8());
				}
				/*if ((command->flags & CMDFLAG_FEATURELIST_MASK) == CMDFLAG_FEATURELIST_BASIC)
				{
					file_out << (const char*)u8" — ";
					file_out << LANG_GET("FTRLST_B");
				}
				else*/ if ((command->flags & CMDFLAG_FEATURELIST_MASK) == CMDFLAG_FEATURELIST_REGULAR)
				{
					file_out << (const char*)u8" — ";
					file_out << LANG_GET("FTRLST_R");
				}
				else if ((command->flags & CMDFLAG_FEATURELIST_MASK) == CMDFLAG_FEATURELIST_ULTIMATE)
				{
					file_out << (const char*)u8" — ";
					file_out << LANG_GET("FTRLST_U");
				}
				file_out << '\n';
				if (command->isList())
				{
					if (command->isT<CommandListConcealer>())
					{
						command->as<CommandListConcealer>()->moveChildrenAllVisible();
					}

					std::string prefix_{ prefix };
					prefix_.push_back('\t');
					recursivelyWriteFeatureList(file_out, std::move(prefix_), (CommandList*)command, censor_command_player_name);

					if (command->isT<CommandListConcealer>())
					{
						command->as<CommandListConcealer>()->moveChildrenConceal();
					}
				}
				CONSUMER_CONTINUE;
			});
		}
	};
}
