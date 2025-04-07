#include "Util.hpp"

#include <sstream>

#include <fmt/xchar.h>

#include <soup/rand.hpp>

#include "AbstractEntity.hpp"
#include "AbstractPlayer.hpp"
#include "AbstractModel.hpp"
#include "Chat.hpp"
#include "ChatCommandsCommon.hpp"
#include "Click.hpp"
#include "CNetworkPlayerMgr.hpp"
#include "CommandOutfitComponent.hpp"
#include "CommandOutfitComponentVariation.hpp"
#include "CommandList.hpp"
#include "CommandLockOutfit.hpp"
#include "CommandOutfitProp.hpp"
#include "CommandOutfitPropVariation.hpp"
#include "CommandPlayer.hpp"
#include "ConsoleLogger.hpp"
#include "CPedHeadBlendData.hpp"
#include "CViewportManager.hpp"
#include "Exceptional.hpp"
#include "ExecCtx.hpp"
#include "FiberPool.hpp"
#include "FileLogger.hpp"
#include "fmt_arg.hpp"
#include "GameToaster.hpp"
#include "GridToaster.hpp"
#include "gta_input.hpp"
#include "gta_player.hpp"
#include "Gui.hpp"
#include "Hooking.hpp"
#include "HudColour.hpp"
#include "lang.hpp"
#include "mthRandom.hpp"
#include "natives.hpp"
#include "PedModel.hpp"
#include "pointers.hpp"
#include "RelayCon.hpp"
#include "Renderer.hpp"
#include "Script.hpp"
#include "ScriptGlobal.hpp"
#include "script_thread.hpp"
#include "str2int.hpp"
#include "StringUtils.hpp"
#include "Toaster.hpp"
#include "TpUtil.hpp"
#include "WeaponLoadout.hpp"
#include "weapons.hpp"

namespace Stand
{
	void Util::bypass_string_limit(void f(const char*), const std::string& message)
	{
		f("CELL_EMAIL_BCON");
		const size_t len = message.length();
		size_t offset = 0;
		do
		{
			HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(message.substr(offset, 99).c_str());
			offset += 99;
		} while (offset < len);
	}

	void Util::toast(const Label& message, toast_t flags)
	{
		if (flags & TOAST_ABOVE_MAP)
		{
			SOUP_IF_LIKELY (pointers::CLoadingScreens_ms_Context != nullptr)
			{
				Toaster* toaster = (*pointers::CLoadingScreens_ms_Context == 0 ? g_toaster : &g_grid_toaster);
				SOUP_IF_LIKELY (toaster != nullptr)
				{
					ensureToasterContext(toaster, [toaster, message]
					{
						toaster->toast(message.getLocalisedUtf16());
					});
				}
			}
		}
		if (flags & TOAST_CONSOLE)
		{
			g_console.log(message.getLocalisedUtf8());
		}
		if (flags & TOAST_FILE)
		{
			g_logger.log(message.getLocalisedUtf8());
		}
		if (flags & TOAST_WEB)
		{
			toast_web(message);
		}
		if (flags & TOAST_CHAT)
		{
			toastChat(message, false);
		}
		else if (flags & TOAST_CHAT_TEAM)
		{
			toastChat(message, true);
		}
	}

	void Util::toast(std::string&& literal_message, toast_t flags)
	{
		return toast(LIT(std::move(literal_message)), flags);
	}

	void Util::toast(const std::wstring& literal_message, toast_t flags)
	{
		return toast(StringUtils::utf16_to_utf8(literal_message), flags);
	}

	void Util::ensureToasterContext(Toaster* toaster, std::function<void()>&& callback)
	{
		if (toaster != &g_game_toaster || ExecCtx::get().isScript())
		{
			callback();
		}
		else
		{
			FiberPool::queueJob(std::move(callback));
		}
	}

	void Util::toast_web(const Label& message)
	{
		if (!g_gui.isWebGuiActive())
		{
			return;
		}
		if (message.isLiteral())
		{
			toastWebLiteral(std::string(message.literal_str));
		}
		else
		{
			Exceptional::createManagedThread([hash{ message.getLocalisationHash() }]() mutable
			{
				EXCEPTIONAL_LOCK(g_relay.send_mtx)
				g_relay.sendLine(std::move(std::string("toast ").append(Util::to_padded_hex_string(hash))));
				EXCEPTIONAL_UNLOCK(g_relay.send_mtx)
			});
		}
	}

	void Util::toast_web(Label&& message)
	{
		if (!g_gui.isWebGuiActive())
		{
			return;
		}
		if (message.isLiteral())
		{
			toastWebLiteral(std::move(message.literal_str));
		}
		else
		{
			Exceptional::createManagedThread([hash{ message.getLocalisationHash() }]() mutable
			{
				EXCEPTIONAL_LOCK(g_relay.send_mtx)
				g_relay.sendLine(std::move(std::string("toast ").append(Util::to_padded_hex_string(hash))));
				EXCEPTIONAL_UNLOCK(g_relay.send_mtx)
			});
		}
	}

	void Util::toastWebLiteral(std::string&& message)
	{
		Exceptional::createManagedThread([message{ std::move(message) }]() mutable
		{
			EXCEPTIONAL_LOCK(g_relay.send_mtx)
			size_t del_pos;
			while ((del_pos = message.find('\n')) != std::string::npos)
			{
				del_pos++;
				g_relay.sendRaw(std::string("toast ").append(message.substr(0, del_pos)));
				message.erase(0, del_pos);
			}
			if (!message.empty())
			{
				g_relay.sendLine(std::move(std::string("toast ").append(message)));
			}
			EXCEPTIONAL_UNLOCK(g_relay.send_mtx)
		});
	}

	void Util::toastChat(const Label& message, bool team_chat, compactplayer_t recipient)
	{
		FiberPool::queueJob([=]
		{
			auto msg = message.getLocalisedUtf8();
			const bool exclude_ad = (msg.find((const char*)u8"崩溃事件 ") == 0 // "Crash Event"
				|| msg.find((const char*)u8"笼子 来自 ") == 0 // "Cage from"
				);
			msg.insert(0, ChatCommandsCommon::getEffectiveReplyPrefix());
			Chat::markAsAuto(msg);
			if (recipient == MAX_PLAYERS)
			{
				Chat::sendMessage(std::move(msg), team_chat, true, true, exclude_ad);
			}
			else
			{
				Chat::sendTargetedMessage(recipient, std::move(msg), team_chat, exclude_ad);
			}
		});
	}

	Label Util::getWotString()
	{
		std::string msg{ LANG_GET("WOT1") };
		if (msg != LANG_GET("WOT2"))
		{
			msg.push_back('\n');
			msg.append(LANG_GET("WOT2")).push_back('\n');
			msg.append(LANG_GET("WOT3")).push_back('\n');
			msg.append(LANG_GET("WOT4")).push_back('\n');
			msg.append(LANG_GET("WOT5"));
		}
		return LIT(std::move(msg));
	}

	void Util::toast_uwotm8()
	{
		Util::toast(
			getWotString(),
			TOAST_DEFAULT & ~TOAST_WEB
		);
		toast_web_uwotm8();
	}
	
	void Util::toast_web_uwotm8()
	{
		Util::toast_web(LOC("WOT1"));
		if (LANG_GET("WOT1") != LANG_GET("WOT2"))
		{
			Util::toast_web(LOC("WOT2"));
			Util::toast_web(LOC("WOT3"));
			Util::toast_web(LOC("WOT4"));
			Util::toast_web(LOC("WOT5"));
		}
	}

	void Util::onPreventedCrash(const Codename& type) noexcept
	{
		onPreventedCrash(LIT(type.toString()));
	}

	void Util::onPreventedCrash(const Codename& type, const std::string& extra_data) noexcept
	{
		std::string str = type.toString();
		str.push_back(':');
		str.append(extra_data);
		onPreventedCrash(LIT(std::move(str)));
	}

	void Util::onPreventedCrash(const Codename& type, AbstractPlayer likely_cause) noexcept
	{
		onPreventedCrash(LIT(type.toString()), likely_cause);
	}

	void Util::onPreventedCrash(const Codename& type, const std::string& extra_data, AbstractPlayer likely_cause) noexcept
	{
		std::string str = type.toString();
		str.push_back(':');
		str.append(extra_data);
		onPreventedCrash(LIT(std::move(str)), likely_cause);
	}

	void Util::onPreventedCrash(const Label& type) noexcept
	{
		__try
		{
			Util::toast(LANG_FMT("CRSH_T", FMT_ARG("type", type.getLocalisedUtf8())), TOAST_ALL);
		}
		__EXCEPTIONAL()
		{
		}
	}

	void Util::onPreventedCrash(const Label& type, AbstractPlayer likely_cause) noexcept
	{
		if (!likely_cause.isValid())
		{
			return onPreventedCrash(type);
		}

		__try
		{
			Util::toast(LANG_FMT("CRSH_T_BLM", FMT_ARG("type", type.getLocalisedUtf8()), FMT_ARG("player", likely_cause.getName())), TOAST_ALL);

			if (auto cmd = likely_cause.getCommand())
			{
				cmd->aggressive_action_warranted = true;
			}
		}
		__EXCEPTIONAL()
		{
		}
	}

	void Util::onPreventedBufferOverrun(const Codename& type) noexcept
	{
		__try
		{
			Util::toast(LANG_FMT("BU_T", FMT_ARG("type", type.toString())), TOAST_ALL);
		}
		__EXCEPTIONAL()
		{
		}
	}

	void Util::onPreventedBufferOverrun(const Label& type) noexcept
	{
		__try
		{
			Util::toast(LANG_FMT("BU_T", FMT_ARG("type", type.getLocalisedUtf8())), TOAST_ALL);
		}
		__EXCEPTIONAL()
		{
		}
	}

	int Util::get_closest_hud_colour(int target_r, int target_g, int target_b, int target_a)
	{
		//g_logger.log(fmt::format("{}, {}, {}, {}", target_r, target_g, target_b, target_a));
		int closest = 0;
		unsigned int closest_dist = -1;
		int i = NUM_HUD_COLOURS;
		while (true)
		{
			--i;
			int r, g, b, a;
			HUD::GET_HUD_COLOUR(i, &r, &g, &b, &a);
			const unsigned int dist = abs(target_r - r) + abs(target_g - g) + abs(target_b - b) + abs(target_a - a);
			if (closest_dist > dist)
			{
				closest = i;
				closest_dist = dist;
			}
			if (i == 0)
			{
				break;
			}
		}
		return closest;
	}

	void Util::show_corner_help_impl(const char* message, bool beep)
	{
		Util::BEGIN_TEXT_COMMAND_DISPLAY_HELP(message);
		HUD::END_TEXT_COMMAND_DISPLAY_HELP(0, 0, beep, -1);
	}

	void Util::show_corner_help_impl(const std::string& message, bool beep)
	{
		return show_corner_help_impl(message.c_str(), beep);
	}

