#pragma once

#include "CommandToggle.hpp"

#include "AbstractPlayer.hpp"
#include "Auth.hpp"
#include "BackgroundBlur.hpp"
#include "CommandPlayer.hpp"
#include "huddecl.hpp"
#include "pointers.hpp"
#include "Renderer.hpp"
#include "ScriptGlobal.hpp"
#include "StringUtils.hpp"
#include "tbScreenshotMode.hpp"
#include "Util.hpp"

#define PLAYERBAR_DUMMY false

namespace Stand
{
	class CommandPlayerbarMain : public CommandToggle
	{
	private:
		BackgroundBlur bgblur;

	public:
		explicit CommandPlayerbarMain(CommandList* const parent)
			: CommandToggle(parent, LOC("PBAR"), { CMDNAME("playerbar") }, NOLABEL, false, CMDFLAGS_TOGGLE | CMDFLAG_FEATURELIST_REGULAR)
		{
		}

		static constexpr auto spacer_x = 5;
		static constexpr auto spacer_y = 5;

		void onEnable(Click& click) final
		{
			if (!click.canBypassEditionRestrictions()
				&& !click.isRegularEdition()
				)
			{
				return onChangeImplUnavailable(click);
			}
			registerPresentEventHandler([this]
			{
				HANDLER_CHECK(m_on);
				bool can_show = !g_tb_screenshot_mode.isEnabled();
				if (*pointers::is_session_started && *pointers::CLoadingScreens_ms_Context == 0)
				{
					switch (parent->as<CommandPlayerbar>()->hide_if->value)
					{
					case 1:
						if (ScriptGlobal(GLOBAL_PLAYERSTATUS1_BASE + GLOBAL_PLAYERSTATUS1_MPINFOSTATE).get<int>() == 1)
						{
							can_show = false;
						}
						break;

					case 2:
						if (ScriptGlobal(GLOBAL_PLAYERSTATUS1_BASE + GLOBAL_PLAYERSTATUS1_MPINFOSTATE).get<int>() != 0)
						{
							can_show = false;
						}
						break;
					}
				}
				if (can_show)
				{
					auto players = AbstractPlayer::listExcept();
					if (parent->as<CommandPlayerbar>()->hide_no_tags->m_on)
					{
						Util::predicate_remove<AbstractPlayer>(players, [](const AbstractPlayer& p)
						{
							if (auto cmd = p.getCommand())
							{
#define CHECK_FLAG(id, name) if ((cmd->flags & (1 << id)) && CommandPlayer::flag_chars[id] != 0) return false;
								FOR_ALL_FLAGS(CHECK_FLAG);
							}
							return true;
						});
					}
#if PLAYERBAR_DUMMY
					players = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };
#endif
					if (!players.empty()
						&& (players.size() != 1 || parent->as<CommandPlayerbar>()->show_if_alone->m_on)
						&& g_auth.license_permissions >= LICPERM_REGULAR
						)
					{
						DirectX::SimpleMath::Vector2 max_size{};
						for (const auto& p : players)
						{
							auto size = g_renderer.sizeC2H(g_renderer.getTextSize(getPlayerName(p), parent->as<CommandPlayerbar>()->text_scale));
							if (size.x > max_size.x)
							{
								max_size.x = size.x;
							}
							if (size.y > max_size.y)
							{
								max_size.y = size.y;
							}
						}

						const float entry_width = (max_size.x + (spacer_x * 2));

						uint8_t num_rows = 1;
						float row_len = 0.0f;
						float max_row_len = row_len;
						for (compactplayer_t i = 0; i < players.size(); ++i)
						{
							if ((row_len + entry_width) >= HUD_WIDTH)
							{
								if (row_len > max_row_len)
								{
									max_row_len = row_len;
								}
								++num_rows;
								row_len = entry_width;
							}
							else
							{
								row_len += entry_width;
							}
						}
						if (row_len > max_row_len)
						{
							max_row_len = row_len;
						}

						const float row_height = max_size.y + (spacer_y * 2);

						if (auto passes = parent->as<CommandPlayerbar>()->bgblur->value)
						{
							bgblur.drawH(0, 0, HUD_WIDTH, num_rows * row_height, passes);
						}
						g_renderer.drawRectC(0, 0, g_renderer.client_size.x, g_renderer.sizeH2C(0, num_rows * row_height).y, g_renderer.bgRectColour);

						const float base_x = ((HUD_WIDTH - max_row_len) * 0.5f) + (max_size.x * 0.5f) + spacer_x;
						float x = base_x;
						float y = spacer_y + (max_size.y * 0.5f);
						for (auto it = players.begin(); true; )
						{
							g_renderer.drawTextH(x, y, getPlayerName(*it), ALIGN_CENTRE, parent->as<CommandPlayerbar>()->text_scale, getPlayerColour(*it), false);

							if (++it == players.end())
							{
								break;
							}

							x += entry_width;
							if (x > HUD_WIDTH)
							{
								x = base_x;
								y += row_height;
							}
						}
					}
				}
				HANDLER_END;
			});
		}

		[[nodiscard]] std::wstring getPlayerName(const AbstractPlayer& p) const
		{
#if PLAYERBAR_DUMMY
			return StringUtils::utf8_to_utf16(AbstractPlayer::getMaskName(fmt::to_string(p)));
#else
			return p.getNameW(parent->as<CommandPlayerbar>()->show_tags->m_on);
#endif
		}

		[[nodiscard]] DirectX::SimpleMath::Color getPlayerColour(const AbstractPlayer& p) const
		{
			if (auto cmd = p.getCommand())
			{
#define CHECK_COLOUR(id, name) if ((cmd->flags & (1 << id)) && parent->as<CommandPlayerbar>()->tag_provides_colour[id]) return parent->as<CommandPlayerbar>()->tag_colours[id];
				FOR_ALL_FLAGS(CHECK_COLOUR);
			}
			return parent->as<CommandPlayerbar>()->tag_colours[FLAG_COUNT];
		}
	};
}