	void Util::show_corner_help(const char* message, bool beep)
	{
		Util::BEGIN_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED(message);
		if (!HUD::END_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED(0))
		{
			show_corner_help_impl(message, beep);
		}
	}

	void Util::show_corner_help(const std::string& message, bool beep)
	{
		return show_corner_help(message.c_str(), beep);
	}

	bool Util::is_help_message_being_displayed(const char* message)
	{
		Util::BEGIN_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED(message);
		return HUD::END_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED(0);
	}

	bool Util::is_help_message_being_displayed(const std::string& message)
	{
		return is_help_message_being_displayed(message.c_str());
	}

	void Util::replace_corner_help(const char* message, const char* replacement_message)
	{
		if (is_help_message_being_displayed(message))
		{
			show_corner_help_impl(message, true);
		}
	}

	std::pair<int, const char*> Util::mps_to_unit_of_choice_pair(float metres_per_second)
	{
		if (unit_of_choice_is_mps)
		{
			return { int(round(metres_per_second)), "MPS" };
		}
		if (MISC::SHOULD_USE_METRIC_MEASUREMENTS())
		{
			return { int(round(metres_per_second * 3.6f)), "KMH" };
		}
		return { int(round(metres_per_second * 2.236936f)), "MPH" };
	}

	std::pair<int, const wchar_t*> Util::mps_to_unit_of_choice_pair_w(float metres_per_second)
	{
		if (unit_of_choice_is_mps)
		{
			return { int(round(metres_per_second)), L"MPS" };
		}
		if (MISC::SHOULD_USE_METRIC_MEASUREMENTS())
		{
			return { int(round(metres_per_second * 3.6f)), L"KMH" };
		}
		return { int(round(metres_per_second * 2.236936f)), L"MPH" };
	}

	std::string Util::mps_to_unit_of_choice(float metres_per_second)
	{
		auto pair = mps_to_unit_of_choice_pair(metres_per_second);
		auto str = fmt::to_string(pair.first);
		str.push_back(' ');
		str.append(pair.second);
		return str;
	}

	std::wstring Util::mps_to_unit_of_choice_w(float metres_per_second)
	{
		auto pair = mps_to_unit_of_choice_pair_w(metres_per_second);
		auto str = fmt::to_wstring(pair.first);
		str.push_back(L' ');
		str.append(pair.second);
		return str;
	}

	bool Util::copy_to_clipboard(const std::string& text)
	{
		const size_t len = (text.length() + 1) * sizeof(char);
		HGLOBAL hMem = GlobalAlloc(GHND | GMEM_DDESHARE, len);
		if (hMem != nullptr)
		{
			void* pMem = GlobalLock(hMem);
			if (pMem != nullptr)
			{
				memcpy(pMem, text.c_str(), len);
				GlobalUnlock(hMem);
				if (OpenClipboard(nullptr))
				{
					bool success = (EmptyClipboard() && SetClipboardData(CF_TEXT, hMem) != nullptr);
					CloseClipboard();
					if (success)
					{
						return true;
					}
				}
				GlobalFree(hMem);
			}
		}
		return false;
	}

	void Util::copy_to_clipboard(Click& click, const StringCastable& text)
	{
		if (copy_to_clipboard(text.value))
		{
			click.setResponse(LOC("CPSUCC"));
		}
		else
		{
			click.setResponse(LOC("CPERR"));
		}
	}

	bool Util::copy_to_clipboard_utf8(const std::string& text)
	{
		return copy_to_clipboard_utf16(StringUtils::utf8_to_utf16(text));
	}

	void Util::copy_to_clipboard_utf8(Click& click, const std::string& text)
	{
		return copy_to_clipboard_utf16(click, StringUtils::utf8_to_utf16(text));
	}

	bool Util::copy_to_clipboard_utf16(const std::wstring& text)
	{
		const size_t len = (text.length() + 1) * sizeof(wchar_t);
		HGLOBAL hMem = GlobalAlloc(GHND | GMEM_DDESHARE, len);
		if (hMem != nullptr)
		{
			void* pMem = GlobalLock(hMem);
			if (pMem != nullptr)
			{
				memcpy(pMem, text.c_str(), len);
				GlobalUnlock(hMem);
				if (OpenClipboard(nullptr))
				{
					bool success = (EmptyClipboard() && SetClipboardData(CF_UNICODETEXT, hMem) != nullptr);
					CloseClipboard();
					if (success)
					{
						return true;
					}
				}
				GlobalFree(hMem);
			}
		}
		return false;
	}

	void Util::copy_to_clipboard_utf16(Click& click, const std::wstring& text)
	{
		if (copy_to_clipboard_utf16(text))
		{
			click.setResponse(LOC("CPSUCC"));
		}
		else
		{
			click.setResponse(LOC("CPERR"));
		}
	}

	std::string Util::getClipboardTextASCII()
	{
		std::string text{};
		if (OpenClipboard(nullptr))
		{
			HANDLE hData = GetClipboardData(CF_TEXT);
			if (hData != nullptr)
			{
				auto pszText = static_cast<char*>(GlobalLock(hData));
				if (pszText != nullptr)
				{
					text = pszText;
					GlobalUnlock(hData);
				}
			}
			CloseClipboard();
		}
		return text;
	}

	std::string Util::getClipboardTextUTF8()
	{
		return StringUtils::utf16_to_utf8(getClipboardTextUTF16());
	}

	std::wstring Util::getClipboardTextUTF16()
	{
		std::wstring text{};
		if (OpenClipboard(nullptr))
		{
			HANDLE hData = GetClipboardData(CF_UNICODETEXT);
			if (hData != nullptr)
			{
				auto pszText = static_cast<wchar_t*>(GlobalLock(hData));
				if (pszText != nullptr)
				{
#if true
					text = pszText;
#else
					if (GlobalSize(hData) != 0 && (GlobalSize(hData) & 1) == 0)
					{
						text = std::wstring(pszText, (GlobalSize(hData) >> 1) - 1);
					}
#endif
					/*if (GlobalSize(hData) != (text.size() + 1) * sizeof(wchar_t))
					{
						Util::toast(fmt::format("Got {} bytes from clipboard, but actual clipboard data is sized {} bytes?!", (text.size() + 1) * sizeof(wchar_t), GlobalSize(hData)));
					}*/
					GlobalUnlock(hData);
				}
			}
			CloseClipboard();
		}
		return text;
	}

	AbstractEntity Util::getVehicle(const bool include_last_driven)
	{
		auto veh = g_player_veh;
		if (include_last_driven && !veh.isValid())
		{
			veh = g_player_ped.getVehicle(true);
		}
		return veh;
	}

	AbstractEntity Util::getVehicle(Click& click, const bool include_last_driven)
	{
		auto veh = getVehicle(include_last_driven);
		if (!veh.isValid())
		{
			click.setResponse(LOC("NOVEHFND"));
		}
		return veh;
	}

	AbstractEntity Util::getCurrentlyDrivingVehicle(Click& click)
	{
		auto veh = g_player_veh;
		if (veh.isValid() && veh.isOwnerOfVehicleAndDriver())
		{
			return veh;
		}
		else
		{
			click.setResponse(LOC("NOVEHFND"));
			return AbstractEntity::invalid();
		}
	}

	AbstractEntity Util::createPed(const int pedType, const Hash model, const v3& pos, const float heading, const bool networked)
	{
		const Ped ped = PED::CREATE_PED(pedType, model, pos.x, pos.y, pos.z, heading, networked, networked);
		SOUP_IF_UNLIKELY (ped == 0)
		{
			return AbstractEntity::invalid();
		}
		return AbstractEntity::get(ped);
	}

	AbstractEntity Util::createVehicle(const Hash hash, const v3& pos, const float heading, const bool networked)
	{
		const Vehicle veh = VEHICLE::CREATE_VEHICLE(hash, pos.x, pos.y, pos.z, heading, networked, networked, true);
		SOUP_IF_UNLIKELY (veh == 0)
		{
			return AbstractEntity::invalid();
		}
		switch (hash)
		{
		case ATSTRINGHASH("phantom2"):
			ENTITY::SET_ENTITY_CAN_ONLY_BE_DAMAGED_BY_SCRIPT_PARTICIPANTS(veh, 1);
			break;
		}
		VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); // this is needed to avoid being kicked out of some vehicles (e.g. oppressor) in SP
		return AbstractEntity::get(veh);
	}

	AbstractEntity Util::createObject(const Hash model, const v3& pos, const bool networked)
	{
		const Object obj = OBJECT::CREATE_OBJECT_NO_OFFSET(model, pos.x, pos.y, pos.z, networked, networked, true);
		SOUP_IF_UNLIKELY (obj == 0)
		{
			return AbstractEntity::invalid();
		}
		return AbstractEntity::get(obj);
	}

	int Util::get_ped_seat(const Ped& ped, const Vehicle& veh)
	{
		const int passengers = VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(veh);
		for (int i = -1; i < passengers; i++)
		{
			if (VEHICLE::GET_PED_IN_VEHICLE_SEAT(veh, i, 0) == ped)
			{
				return i;
			}
		}
		return -3;
	}

	std::string Util::ped_model_hash_to_string(AbstractModel model)
	{
		return model.getLabelPed().getEnglishUtf8();
	}

	std::wstring Util::ped_model_hash_to_localised_string(AbstractModel model)
	{
		return model.getLabelPed().getLocalisedUtf16();
	}

	Hash Util::string_to_ped_model_hash(const std::string& str)
	{
		for (const auto& model : PedModel::all)
		{
			if (model.menu_name.getEnglishUtf8() == str)
			{
				return model.hash;
			}
		}
		return str2int<Hash>(str, 0);
	}

	void Util::transform(Click& click, const AbstractModel model, std::unordered_map<std::string, std::string> outfit)
	{
		if (model.isWaterAnimal() && !ENTITY::IS_ENTITY_IN_WATER(g_player_ped))
		{
			return click.setResponse(LOC("CHNGMDL_W"));
		}
		transformNoCheck(model, std::move(outfit));
	}

	void Util::transformNoCheck(const AbstractModel model, std::unordered_map<std::string, std::string>&& outfit)
	{
		if (STREAMING::IS_MODEL_A_PED(model))
		{
			// Using model
			STREAMING::REQUEST_MODEL(model);
			while (!STREAMING::HAS_MODEL_LOADED(model))
			{
				Script::current()->yield();
			}
			// Store state
			const auto state = PositionState::save();
			Hash selected_weapon = ATSTRINGHASH("WEAPON_UNARMED");
			if (!state.hasVehicle()
				&& model.canUseProjectiles()
				&& !model.isCsPed()
				&& !model.isAnimal()
				)
			{
				selected_weapon = WEAPON::GET_SELECTED_PED_WEAPON(g_player_ped);
				if (!Weapon::isValidHash(selected_weapon)) // Animals... see modded event T6
				{
					selected_weapon = ATSTRINGHASH("WEAPON_UNARMED");
				}
			}
			const auto loadout = WeaponLoadout::fromPed(g_player_ped);
			const Vector2Plus waypoint = g_gui.waypoint;
			if (outfit.empty() && g_player_ped.getModel() == model)
			{
				outfit = get_colons_outfit_from_ped(g_player_ped);
			}
			// Transform
			PLAYER::SET_PLAYER_MODEL(g_player, model);
			AbstractEntity::updatePlayerPed();
			// Fix green hair
			if (model.isOnlineCharacter())
			{
				if (NETWORK::NETWORK_HAS_CACHED_PLAYER_HEAD_BLEND_DATA(g_player))
				{
					NETWORK::NETWORK_APPLY_CACHED_PLAYER_HEAD_BLEND_DATA(g_player_ped, g_player);
				}
			}
			// Get unarmed weapon for this model
			Hash unarmed_hash = ATSTRINGHASH("WEAPON_UNARMED");
			WEAPON::GET_CURRENT_PED_WEAPON(g_player_ped, &unarmed_hash, false); // note: selected ped weapon ≠ current ped weapon
			// Restore state
			state.restore();
			AbstractEntity::updatePlayerVehAndEnt();
			loadout.apply(g_player_ped);
			if (!state.hasVehicle())
			{
				if (selected_weapon == ATSTRINGHASH("WEAPON_UNARMED"))
				{
					WEAPON::SET_CURRENT_PED_WEAPON(g_player_ped, unarmed_hash, true);
					/*FiberPool::queueJob([]
					{
						Script::current()->yield(400);
						PAD::SET_CONTROL_VALUE_NEXT_FRAME(0, INPUT_SELECT_WEAPON, 1.0f);
					});*/
				}
				else
				{
					WEAPON::SET_CURRENT_PED_WEAPON(g_player_ped, selected_weapon, true);
				}
			}
			if (!waypoint.isNull())
			{
				Util::setWaypoint(waypoint);
			}
			// Fix invisibility by updating outfit, optionally with an outfit that the user transformed for
			if (outfit.empty())
			{
				PED::SET_PED_DEFAULT_COMPONENT_VARIATION(g_player_ped);
			}
			else
			{
				Util::applyOutfitToPlayerPed(model, outfit);
			}
			// Finish using model
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
		}
		else
		{
			Util::toast(LOC("GENFAIL"));
		}
	}

	std::array<std::int16_t, COMPACT_OUTFIT_SIZE> Util::get_compact_outfit_from_commands_state()
	{
		std::array<std::int16_t, COMPACT_OUTFIT_SIZE> outfit = {};
		for (const auto& entry : CommandLockOutfit::ped_component_commands)
		{
			outfit[((size_t)entry.first * 2ULL)] = entry.second.first->value;
			outfit[((size_t)entry.first * 2ULL) + 1ULL] = entry.second.second->value;
		}
		size_t i = (NUM_PED_COMPONENTS * 2ULL);
		for (const auto& entry : CommandLockOutfit::ped_prop_commands)
		{
			outfit[i++] = entry.second.first->value;
			outfit[i++] = entry.second.second->value;
		}
		outfit[i++] = CommandLockOutfit::hair_highlight->value;
		SOUP_ASSERT(i == COMPACT_OUTFIT_SIZE);
		return outfit;
	}

	std::unordered_map<std::string, std::string> Util::get_colons_outfit_from_commands_state()
	{
		return Util::compact_outfit_to_colons_outfit(g_player_ped.getModel(), Util::get_compact_outfit_from_commands_state(), true);
	}

	std::array<std::int16_t, COMPACT_OUTFIT_SIZE> Util::get_compact_outfit_from_ped(AbstractEntity& ped)
	{
		std::array<std::int16_t, COMPACT_OUTFIT_SIZE> outfit = {};
		for (const PedComponent& component : ped_components)
		{
			outfit[((size_t)component.id * 2ULL)] = PED::GET_PED_DRAWABLE_VARIATION(ped, component.id);
			outfit[((size_t)component.id * 2ULL) + 1ULL] = ped.getComponentTexture(component.id);
		}
		size_t i = (NUM_PED_COMPONENTS * 2ULL);
		for (const PedProp& prop : ped_props)
		{
			outfit[i++] = PED::GET_PED_PROP_INDEX(ped, prop.id, 1);
			outfit[i++] = PED::GET_PED_PROP_TEXTURE_INDEX(ped, prop.id);
		}
		if (auto headblend = ped.getHeadBlendData())
		{
			outfit[i++] = headblend->hair_highlight_colour_id;
		}
		else
		{
			outfit[i++] = -1;
		}
		SOUP_ASSERT(i == COMPACT_OUTFIT_SIZE);
		return outfit;
	}

	std::unordered_map<std::string, std::string> Util::get_colons_outfit_from_ped(AbstractEntity& ped)
	{
		return Util::compact_outfit_to_colons_outfit(ped.getModel(), Util::get_compact_outfit_from_ped(ped), true);
	}

	std::unordered_map<std::string, std::string> Util::compact_outfit_to_colons_outfit(const AbstractModel model, const std::array<std::int16_t, COMPACT_OUTFIT_SIZE>& compact_outfit, const bool include_model)
	{
		std::unordered_map<std::string, std::string> outfit = {};
		const bool is_online_character = model.isOnlineCharacter();
		if (include_model)
		{
			outfit.emplace(LANG_GET_EN("MDL"), ped_model_hash_to_string(model));
		}
		for (const auto& entry : CommandLockOutfit::ped_component_commands)
		{
			outfit.emplace(Lang::get_en(is_online_character ? entry.second.first->data->online_menu_name : entry.second.first->data->menu_name), fmt::to_string(compact_outfit.at((size_t)entry.first * 2ULL)));
			outfit.emplace(Lang::get_en(is_online_character ? entry.second.first->data->online_variation_menu_name : entry.second.first->data->variation_menu_name), fmt::to_string(compact_outfit.at(((size_t)entry.first * 2ULL) + 1ULL)));
		}
		size_t i = (NUM_PED_COMPONENTS * 2ULL);
		for (const auto& entry : CommandLockOutfit::ped_prop_commands)
		{
			outfit.emplace(Lang::get_en(entry.second.first->data->menu_name), fmt::to_string(compact_outfit.at(i++)));
			outfit.emplace(Lang::get_en(entry.second.first->data->variation_menu_name), fmt::to_string(compact_outfit.at(i++)));
		}
		outfit.emplace(LANG_GET_EN("HAIRHI"), fmt::to_string(compact_outfit.at(i++)));
		SOUP_ASSERT(i == COMPACT_OUTFIT_SIZE);
		return outfit;
	}

	void Util::loadOutfit(Command* const command_for_warning, Click& click, const std::unordered_map<std::string, std::string>& outfit)
	{
		const Hash model = Util::string_to_ped_model_hash(outfit.at(LANG_GET_EN("MDL")));
		if (model == 0)
		{
			Util::toast(LANG_FMT("LDOTFT_E2", outfit.at(LANG_GET_EN("MDL"))), TOAST_ALL); // "Outfit was made for an invalid model: {}"
		}
		else if (g_player_ped.getModel() == model)
		{
			Util::applyOutfitToPlayerPed(model, outfit);
		}
		else if (outfit.at(LANG_GET_EN("MDL")) == fmt::to_string(model))
		{
			Util::toast(LOC("LDOTFT_E3"), TOAST_DEFAULT); // "Outfit was made for a different model. :?"
		}
		else
		{
			const ClickType type = click.type;
			// "This outfit was made for {}, would you like to transform?"
			command_for_warning->showWarning(click, fmt::format(fmt::runtime(LANG_GET_W("LDOTFT_TF")), Util::ped_model_hash_to_localised_string(model)), WARNING_SKIPPABLE, [type, model, outfit](ThreadContext thread_context)
			{
				FiberPool::queueJob([type, model, outfit]() mutable
				{
					Click click(type, TC_SCRIPT_YIELDABLE);
					Util::transform(click, model, std::move(outfit));
				});
			});
		}
	}

	bool Util::convertOutfitKey(std::string& key)
	{
		if (key == "Top")
		{
			key = "Top 1";
		}
		else if (key == "Top Variation")
		{
			key = "Top 1 Variation";
		}
		else if (key == "Parachute / Bag")
		{
			key = "Parachute";
		}
		else if (key == "Parachute / Bag Variation")
		{
			key = "Parachute Variation";
		}
		else
		{
			return false;
		}
		return true;
	}

	void Util::applyOutfit(AbstractEntity& ped, const std::unordered_map<std::string, std::string>& outfit)
	{
		const bool is_online_character = ped.getModel().isOnlineCharacter();
		for (const auto& entry : CommandLockOutfit::ped_component_commands)
		{
			std::string menu_name = Lang::get_en(is_online_character ? entry.second.first->data->online_menu_name : entry.second.first->data->menu_name);
			if (outfit.find(menu_name) == outfit.end())
			{
				if (!convertOutfitKey(menu_name))
				{
					continue;
				}
				if (outfit.find(menu_name) == outfit.end())
				{
					continue;
				}
			}
			int comp = std::stoi(outfit.at(menu_name));
			int var = -1;
			std::string variation_menu_name = Lang::get_en(is_online_character ? entry.second.first->data->online_variation_menu_name : entry.second.first->data->variation_menu_name);
			if (outfit.find(variation_menu_name) != outfit.end())
			{
				var = std::stoi(outfit.at(variation_menu_name));
			}
			if (entry.first == 2)
			{
				PED::SET_PED_COMPONENT_VARIATION(ped, 2, comp, 0, 0);
				int highlight = var;
				if (auto e = outfit.find(LANG_GET_EN("HAIRHI")); e != outfit.end())
				{
					highlight = std::stoi(e->second);
				}
				PED::SET_PED_HAIR_TINT(ped, var, highlight);
			}
			else
			{
				PED::SET_PED_COMPONENT_VARIATION(ped, entry.first, comp, var, 0);
			}
		}
		for (const auto& entry : CommandLockOutfit::ped_prop_commands)
		{
			std::string menu_name = Lang::get_en(entry.second.first->data->menu_name);
			if (outfit.find(menu_name) != outfit.end())
			{
				int comp = std::stoi(outfit.at(menu_name));
				if (comp == -1)
				{
					PED::CLEAR_PED_PROP(ped, entry.first, 1);
				}
				else
				{
					int var = -1;
					std::string variation_menu_name = Lang::get_en(entry.second.first->data->variation_menu_name);
					if (outfit.find(variation_menu_name) != outfit.end())
					{
						var = std::stoi(outfit.at(variation_menu_name));
					}
					PED::SET_PED_PROP_INDEX(ped, entry.first, comp, var, true, 1);
				}
			}
		}
	}

	void Util::applyOutfitToPlayerPed(const AbstractModel model, const std::unordered_map<std::string, std::string>& outfit)
	{
		const bool is_online_character = model.isOnlineCharacter();
		for (const auto& entry : CommandLockOutfit::ped_component_commands)
		{
			std::string menu_name = Lang::get_en(is_online_character ? entry.second.first->data->online_menu_name : entry.second.first->data->menu_name);
			if (outfit.find(menu_name) == outfit.end())
			{
				if (!convertOutfitKey(menu_name))
				{
					continue;
				}
				if (outfit.find(menu_name) == outfit.end())
				{
					continue;
				}
			}
			entry.second.first->value = std::stoi(outfit.at(menu_name));
			std::string variation_menu_name = Lang::get_en(is_online_character ? entry.second.first->data->online_variation_menu_name : entry.second.first->data->variation_menu_name);
			if (outfit.find(variation_menu_name) != outfit.end())
			{
				entry.second.second->value = std::stoi(outfit.at(variation_menu_name));
			}
			Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
			entry.second.second->onChange(click, 0);
		}
		for (const auto& entry : CommandLockOutfit::ped_prop_commands)
		{
			std::string menu_name = Lang::get_en(entry.second.first->data->menu_name);
			if (outfit.find(menu_name) != outfit.end())
			{
				entry.second.first->value = std::stoi(outfit.at(menu_name));
				if (entry.second.first->value == -1)
				{
					Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
					entry.second.first->onChange(click, 0);
				}
				else
				{
					std::string variation_menu_name = Lang::get_en(entry.second.first->data->variation_menu_name);
					if (outfit.find(variation_menu_name) != outfit.end())
					{
						entry.second.second->value = std::stoi(outfit.at(variation_menu_name));
					}
					Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
					entry.second.second->onChange(click, 0);
				}
			}
		}
		if (auto e = outfit.find(LANG_GET_EN("HAIRHI")); e != outfit.end())
		{
			CommandLockOutfit::hair_highlight->value = std::stoi(e->second);
		}
		else
		{
			CommandLockOutfit::hair_highlight->value = -1;
		}
		Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
		CommandLockOutfit::hair_highlight->onChange(click, 0);
	}

	std::vector<hash_t> Util::getOutfitDataOrderRaw()
	{
		std::vector<hash_t> order = { ATSTRINGHASH("MDL") };
		for (const PedComponent& component : ped_components)
		{
			vector_emplace_back_unique(order, component.menu_name);
			vector_emplace_back_unique(order, component.variation_menu_name);
			vector_emplace_back_unique(order, component.online_menu_name);
			vector_emplace_back_unique(order, component.online_variation_menu_name);
			if (component.id == PV_COMP_HAIR)
			{
				vector_emplace_back_unique(order, ATSTRINGHASH("HAIRHI"));
			}
		}
		size_t i = (NUM_PED_COMPONENTS * 2ULL);
		for (const PedProp& prop : ped_props)
		{
			vector_emplace_back_unique(order, prop.menu_name);
			vector_emplace_back_unique(order, prop.variation_menu_name);
		}
		return order;
	}

	const unsigned long Util::menu_names_version = 55;

	bool Util::upgrade_menu_names_from_colons_and_tabs_file(std::unordered_map<std::string, std::string>& data, unsigned long version)
	{
		std::vector<std::pair<std::string, std::string>> prefix_conversion_table{};
		std::vector<std::pair<std::string, std::string>> suffix_conversion_table{};
		if (version == 29)
		{
			// -> 0.76
			prefix_conversion_table = {
				{"Online>Session>Log Leaving Players", "Online>Session>Player Leave Notifications>Write To Log"},
			};
		}
		else if (version == 30)
		{
			// -> 0.77
			prefix_conversion_table = {
				{"Online>Restrictions>", "Online>Enhancements>"},
				{"Online>Freemode Banners To Notifications", "Online>Enhancements>Freemode Banners To Notifications"},
				{"Online>Disable Daily Expenses", "Online>Enhancements>Disable Daily Expenses"},
				{"Online>Notify On Commendation", "Online>Enhancements>Notify On Commendation"},
				{"Online>Protections>Lessen Host Kicks", "Online>Protections>Host Kicks>Lessen Host Kicks"},
				{"Online>Session>Block Joins", "Online>Session>Block Joins>From Anyone"},
			};
		}
		else if (version == 31)
		{
			// -> 0.78
			prefix_conversion_table = {
				{"Game>Disables>Disable New Invites Indicator", "Game>Minimap>Disable New Invites Indicator"},
			};
		}
		else if (version == 32) // Streaming profile uses this version!
		{
			// -> 0.79
			prefix_conversion_table = {
				{"World>Water>Behaviour", "World>Water>Wave Behaviour"},
			};
		}
		else if (version == 33)
		{
			// -> 0.80
			prefix_conversion_table = {
				{"Self>Movement>Super Flight", "Self>Movement>Super Flight>Super Flight"},
				{"Online>Player History>Settings>Write Tracking Updates To Log", "Online>Player History>Settings>On Tracking Update...>Write To Log"},
				{"Game>Info Overlay>Friends", "Game>Info Overlay>Player Tags>Friends"},
				{"Game>Info Overlay>Crew Members", "Game>Info Overlay>Player Tags>Crew Members"},
				{"Game>Info Overlay>Modders", "Game>Info Overlay>Player Tags>Modders"},
				{"Game>Info Overlay>Attacked You", "Game>Info Overlay>Player Tags>Attacked You"},
				{"Game>Info Overlay>Host", "Game>Info Overlay>Player Tags>Host"},
				{"Game>Info Overlay>Next Host", "Game>Info Overlay>Player Tags>Next Host"},
				{"Game>Info Overlay>Script Host", "Game>Info Overlay>Player Tags>Script Host"},
				{"Game>Info Overlay>Spectating", "Game>Info Overlay>Player Tags>Spectating"},
				{"Game>Info Overlay>RC Vehicle", "Game>Info Overlay>Player Tags>RC Vehicle"},
				{"Game>Info Overlay>Invulnerable", "Game>Info Overlay>Player Tags>Invulnerable"},
				{"Game>Info Overlay>Off The Radar", "Game>Info Overlay>Player Tags>Off The Radar"},
				{"Game>Info Overlay>In An Interior", "Game>Info Overlay>Player Tags>In An Interior"},
				{"Game>Info Overlay>Typing", "Game>Info Overlay>Player Tags>Typing"},
			};
		}
		else if (version == 34)
		{
			// -> 0.81.7
			prefix_conversion_table = {
				{"Online>Protections>Events>Kick From Personal Vehicle", "Online>Protections>Events>Kick From Vehicle"}
			};
		}
		else if (version == 35)
		{
			// -> 0.83
			prefix_conversion_table = {
				{"Online>Quick Progress>Rig Slot Machines", "Online>Quick Progress>Casino>Slot Machines Outcome"},
				{"Online>Quick Progress>Lucky Wheel Reward", "Online>Quick Progress>Casino>Rig Slot Machines"},
				{"Online>Protections>Detections>Invalid Rank/RP", "Online>Protections>Detections>Invalid RP"},
				{"Stand>Lua Scripts>Settings>Offline Mode", "Stand>Lua Scripts>Settings>Disable Internet Access"},
			};
		}
		else if (version == 36)
		{
			// -> 0.84
			prefix_conversion_table = {
				{"Self>Weapons>Aimbot>Exclude Friends", "Self>Weapons>Aimbot>Target Selection>Exclude Friends"},
				{"Self>Weapons>Aimbot>Players", "Self>Weapons>Aimbot>Target Selection>Players"},
				{"Self>Weapons>Aimbot>Pedestrians", "Self>Weapons>Aimbot>Target Selection>Pedestrians"},
				{"Self>Weapons>Aimbot>Vehicles", "Self>Weapons>Aimbot>Target Selection>Vehicles"},
				{"Stand>Settings>Set High Priority If Injected Early", "Game>Early Inject Enhancements>Set Game To High Priority"},
				{"Stand>Settings>Skip Intro & License If Injected Early", "Game>Early Inject Enhancements>Skip Intro & License"},
			};
		}
		else if (version == 37)
		{
			// -> 0.85
			prefix_conversion_table = {
				{"Online>Player History>Settings", "Online>Player History>Meta"},
			};
			suffix_conversion_table = {
				{">GIVE_WEAPON_EVENT", ">Give Weapon Event"},
				{">REMOVE_WEAPON_EVENT", ">Remove Weapon Event"},
				{">REMOVE_ALL_WEAPONS_EVENT", ">Remove All Weapons Event"},
				{">FIRE_EVENT", ">Fire"},
				{">NETWORK_PTFX_EVENT", ">PTFX"},
			};
		}
		else if (version == 38)
		{
			// -> 0.88
			prefix_conversion_table = {
				{"Online>Protections>Knockoff Breakup Kick Reactions", "Online>Protections>Knockoff Breakup Kick Reactions>Myself"},
			};
		}
		else if (version == 39)
		{
			// -> 0.90
			prefix_conversion_table = {
				{"Online>Spoofing>Host Token Spoofing>Kick Host When Joining As Next In Queue", "Online>Spoofing>Host Token Spoofing>Kick Host When Joining As Next In Queue>Kick Host When Joining As Next In Queue"},
				{"Online>Transitions>Break Sessions But Join Fast", "Online>Transitions>Speed Up>Become Script Host"},
				{"Stand>Settings>Speculative Connections", "Stand>Settings>Session Joining>Speculative Connections"},
				{"Online>Protections>Block Entity Spam", "Online>Protections>Block Entity Spam>Block Entity Spam"},
			};
		}
		else if (version == 40)
		{
			// -> 0.90 (Logger Rewrite)
			suffix_conversion_table = {
				{">Write To Log", ">Write To Log File"},
			};
		}
		else if (version == 41)
		{
			// -> 0.91
			prefix_conversion_table = {
				{"Stand>Lua Scripts>Settings>Force Silent Stop", "Stand>Lua Scripts>Settings>Enforce Silent Stop"},
			};
		}
		else if (version == 42)
		{
			// -> 95
			suffix_conversion_table = {
				{">Bypassed Profanity Filter", ">Bypassed Message Filter"},
			};
		}
		else if (version == 43)
		{
			// -> 100
			prefix_conversion_table = {
				{"Self>Bodyguards>Formation", "Self>Bodyguards>Behaviour>Formation"},
				{"Self>Bodyguards>Aggression", "Self>Bodyguards>Behaviour>Aggression"},
				{"Self>Bodyguards>Spawn>Weapon", "Self>Bodyguards>Spawn>Primary Weapon"},
			};
		}
		else if (version == 44)
		{
			// -> 101
			prefix_conversion_table = {
				{"Online>Protections>Delete Stale Objects", "Online>Protections>Delete Stale Objects>Delete Stale Objects"},
				{"Online>Set Mental State", "Online>Mental State>Set Mental State"},
				{"Online>Pulsating Mental State", "Online>Mental State>Pulsating Mental State"},
			};
		}
		else if (version == 45)
		{
			// -> 102
			prefix_conversion_table = {
				{"Online>Protections>Block Join Karma", "Online>Protections>Block Join Karma>Block Join Karma"},
				{"Online>Session>Block Joins>From Kicked Players", "Online>Session>Block Joins>From Removed Players"},
				{"Online>Session>Block Joins>Kicked Players", "Online>Session>Block Joins>Removed Players"},
			};
		}
		else if (version == 46)
		{
			// -> 105
			prefix_conversion_table = {
				{"Vehicle>Movement>Gravity Multiplier", "Vehicle>Movement>Gravity Multiplier>Gravity Multiplier"}
			};
		}
		else if (version == 47)
		{
			// -> 107
			prefix_conversion_table = {
				{"Vehicle>Auto Transform Amphibious Cars>Auto Transform Amphibious Cars", "Vehicle>Auto Transform Submarine Cars>Auto Transform Submarine Cars"},
				{"Vehicle>Auto Transform Amphibious Cars", "Vehicle>Auto Transform Submarine Cars"},
				{"Online>Protections>Block Player Model Swaps", "Online>Protections>Block Player Model Swaps>Block Player Model Swaps"}
			};
		}
		else if (version == 48)
		{
			// -> 108
			prefix_conversion_table = {
				{"Self>Weapons>Lock Weapons>Melee Weapons>Knife", "Self>Weapons>Lock Weapons>Melee Weapons>Knife>Knife"},
				{"Self>Weapons>Lock Weapons>Melee Weapons>Knuckle Duster", "Self>Weapons>Lock Weapons>Melee Weapons>Knuckle Duster>Knuckle Duster"},
				{"Self>Weapons>Lock Weapons>Melee Weapons>Nightstick", "Self>Weapons>Lock Weapons>Melee Weapons>Nightstick>Nightstick"},
				{"Self>Weapons>Lock Weapons>Melee Weapons>Hammer", "Self>Weapons>Lock Weapons>Melee Weapons>Hammer>Hammer"},
				{"Self>Weapons>Lock Weapons>Melee Weapons>Baseball Bat", "Self>Weapons>Lock Weapons>Melee Weapons>Baseball Bat>Baseball Bat"},
				{"Self>Weapons>Lock Weapons>Melee Weapons>Golf Club", "Self>Weapons>Lock Weapons>Melee Weapons>Golf Club>Golf Club"},
				{"Self>Weapons>Lock Weapons>Melee Weapons>Crowbar", "Self>Weapons>Lock Weapons>Melee Weapons>Crowbar>Crowbar"},
				{"Self>Weapons>Lock Weapons>Melee Weapons>Bottle", "Self>Weapons>Lock Weapons>Melee Weapons>Bottle>Bottle"},
				{"Self>Weapons>Lock Weapons>Melee Weapons>Antique Cavalry Dagger", "Self>Weapons>Lock Weapons>Melee Weapons>Antique Cavalry Dagger>Antique Cavalry Dagger"},
				{"Self>Weapons>Lock Weapons>Melee Weapons>Hatchet", "Self>Weapons>Lock Weapons>Melee Weapons>Hatchet>Hatchet"},
				{"Self>Weapons>Lock Weapons>Melee Weapons>Machete", "Self>Weapons>Lock Weapons>Melee Weapons>Machete>Machete"},
				{"Self>Weapons>Lock Weapons>Melee Weapons>Flashlight", "Self>Weapons>Lock Weapons>Melee Weapons>Flashlight>Flashlight"},
				{"Self>Weapons>Lock Weapons>Melee Weapons>Switchblade", "Self>Weapons>Lock Weapons>Melee Weapons>Switchblade>Switchblade"},
				{"Self>Weapons>Lock Weapons>Melee Weapons>Pool Cue", "Self>Weapons>Lock Weapons>Melee Weapons>Pool Cue>Pool Cue"},
				{"Self>Weapons>Lock Weapons>Melee Weapons>Pipe Wrench", "Self>Weapons>Lock Weapons>Melee Weapons>Pipe Wrench>Pipe Wrench"},
				{"Self>Weapons>Lock Weapons>Melee Weapons>Battle Axe", "Self>Weapons>Lock Weapons>Melee Weapons>Battle Axe>Battle Axe"},
				{"Self>Weapons>Lock Weapons>Melee Weapons>Stone Hatchet", "Self>Weapons>Lock Weapons>Melee Weapons>Stone Hatchet>Stone Hatchet"},
				{"Self>Weapons>Lock Weapons>Melee Weapons>Candy Cane", "Self>Weapons>Lock Weapons>Melee Weapons>Candy Cane>Candy Cane"},

				{"Self>Weapons>Lock Weapons>Throwables>Grenade", "Self>Weapons>Lock Weapons>Throwables>Grenade>Grenade"},
				{"Self>Weapons>Lock Weapons>Throwables>Sticky Bomb", "Self>Weapons>Lock Weapons>Throwables>Sticky Bomb>Sticky Bomb"},
				{"Self>Weapons>Lock Weapons>Throwables>Proximity Mine", "Self>Weapons>Lock Weapons>Throwables>Proximity Mine>Proximity Mine"},
				{"Self>Weapons>Lock Weapons>Throwables>BZ Gas", "Self>Weapons>Lock Weapons>Throwables>BZ Gas>BZ Gas"},
				{"Self>Weapons>Lock Weapons>Throwables>Tear Gas", "Self>Weapons>Lock Weapons>Throwables>Tear Gas>Tear Gas"},
				{"Self>Weapons>Lock Weapons>Throwables>Molotov", "Self>Weapons>Lock Weapons>Throwables>Molotov>Molotov"},
				{"Self>Weapons>Lock Weapons>Throwables>Fire Extinguisher", "Self>Weapons>Lock Weapons>Throwables>Fire Extinguisher>Fire Extinguisher"},
				{"Self>Weapons>Lock Weapons>Throwables>Jerry Can", "Self>Weapons>Lock Weapons>Throwables>Jerry Can>Jerry Can"},
				{"Self>Weapons>Lock Weapons>Throwables>Snowball", "Self>Weapons>Lock Weapons>Throwables>Snowball>Snowball"},
				{"Self>Weapons>Lock Weapons>Throwables>Flare", "Self>Weapons>Lock Weapons>Throwables>Flare>Flare"},
				{"Self>Weapons>Lock Weapons>Throwables>Ball", "Self>Weapons>Lock Weapons>Throwables>Ball>Ball"},
				{"Self>Weapons>Lock Weapons>Throwables>Pipe Bomb", "Self>Weapons>Lock Weapons>Throwables>Pipe Bomb>Pipe Bomb"},
				{"Self>Weapons>Lock Weapons>Throwables>Hazardous Jerry Can", "Self>Weapons>Lock Weapons>Throwables>Hazardous Jerry Can>Hazardous Jerry Can"},
				{"Self>Weapons>Lock Weapons>Throwables>Fertilizer Can", "Self>Weapons>Lock Weapons>Throwables>Fertilizer Can>Fertilizer Can"},
				{"Self>Weapons>Lock Weapons>Throwables>Acid Package", "Self>Weapons>Lock Weapons>Throwables>Acid Package>Acid Package"},

				{"Self>Weapons>Lock Weapons>Pistols>Heavy Revolver", "Self>Weapons>Lock Weapons>Pistols>Heavy Revolver>Heavy Revolver"},
				{"Self>Weapons>Lock Weapons>Pistols>Pistol", "Self>Weapons>Lock Weapons>Pistols>Pistol>Pistol"},
				{"Self>Weapons>Lock Weapons>Pistols>Pistol Mk II", "Self>Weapons>Lock Weapons>Pistols>Pistol Mk II>Pistol Mk II"},
				{"Self>Weapons>Lock Weapons>Pistols>Combat Pistol", "Self>Weapons>Lock Weapons>Pistols>Combat Pistol>Combat Pistol"},
				{"Self>Weapons>Lock Weapons>Pistols>AP Pistol", "Self>Weapons>Lock Weapons>Pistols>AP Pistol>AP Pistol"},
				{"Self>Weapons>Lock Weapons>Pistols>Pistol .50", "Self>Weapons>Lock Weapons>Pistols>Pistol .50>Pistol .50"},
				{"Self>Weapons>Lock Weapons>Pistols>SNS Pistol", "Self>Weapons>Lock Weapons>Pistols>SNS Pistol>SNS Pistol"},
				{"Self>Weapons>Lock Weapons>Pistols>Heavy Pistol", "Self>Weapons>Lock Weapons>Pistols>Heavy Pistol>Heavy Pistol"},
				{"Self>Weapons>Lock Weapons>Pistols>Vintage Pistol", "Self>Weapons>Lock Weapons>Pistols>Vintage Pistol>Vintage Pistol"},
				{"Self>Weapons>Lock Weapons>Pistols>Stun Gun (SP)", "Self>Weapons>Lock Weapons>Pistols>Stun Gun (SP)>Stun Gun (SP)"},
				{"Self>Weapons>Lock Weapons>Pistols>Stun Gun (MP)", "Self>Weapons>Lock Weapons>Pistols>Stun Gun (MP)>Stun Gun (MP)"},
				{"Self>Weapons>Lock Weapons>Pistols>Flare Gun", "Self>Weapons>Lock Weapons>Pistols>Flare Gun>Flare Gun"},
				{"Self>Weapons>Lock Weapons>Pistols>Marksman Pistol", "Self>Weapons>Lock Weapons>Pistols>Marksman Pistol>Marksman Pistol"},
				{"Self>Weapons>Lock Weapons>Pistols>SNS Pistol Mk II", "Self>Weapons>Lock Weapons>Pistols>SNS Pistol Mk II>SNS Pistol Mk II"},
				{"Self>Weapons>Lock Weapons>Pistols>Heavy Revolver Mk II", "Self>Weapons>Lock Weapons>Pistols>Heavy Revolver Mk II>Heavy Revolver Mk II"},
				{"Self>Weapons>Lock Weapons>Pistols>Double-Action Revolver", "Self>Weapons>Lock Weapons>Pistols>Double-Action Revolver>Double-Action Revolver"},
				{"Self>Weapons>Lock Weapons>Pistols>Up-n-Atomizer", "Self>Weapons>Lock Weapons>Pistols>Up-n-Atomizer>Up-n-Atomizer"},
				{"Self>Weapons>Lock Weapons>Pistols>Ceramic Pistol", "Self>Weapons>Lock Weapons>Pistols>Ceramic Pistol>Ceramic Pistol"},
				{"Self>Weapons>Lock Weapons>Pistols>Navy Revolver", "Self>Weapons>Lock Weapons>Pistols>Navy Revolver>Navy Revolver"},
				{"Self>Weapons>Lock Weapons>Pistols>Perico Pistol", "Self>Weapons>Lock Weapons>Pistols>Perico Pistol>Perico Pistol"},
				{"Self>Weapons>Lock Weapons>Pistols>WM 29 Pistol", "Self>Weapons>Lock Weapons>Pistols>WM 29 Pistol>WM 29 Pistol"},

				{"Self>Weapons>Lock Weapons>Machine Guns>Micro SMG", "Self>Weapons>Lock Weapons>Machine Guns>Micro SMG>Micro SMG"},
				{"Self>Weapons>Lock Weapons>Machine Guns>Machine Pistol", "Self>Weapons>Lock Weapons>Machine Guns>Machine Pistol>Machine Pistol"},
				{"Self>Weapons>Lock Weapons>Machine Guns>Mini SMG", "Self>Weapons>Lock Weapons>Machine Guns>Mini SMG>Mini SMG"},
				{"Self>Weapons>Lock Weapons>Machine Guns>SMG", "Self>Weapons>Lock Weapons>Machine Guns>SMG>SMG"},
				{"Self>Weapons>Lock Weapons>Machine Guns>SMG Mk II", "Self>Weapons>Lock Weapons>Machine Guns>SMG Mk II>SMG Mk II"},
				{"Self>Weapons>Lock Weapons>Machine Guns>Assault SMG", "Self>Weapons>Lock Weapons>Machine Guns>Assault SMG>Assault SMG"},
				{"Self>Weapons>Lock Weapons>Machine Guns>MG", "Self>Weapons>Lock Weapons>Machine Guns>MG>MG"},
				{"Self>Weapons>Lock Weapons>Machine Guns>Combat MG", "Self>Weapons>Lock Weapons>Machine Guns>Combat MG>Combat MG"},
				{"Self>Weapons>Lock Weapons>Machine Guns>Combat MG Mk II", "Self>Weapons>Lock Weapons>Machine Guns>Combat MG Mk II>Combat MG Mk II"},
				{"Self>Weapons>Lock Weapons>Machine Guns>Unholy Hellbringer", "Self>Weapons>Lock Weapons>Machine Guns>Unholy Hellbringer>Unholy Hellbringer"},
				{"Self>Weapons>Lock Weapons>Machine Guns>Combat PDW", "Self>Weapons>Lock Weapons>Machine Guns>Combat PDW>Combat PDW"},
				{"Self>Weapons>Lock Weapons>Machine Guns>Gusenberg Sweeper", "Self>Weapons>Lock Weapons>Machine Guns>Gusenberg Sweeper>Gusenberg Sweeper"},
				{"Self>Weapons>Lock Weapons>Machine Guns>Tactical SMG", "Self>Weapons>Lock Weapons>Machine Guns>Tactical SMG>Tactical SMG"},

				{"Self>Weapons>Lock Weapons>Rifles>Assault Rifle", "Self>Weapons>Lock Weapons>Rifles>Assault Rifle>Assault Rifle"},
				{"Self>Weapons>Lock Weapons>Rifles>Assault Rifle Mk II", "Self>Weapons>Lock Weapons>Rifles>Assault Rifle Mk II>Assault Rifle Mk II"},
				{"Self>Weapons>Lock Weapons>Rifles>Carbine Rifle", "Self>Weapons>Lock Weapons>Rifles>Carbine Rifle>Carbine Rifle"},
				{"Self>Weapons>Lock Weapons>Rifles>Carbine Rifle Mk II", "Self>Weapons>Lock Weapons>Rifles>Carbine Rifle Mk II>Carbine Rifle Mk II"},
				{"Self>Weapons>Lock Weapons>Rifles>Advanced Rifle", "Self>Weapons>Lock Weapons>Rifles>Advanced Rifle>Advanced Rifle"},
				{"Self>Weapons>Lock Weapons>Rifles>Special Carbine", "Self>Weapons>Lock Weapons>Rifles>Special Carbine>Special Carbine"},
				{"Self>Weapons>Lock Weapons>Rifles>Special Carbine Mk II", "Self>Weapons>Lock Weapons>Rifles>Special Carbine Mk II>Special Carbine Mk II"},
				{"Self>Weapons>Lock Weapons>Rifles>Bullpup Rifle", "Self>Weapons>Lock Weapons>Rifles>Bullpup Rifle>Bullpup Rifle"},
				{"Self>Weapons>Lock Weapons>Rifles>Bullpup Rifle Mk II", "Self>Weapons>Lock Weapons>Rifles>Bullpup Rifle Mk II>Bullpup Rifle Mk II"},
				{"Self>Weapons>Lock Weapons>Rifles>Compact Rifle", "Self>Weapons>Lock Weapons>Rifles>Compact Rifle>Compact Rifle"},
				{"Self>Weapons>Lock Weapons>Rifles>Military Rifle", "Self>Weapons>Lock Weapons>Rifles>Military Rifle>Military Rifle"},
				{"Self>Weapons>Lock Weapons>Rifles>Heavy Rifle", "Self>Weapons>Lock Weapons>Rifles>Heavy Rifle>Heavy Rifle"},
				{"Self>Weapons>Lock Weapons>Rifles>Service Carbine", "Self>Weapons>Lock Weapons>Rifles>Service Carbine>Service Carbine"},

				{"Self>Weapons>Lock Weapons>Shotguns>Pump Shotgun", "Self>Weapons>Lock Weapons>Shotguns>Pump Shotgun>Pump Shotgun"},
				{"Self>Weapons>Lock Weapons>Shotguns>Pump Shotgun Mk II", "Self>Weapons>Lock Weapons>Shotguns>Pump Shotgun Mk II>Pump Shotgun Mk II"},
				{"Self>Weapons>Lock Weapons>Shotguns>Sweeper Shotgun", "Self>Weapons>Lock Weapons>Shotguns>Sweeper Shotgun>Sweeper Shotgun"},
				{"Self>Weapons>Lock Weapons>Shotguns>Sawed-Off Shotgun", "Self>Weapons>Lock Weapons>Shotguns>Sawed-Off Shotgun>Sawed-Off Shotgun"},
				{"Self>Weapons>Lock Weapons>Shotguns>Bullpup Shotgun", "Self>Weapons>Lock Weapons>Shotguns>Bullpup Shotgun>Bullpup Shotgun"},
				{"Self>Weapons>Lock Weapons>Shotguns>Assault Shotgun", "Self>Weapons>Lock Weapons>Shotguns>Assault Shotgun>Assault Shotgun"},
				{"Self>Weapons>Lock Weapons>Shotguns>Musket", "Self>Weapons>Lock Weapons>Shotguns>Musket>Musket"},
				{"Self>Weapons>Lock Weapons>Shotguns>Heavy Shotgun", "Self>Weapons>Lock Weapons>Shotguns>Heavy Shotgun>Heavy Shotgun"},
				{"Self>Weapons>Lock Weapons>Shotguns>Double Barrel Shotgun", "Self>Weapons>Lock Weapons>Shotguns>Double Barrel Shotgun>Double Barrel Shotgun"},
				{"Self>Weapons>Lock Weapons>Shotguns>Combat Shotgun", "Self>Weapons>Lock Weapons>Shotguns>Combat Shotgun>Combat Shotgun"},

				{"Self>Weapons>Lock Weapons>Sniper Rifles>Sniper Rifle", "Self>Weapons>Lock Weapons>Sniper Rifles>Sniper Rifle>Sniper Rifle"},
				{"Self>Weapons>Lock Weapons>Sniper Rifles>Heavy Sniper", "Self>Weapons>Lock Weapons>Sniper Rifles>Heavy Sniper>Heavy Sniper"},
				{"Self>Weapons>Lock Weapons>Sniper Rifles>Heavy Sniper Mk II", "Self>Weapons>Lock Weapons>Sniper Rifles>Heavy Sniper Mk II>Heavy Sniper Mk II"},
				{"Self>Weapons>Lock Weapons>Sniper Rifles>Marksman Rifle", "Self>Weapons>Lock Weapons>Sniper Rifles>Marksman Rifle>Marksman Rifle"},
				{"Self>Weapons>Lock Weapons>Sniper Rifles>Marksman Rifle Mk II", "Self>Weapons>Lock Weapons>Sniper Rifles>Marksman Rifle Mk II>Marksman Rifle Mk II"},
				{"Self>Weapons>Lock Weapons>Sniper Rifles>Precision Rifle", "Self>Weapons>Lock Weapons>Sniper Rifles>Precision Rifle>Precision Rifle"},

				{"Self>Weapons>Lock Weapons>Heavy Weapons>Grenade Launcher", "Self>Weapons>Lock Weapons>Heavy Weapons>Grenade Launcher>Grenade Launcher"},
				{"Self>Weapons>Lock Weapons>Heavy Weapons>RPG", "Self>Weapons>Lock Weapons>Heavy Weapons>RPG>RPG"},
				{"Self>Weapons>Lock Weapons>Heavy Weapons>Minigun", "Self>Weapons>Lock Weapons>Heavy Weapons>Minigun>Minigun"},
				{"Self>Weapons>Lock Weapons>Heavy Weapons>Firework Launcher", "Self>Weapons>Lock Weapons>Heavy Weapons>Firework Launcher>Firework Launcher"},
				{"Self>Weapons>Lock Weapons>Heavy Weapons>Railgun (SP)", "Self>Weapons>Lock Weapons>Heavy Weapons>Railgun (SP)>Railgun (SP)"},
				{"Self>Weapons>Lock Weapons>Heavy Weapons>Railgun (MP)", "Self>Weapons>Lock Weapons>Heavy Weapons>Railgun (MP)>Railgun (MP)"},
				{"Self>Weapons>Lock Weapons>Heavy Weapons>Widowmaker", "Self>Weapons>Lock Weapons>Heavy Weapons>Widowmaker>Widowmaker"},
				{"Self>Weapons>Lock Weapons>Heavy Weapons>Homing Launcher", "Self>Weapons>Lock Weapons>Heavy Weapons>Homing Launcher>Homing Launcher"},
				{"Self>Weapons>Lock Weapons>Heavy Weapons>Compact Grenade Launcher", "Self>Weapons>Lock Weapons>Heavy Weapons>Compact Grenade Launcher>Compact Grenade Launcher"},
				{"Self>Weapons>Lock Weapons>Heavy Weapons>Compact EMP Launcher", "Self>Weapons>Lock Weapons>Heavy Weapons>Compact EMP Launcher>Compact EMP Launcher"},
			};
		}
		else if (version == 49)
		{
			// -> 108.11
			prefix_conversion_table = {
				{"Self>Movement>Floppy Mode>Prioritise Synching", "Self>Movement>Floppy Mode>Prioritise Syncing"},
				{"Online>Transitions>Show Array Synch Progress", "Online>Transitions>Show Array Sync Progress"},
			};
		}
		else if (version == 50)
		{
			// -> 112
			prefix_conversion_table = {
				{"Online>Protections>Host Kicks", "Online>Protections>Host-Controlled Kicks"},
				{"Online>Protections>Love Letter & Desync Kicks", "Online>Protections>Host-Controlled Kicks"},
			};
		}
		else if (version == 51)
		{
			// -> 114
			prefix_conversion_table = {
				{"Online>Protections>Detections>Stand User Identification", "Online>Session>Stand User Identification>Stand User Identification (SUI)"},
			};
		}
		else if (version == 52)
		{
			// -> 115
			prefix_conversion_table = {
				{"Settings>Appearance>Disable Entity Previews", "Stand>Settings>Appearance>Entity Previews>Disable Entity Previews"}
			};
		}
		else if (version == 53)
		{
			// -> 116
			prefix_conversion_table = {
				{"Game>Custom DLCs>Spoof Models", "Game>Custom DLCs>Spoof Models>Spoof Models"}
			};
		}
		else if (version == 54)
		{
			// -> 24.8.1 (117)
			prefix_conversion_table = {
				{"Game>Wooting Analog Support", "Game>Analogue Keyboard Support>Analogue Keyboard Support"}
			};
		}

		if (prefix_conversion_table.empty() && suffix_conversion_table.empty())
		{
			return false;
		}
		for (const auto& conversion_entry : prefix_conversion_table)
		{
			std::unordered_set<const char*> processed_paths{};
			bool restart;
			do
			{
				restart = false;
				for (auto data_i = data.begin(); data_i != data.end(); )
				{
					if (!processed_paths.contains(data_i->first.data()))
					{
						if (data_i->first.length() >= conversion_entry.first.length() && data_i->first.substr(0, conversion_entry.first.length()) == conversion_entry.first)
						{
							auto res = data.emplace(
								std::string(conversion_entry.second).append(data_i->first.substr(conversion_entry.first.length())),
								std::move(data_i->second)
							);
							data.erase(data_i);
							processed_paths.emplace(res.first->first.data());
							restart = true;
							break;
						}
						processed_paths.emplace(data_i->first.data());
					}
					++data_i;
				}
			} while (restart);
		}
		for (const auto& conversion_entry : suffix_conversion_table)
		{
			for (auto data_i = data.begin(); data_i != data.end(); )
			{
				if (data_i->first.length() >= conversion_entry.first.length() && data_i->first.substr(data_i->first.length() - conversion_entry.first.length()) == conversion_entry.first)
				{
					data[data_i->first.substr(0, data_i->first.length() - conversion_entry.first.length()).append(conversion_entry.second)] = std::move(data_i->second);
					data_i = data.erase(data_i);
					continue;
				}
				++data_i;
			}
		}
		return true;
	}

	bool Util::upgrade_menu_names_from_colons_and_tabs_file(std::unordered_map<std::string, std::string>& data)
	{
		// Read tree version
		unsigned long version = 0;
		{
			auto entry = data.find("Tree Compatibility Version");
			if (entry != data.end())
			{
				try
				{
					version = std::stoul(entry->second);
				}
				catch (const std::exception&)
				{
				}
			}
			else if (data.empty())
			{
				return false;
			}
		}

		// Tree version is newer? Abort.
		if (version > menu_names_version)
		{
			return false;
		}

		// Upgrade to the latest tree version
		uint8_t loop_count = 0;
		while (/*g_logger.log(fmt::format("Upgrading from t.v. {}", version)),*/ upgrade_menu_names_from_colons_and_tabs_file(data, version))
		{
			if (++loop_count == 255)
			{
				Exceptional::report("Logic Error");
				break;
			}
			++version;
		}
		return loop_count != 0; // return if we did any upgrade
	}

	std::string Util::upgrade_menu_path(std::string&& path, unsigned long tree_version)
	{
		std::unordered_map<std::string, std::string> data;
		data.emplace(std::move(path), std::string());
		while (upgrade_menu_names_from_colons_and_tabs_file(data, tree_version))
		{
			++tree_version;
		}
		return std::move(data.begin()->first);
	}

	void Util::upgrade_menu_path(std::string& path, unsigned long tree_version)
	{
		path = upgrade_menu_path(std::move(path), tree_version);
	}

	void Util::BEGIN_TEXT_COMMAND_DISPLAY_TEXT(const std::string& message)
	{
		bypass_string_limit(HUD::BEGIN_TEXT_COMMAND_DISPLAY_TEXT, message);
	}

	void Util::_BEGIN_TEXT_COMMAND_LINE_COUNT(const std::string& message)
	{
		bypass_string_limit(HUD::BEGIN_TEXT_COMMAND_GET_NUMBER_OF_LINES_FOR_STRING, message);
	}

	void Util::BEGIN_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED(const std::string& message)
	{
		bypass_string_limit(HUD::BEGIN_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED, message);
	}

	void Util::BEGIN_TEXT_COMMAND_DISPLAY_HELP(const std::string& message)
	{
		bypass_string_limit(HUD::BEGIN_TEXT_COMMAND_DISPLAY_HELP, message);
	}

	void Util::_BEGIN_TEXT_COMMAND_GET_WIDTH(const std::string& message)
	{
		bypass_string_limit(HUD::BEGIN_TEXT_COMMAND_GET_SCREEN_WIDTH_OF_DISPLAY_TEXT, message);
	}

	void Util::BEGIN_TEXT_COMMAND_THEFEED_POST(const std::string& message)
	{
		bypass_string_limit(HUD::BEGIN_TEXT_COMMAND_THEFEED_POST, message);
	}

	int8_t Util::get_char_slot_no_notify()
	{
		return *pointers::is_session_started ? *ScriptGlobal(GLOBAL_CHAR_SLOT).as<int*>() : -1;
	}

	int8_t Util::getCharSlot(Click& click) noexcept
	{
		if (click.inOnline())
		{
			return ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>();
		}
		else
		{
			return -1;
		}
	}

	int8_t Util::getCharSlot(const Click& click) noexcept
	{
		if (click.inOnline())
		{
			return ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>();
		}
		else
		{
			return -1;
		}
	}

	std::string Util::GET_LABEL_TEXT_no_replacements(hash_t label)
	{
		const char* str = g_hooking.getLabelText(label);
		SOUP_IF_UNLIKELY (str == nullptr)
		{
			return "NULL";
		}
		std::string text = str;
		cleanup_label_text(text);
		return text;
	}

	std::string Util::GET_LABEL_TEXT(const char* label, const bool use_replacements)
	{
		if (!use_replacements)
		{
			return GET_LABEL_TEXT_no_replacements(rage::atStringHash(label));
		}

		const char* str = hooks::CTextFile_Get(pointers::thetext, label);
		SOUP_IF_UNLIKELY (str == nullptr)
		{
			return "NULL";
		}
		std::string text = str;
		cleanup_label_text(text);
		return text;
	}

	std::string Util::GET_LABEL_TEXT(const std::string& label, const bool use_replacements)
	{
		return GET_LABEL_TEXT(label.c_str(), use_replacements);
	}

	std::string Util::getLabelTextOrKey(const std::string& label, const bool use_replacements)
	{
		auto text = GET_LABEL_TEXT(label, use_replacements);
		SOUP_IF_LIKELY (text != "NULL")
		{
			return text;
		}
		return label;
	}

	void Util::cleanup_label_text(std::string& text)
	{
		StringUtils::replace_all(text, (const char*)u8"µ", " ");
	}

	bool Util::isMeleeDamageModifierLegitimate(const float modifier)
	{
		return modifier == 1.0f // Default
			|| modifier == 2.0f || modifier == 1.9550343f // BST
			|| modifier == 100.0f // The Beast
			;
	}

	bool Util::isWeaponDamageModifierLegitimate(const float modifier)
	{
		if (modifier <= 1.0f)
		{
			return modifier >= 0.71358746f;
		}
		else
		{
			return modifier == 1.12f || modifier == 1.1143695f // MC Stand Your Ground
				|| modifier == 1.02f || modifier == 1.0166178f // MC Stand Your Ground
				|| modifier == 1.44f || modifier == 1.4369501f // BST
				;
		}
	}

	bool Util::isMaxHealthLegitimate(const int max_health)
	{
		switch (max_health)
		{
		case 175:
		case 200:
		case 238:
		case 256:
		case 274:
		case 292:
		case 310:
		case 328:
		case 984:
		case 2500:
		case 2600:
		case 5000:
			return true;
		}

		return false;
	}

	int Util::getMaxHealthForRank(const int rank)
	{
		constexpr int RANK_HEALTH_VALUE_0 = 238;
		constexpr int RANK_HEALTH_VALUE_1 = 256;
		constexpr int RANK_HEALTH_VALUE_2 = 274;
		constexpr int RANK_HEALTH_VALUE_3 = 292;
		constexpr int RANK_HEALTH_VALUE_4 = 310;
		constexpr int RANK_HEALTH_VALUE_5 = 328;

		if (rank < 19)
		{
			return RANK_HEALTH_VALUE_0;
		}
		else if (rank < 39)
		{
			return RANK_HEALTH_VALUE_1;
		}
		else if (rank < 59)
		{
			return RANK_HEALTH_VALUE_2;
		}
		else if (rank < 79)
		{
			return RANK_HEALTH_VALUE_3;
		}
		else if (rank < 99)
		{
			return RANK_HEALTH_VALUE_4;
		}

		return RANK_HEALTH_VALUE_5;
	}

	int Util::get_rp_required_for_rank(const int rank)
	{
		if (rank <= 1)
		{
			return 0;
		}
		switch (rank)
		{
		case 2: return 800;
		case 3: return 2100;
		case 4: return 3800;
		case 5: return 6100;
		case 6: return 9500;
		case 7: return 12500;
		case 8: return 16000;
		case 9: return 19800;
		case 10: return 24000;
		case 11: return 28500;
		case 12: return 33400;
		case 13: return 38700;
		case 14: return 44200;
		case 15: return 50200;
		case 16: return 56400;
		case 17: return 63000;
		case 18: return 69900;
		case 19: return 77100;
		case 20: return 84700;
		case 21: return 92500;
		case 22: return 100700;
		case 23: return 109200;
		case 24: return 118000;
		case 25: return 127100;
		case 26: return 136500;
		case 27: return 146200;
		case 28: return 156200;
		case 29: return 166500;
		case 30: return 177100;
		case 31: return 188000;
		case 32: return 199200;
		case 33: return 210700;
		case 34: return 222400;
		case 35: return 234500;
		case 36: return 246800;
		case 37: return 259400;
		case 38: return 272300;
		case 39: return 285500;
		case 40: return 299000;
		case 41: return 312700;
		case 42: return 326800;
		case 43: return 341000;
		case 44: return 355600;
		case 45: return 370500;
		case 46: return 385600;
		case 47: return 401000;
		case 48: return 416600;
		case 49: return 432600;
		case 50: return 448800;
		case 51: return 465200;
		case 52: return 482000;
		case 53: return 499000;
		case 54: return 516300;
		case 55: return 533800;
		case 56: return 551600;
		case 57: return 569600;
		case 58: return 588000;
		case 59: return 606500;
		case 60: return 625400;
		case 61: return 644500;
		case 62: return 663800;
		case 63: return 683400;
		case 64: return 703300;
		case 65: return 723400;
		case 66: return 743800;
		case 67: return 764500;
		case 68: return 785400;
		case 69: return 806500;
		case 70: return 827900;
		case 71: return 849600;
		case 72: return 871500;
		case 73: return 893600;
		case 74: return 916000;
		case 75: return 938700;
		case 76: return 961600;
		case 77: return 984700;
		case 78: return 1008100;
		case 79: return 1031800;
		case 80: return 1055700;
		case 81: return 1079800;
		case 82: return 1104200;
		case 83: return 1128800;
		case 84: return 1153700;
		case 85: return 1178800;
		case 86: return 1204200;
		case 87: return 1229800;
		case 88: return 1255600;
		case 89: return 1281700;
		case 90: return 1308100;
		case 91: return 1334600;
		case 92: return 1361400;
		case 93: return 1388500;
		case 94: return 1415800;
		case 95: return 1443300;
		case 96: return 1471100;
		case 97: return 1499100;
		}
		//return 25 * (int)pow(rank, 2) + 23575 * rank - 1023150;
		return 25 * (rank * rank) + 23575 * rank - 1023150;
	}

	int Util::get_car_club_rep_required_for_level(int rank)
	{
		auto fTier = (float)rank;
		float fIncrementalFactor = 5.0f / 2.0f;
		return (int)((fTier * 100.f) - 100.f + (fTier - 3.f) * ((fIncrementalFactor * (fTier - 3.f)) + fIncrementalFactor));
	}

	float Util::VDIST(const float x1, const float y1, const float x2, const float y2)
	{
		return (float)sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	}

	void Util::STAT_SET_INT_IF_LESS(const Hash hash, const int value)
	{
		int cur_value;
		if (STATS::STAT_GET_INT(hash, &cur_value, -1) && cur_value < value)
		{
			STATS::STAT_SET_INT(hash, value, true);
		}
	}

	void Util::setWaypoint(const Vector2Plus& coords)
	{
		if (g_gui.waypoint.x != coords.x || g_gui.waypoint.y != coords.y)
		{
			g_gui.waypoint = coords;
			HUD::SET_NEW_WAYPOINT(coords.x, coords.y);
		}
		else if (g_gui.waypoint.z_is_guessed && !coords.z_is_guessed)
		{
			g_gui.waypoint.unguessZ(coords.z);
		}
	}

	void Util::setWaypoint2d(const rage::Vector2& coords)
	{
		setWaypoint(Vector2Plus(coords.x, coords.y));
	}

	void Util::setWaypoint2d(const v3& coords)
	{
		setWaypoint(Vector2Plus(coords.x, coords.y));
	}

	void Util::setWaypoint3d(const v3& coords)
	{
		setWaypoint(Vector2Plus(coords.x, coords.y, coords.z));
	}

	playerbitset_t Util::get_session_players_bitflag()
	{
		playerbitset_t bitflag = 0;
		if (*pointers::is_session_started)
		{
			if (const CNetworkPlayerMgr* playermgr = *pointers::network_player_mgr)
			{
				for (player_t i = 0; i < MAX_PLAYERS; i++)
				{
					if (playermgr->Players[i] != nullptr)
					{
						bitflag |= 1 << i;
					}
				}
			}
		}
		return bitflag;
	}

	playerbitset_t Util::get_session_players_bitflag(const std::vector<AbstractPlayer>& players)
	{
		playerbitset_t bitflag = 0;
		for (const AbstractPlayer p : players)
		{
			bitflag |= (1 << p);
		}
		return bitflag;
	}

	template <typename T>
	inline static std::string to_hex_string_impl(T dec)
	{
		std::stringstream stream;
		stream << std::uppercase << std::hex << dec;
		return stream.str();
	}

	std::string Util::to_hex_string(uint16_t dec)
	{
		return to_hex_string_impl(dec);
	}

	std::string Util::to_hex_string(uint32_t dec)
	{
		return to_hex_string_impl(dec);
	}

	template <typename T>
	inline static std::string to_hex_string_with_0x_impl(T dec)
	{
		std::stringstream stream;
		stream << "0x" << std::uppercase << std::hex << dec;
		return stream.str();
	}

	std::string Util::to_hex_string_with_0x(uint32_t dec)
	{
		return to_hex_string_with_0x_impl(dec);
	}

	template <typename T>
	inline static std::string to_padded_hex_string_impl(T dec)
	{
		std::stringstream stream;
		stream << std::setfill('0') << std::setw(sizeof(T) * 2) << std::uppercase << std::hex << dec;
		return stream.str();
	}

	std::string Util::to_padded_hex_string(int32_t dec)
	{
		return to_padded_hex_string_impl(dec);
	}

	std::string Util::to_padded_hex_string(uint32_t dec)
	{
		return to_padded_hex_string_impl(dec);
	}

	std::string Util::to_padded_hex_string(int64_t dec)
	{
		return to_padded_hex_string_impl(dec);
	}

	std::string Util::to_padded_hex_string(uint64_t dec)
	{
		return to_padded_hex_string_impl(dec);
	}

	template <typename T>
	inline static std::string to_padded_hex_string_with_0x_impl(T dec)
	{
		std::stringstream stream;
		stream << "0x" << std::setfill('0') << std::setw(sizeof(T) * 2) << std::uppercase << std::hex << dec;
		return stream.str();
	}

	std::string Util::to_padded_hex_string_with_0x(int32_t dec)
	{
		return to_padded_hex_string_with_0x_impl(dec);
	}

	std::string Util::to_padded_hex_string_with_0x(uint32_t dec)
	{
		return to_padded_hex_string_with_0x_impl(dec);
	}

	std::string Util::to_padded_hex_string_with_0x(int64_t dec)
	{
		return to_padded_hex_string_with_0x_impl(dec);
	}

	std::string Util::to_padded_hex_string_with_0x(uint64_t dec)
	{
		return to_padded_hex_string_with_0x_impl(dec);
	}

	std::string Util::get_module_file_name(HMODULE hmod)
	{
		char lpFilename[MAX_PATH] = { 0 };
		GetModuleFileNameA(hmod, lpFilename, MAX_PATH);
		std::string library_name = lpFilename;
		const size_t last_slash_i = library_name.find_last_of("\\/");
		if (last_slash_i != std::string::npos)
		{
			library_name.erase(0, last_slash_i + 1);
		}
		/*if (library_name.length() > 4)
		{
			const std::string& last_4_chars = library_name.substr(library_name.length() - 4);
			if (last_4_chars == ".dll" || last_4_chars == ".exe")
			{
				library_name.erase(library_name.length() - 4);
			}
		}*/
		return library_name;
	}

	bool Util::bin_file(const std::wstring& path, const char* const data, const size_t data_size)
	{
		if (std::filesystem::exists(path))
		{
			return false;
		}
		bin_file_write(path, data, data_size);
		return true;
	}

	void Util::bin_file_write(const std::wstring& path, const char* const data, const size_t data_size)
	{
		std::ofstream out(path, std::ios::binary | std::ios::out);
		out.write(data, data_size);
		out.close();
	}

	void Util::bin_file_write(const std::wstring& path, const std::string& data)
	{
		return bin_file_write(path, data.c_str(), data.size());
	}

	player_t Util::generate_invalid_player_id()
	{
		return soup::rand.t<player_t>(MAX_PLAYERS, INT_MAX);
	}

	player_t Util::get_invalid_player_id()
	{
		player_t fallback = 0;
		if (*pointers::is_session_started)
		{
			if (const CNetworkPlayerMgr* playermgr = *pointers::network_player_mgr)
			{
				for (player_t i = 0; i < MAX_PLAYERS; i++)
				{
					if (playermgr->Players[i] == nullptr)
					{
						return i;
					}
					if(i != g_player)
					{
						fallback = i;
					}
				}
			}
		}
		return fallback;
	}

	player_t Util::get_invalid_player_id(player_t fallback)
	{
		if (*pointers::is_session_started)
		{
			if (const CNetworkPlayerMgr* playermgr = *pointers::network_player_mgr)
			{
				for (player_t i = 0; i < MAX_PLAYERS; i++)
				{
					if (playermgr->Players[i] == nullptr)
					{
						return i;
					}
				}
			}
		}
		return fallback;
	}

	void Util::ensureScriptThreadIfAllowed(ThreadContext thread_context, std::function<void()>&& func)
	{
		if (thread_context_is_script(thread_context))
		{
			func();
		}
		else
		{
			queueJobIfAllowed(std::move(func));
		}
	}

	void Util::queueJobIfAllowed(std::function<void()>&& func)
	{
		if (g_renderer.doesGameplayStateAllowScriptExecution())
		{
			FiberPool::queueJob(std::move(func));
		}
	}

	void Util::ensureYieldableScriptThread(ThreadContext thread_context, std::function<void()>&& func)
	{
		if (thread_context == TC_SCRIPT_YIELDABLE)
		{
			func();
		}
		else
		{
			FiberPool::queueJob(std::move(func));
		}
	}

	float Util::angles_dist(float a1, float a2) noexcept
	{
		/*float diff = (a2 - a1 + 180) % 360 - 180;
		return diff < -180 ? diff + 360 : diff;*/
		return 180 - abs(abs(a1 - a2) - 180);
	}

	bool Util::is_bigmap_active()
	{
		return ScriptGlobal(GLOBAL_BIGMAP).get<int>() != 0
			&& GtaThread::fromHash(ATSTRINGHASH("freemode")) != nullptr
			;
	}

	std::string Util::getRandomLicensePlate(int seed)
	{
		rage::mthRandom rng(seed);

		std::string res{};
		res.push_back((char)rng.GetRanged('0', '9'));
		res.push_back((char)rng.GetRanged('0', '9'));
		res.push_back((char)rng.GetRanged('A', 'Z'));
		res.push_back((char)rng.GetRanged('A', 'Z'));
		res.push_back((char)rng.GetRanged('A', 'Z'));
		res.push_back((char)rng.GetRanged('0', '9'));
		res.push_back((char)rng.GetRanged('0', '9'));
		res.push_back((char)rng.GetRanged('0', '9'));
		return res;
	}

	// CScriptHud::GetScreenPosFromWorldCoord does adjustments for multihead monitor configs, we don't want that.
	bool Util::GET_SCREEN_COORD_FROM_WORLD_COORD_NO_ADJUSTMENT(float worldX, float worldY, float worldZ, float* screenX, float* screenY)
	{
		if (GRAPHICS::GET_SCREEN_COORD_FROM_WORLD_COORD(worldX, worldY, worldZ, screenX, screenY))
		{
			const rage::grcViewport* vp = pointers::viewport_manager->GetCurrentGameGrcViewport();

			soup::Vector3 v(worldX, worldY, worldZ);
			v = (vp->m_FullComposite * v);
			*screenX = (v.x / vp->GetWidth());
			*screenY = (v.y / vp->GetHeight());
			return true;
		}
		return false;
	}

	bool Util::is_legit_ambient_pickup(hash_t type, int value, hash_t model)
	{
		switch (type)
		{
		case ATSTRINGHASH("pickup_money_variable"):
			return model != ATSTRINGHASH("p_poly_bag_01_s");

		case ATSTRINGHASH("pickup_money_med_bag"): // seen with model=prop_cash_pile_01
			return value < 100;

		case ATSTRINGHASH("pickup_gang_attack_money"): // seen with model=0x00000000
			return value == 500;
		}
		return false;
	}
}
