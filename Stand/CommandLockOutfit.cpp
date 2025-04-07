#include "CommandLockOutfit.hpp"

#include <fmt/core.h>

#include "AbstractEntity.hpp"
#include "AbstractModel.hpp"
#include "Auth.hpp"
#include "CommandOutfitComponent.hpp"
#include "CommandOutfitComponentVariation.hpp"
#include "CommandList.hpp"
#include "CommandOutfitProp.hpp"
#include "CommandOutfitPropVariation.hpp"
#include "Gui.hpp"
#include "natives.hpp"
#include "ScriptGlobal.hpp"
#include "TransitionState.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandLockOutfit::CommandLockOutfit(CommandList* const parent)
		: CommandToggleNoCorrelation(parent, LOC("LKOTFT"), { CMDNAME("lockoutfit"), CMDNAME("outfitlock") }, LOC("LKOTFT_H"))
	{
		instance = this;
		player_prev_model = 0;
	}

	CommandLockOutfit::~CommandLockOutfit()
	{
		if (instance == this)
		{
			instance = nullptr;
		}
	}

	void CommandLockOutfit::onTick()
	{
		if (instance == nullptr
			|| *ScriptGlobal(GLOBAL_TRANSITION_STATE).as<TransitionState*>() == TRANSITION_STATE_IS_FM_AND_TRANSITION_READY
			)
		{
			return;
		}
		const bool model_changed = (player_prev_model != g_player_ped.getModel());
		SOUP_IF_UNLIKELY (model_changed)
		{
			if (player_prev_model != 0 && instance->m_on)
			{
				model_outfits[player_prev_model] = Util::get_compact_outfit_from_commands_state();
			}
			player_prev_model = g_player_ped.getModel();
			auto outfit_e = model_outfits.find(g_player_ped.getModel());
			if (outfit_e == model_outfits.end())
			{
				instance->setOnIndication(false);
			}
			else
			{
				instance->setOnIndication(true);
				Util::applyOutfitToPlayerPed(g_player_ped.getModel(), Util::compact_outfit_to_colons_outfit(g_player_ped.getModel(), outfit_e->second, false));
			}
			for (const auto& entry : ped_component_commands)
			{
				if (entry.first == 0)
				{
					if (g_player_ped.getModel() == ATSTRINGHASH("player_zero"))
					{
						entry.second.first->setMinValue(0);
						entry.second.first->setMaxValue(0);
					}
					else
					{
						entry.second.first->setMinValue(-1);
						entry.second.first->setMaxValue(g_player_ped.getComponentMaxDrawable(entry.first));
					}
				}
				else
				{
					entry.second.first->setMaxValue(g_player_ped.getComponentMaxDrawable(entry.first));
					if (entry.first == 2)
					{
						entry.second.second->max_value = PED::GET_NUM_PED_HAIR_TINTS();
					}
					else if (g_player_ped.getModel().isOnlineCharacter())
					{
						entry.second.first->setMenuNameNoWeb(LOC_RT(entry.second.first->data->online_menu_name));
						entry.second.first->command_names = { entry.second.first->data->online_command_name };
						entry.second.second->setMenuNameNoWeb(LOC_RT(entry.second.first->data->online_variation_menu_name));
						entry.second.second->command_names = { entry.second.first->data->online_variation_command_name };
					}
					else
					{
						entry.second.first->setMenuNameNoWeb(LOC_RT(entry.second.first->data->menu_name));
						entry.second.first->command_names = { entry.second.first->data->command_name };
						entry.second.second->setMenuNameNoWeb(LOC_RT(entry.second.first->data->variation_menu_name));
						entry.second.second->command_names = { entry.second.first->data->variation_command_name };
					}
				}
			}
			for (const auto& entry : ped_prop_commands)
			{
				entry.second.first->setMaxValue(PED::GET_NUMBER_OF_PED_PROP_DRAWABLE_VARIATIONS(g_player_ped, entry.first) - 1);
				entry.second.second->setMaxValue(PED::GET_NUMBER_OF_PED_PROP_TEXTURE_VARIATIONS(g_player_ped, entry.first, entry.second.first->value) - 1);
			}
			instance->parent->updateWebState();
		}
		if (model_changed || instance->m_on || instance->parent->isCurrentUiOrWebList())
		{
			updateOutfitCommands();
		}
	}

	void CommandLockOutfit::updateOutfitCommands()
	{
		for (const auto& entry : ped_component_commands)
		{
			if (instance->m_on)
			{
				if (PED::GET_PED_DRAWABLE_VARIATION(g_player_ped, entry.first) != entry.second.first->value || g_player_ped.getComponentTexture(entry.first) != entry.second.second->value)
				{
					Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
					entry.second.second->onChange(click, 0);
				}
			}
			else
			{
				int drawable = PED::GET_PED_DRAWABLE_VARIATION(g_player_ped, entry.first);
				if (entry.second.first->value != drawable)
				{
					if (!entry.second.first->isFocused())
					{
						Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
						entry.second.first->setValueIndicator(click, drawable);
					}
				}
				int texture = g_player_ped.getComponentTexture(entry.first);
				if (entry.second.second->value != texture)
				{
					if (!entry.second.second->isFocused())
					{
						Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
						entry.second.second->setValueIndicator(click, texture);
					}
				}
			}
			if (entry.first != PV_COMP_HAIR)
			{
				entry.second.second->updateRange();
			}
		}
		for (const auto& entry : ped_prop_commands)
		{
			if (instance->m_on)
			{
				if (PED::GET_PED_PROP_INDEX(g_player_ped, entry.first, 1) != entry.second.first->value || PED::GET_PED_PROP_TEXTURE_INDEX(g_player_ped, entry.first) != entry.second.second->value)
				{
					Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
					if (entry.second.first->value == -1)
					{
						entry.second.first->onChange(click, 0);
					}
					else
					{
						entry.second.second->onChange(click, 0);
					}
				}
			}
			else
			{
				int drawable = PED::GET_PED_PROP_INDEX(g_player_ped, entry.first, 1);
				if (entry.second.first->value != drawable)
				{
					if (!entry.second.first->isFocused())
					{
						Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
						entry.second.first->setValueIndicator(click, drawable);
					}
				}
				int texture = PED::GET_PED_PROP_TEXTURE_INDEX(g_player_ped, entry.first);
				if (entry.second.second->value != texture)
				{
					if (!entry.second.second->isFocused())
					{
						Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
						entry.second.second->setValueIndicator(click, texture);
					}
				}
			}
		}
	}

	void CommandLockOutfit::onClick(Click& click)
	{
		if (!job_queued)
		{
			onClickSoundEffect(click);
			click.ensureScriptThread([this](Click& click)
			{
				if (!m_on)
				{
					updateOutfitCommands();
				}
				else
				{
					model_outfits.erase(g_player_ped.getModel());
				}
				toggleState(click);
			});
		}
	}

	std::string CommandLockOutfit::getState() const
	{
		std::string state{};
		if (m_on)
		{
			model_outfits[g_player_ped.getModel()] = Util::get_compact_outfit_from_commands_state();
		}
		for (const auto& model_outfit : model_outfits)
		{
			state.append(fmt::to_string(model_outfit.first)).push_back('<');
			for (const auto& outfit_part : model_outfit.second)
			{
				state.append(fmt::to_string(outfit_part)).push_back(':');
			}
			state.erase(state.length() - 1).push_back('>');
		}
		return state;
	}

	std::string CommandLockOutfit::getDefaultState() const
	{
		return {};
	}

	void CommandLockOutfit::setState(Click& click, const std::string& state)
	{
		m_on = false;
		model_outfits.clear();
		if (!state.empty())
		{
			std::string state_ = state;
			size_t model_del_pos;
			while ((model_del_pos = state_.find('>')) != std::string::npos)
			{
				std::string model_data = state_.substr(0, model_del_pos);
				state_.erase(0, model_del_pos + 1);
				size_t outfit_del_pos = model_data.find('<');
				auto model = (Hash)std::stoull(model_data.substr(0, outfit_del_pos));
				model_data.erase(0, outfit_del_pos + 1);
				model_outfits[model] = {};
				size_t part_del_pos = 0;
				size_t i = 0;
				while ((part_del_pos = model_data.find(':')) != std::string::npos)
				{
					std::string val = model_data.substr(0, part_del_pos);
					model_data.erase(0, part_del_pos + 1);
					model_outfits[model][i++] = std::stoi(val);
				}
				model_outfits[model][i++] = std::stoi(model_data);
				// i = 35 if saved with 102

				// Default missing props
				while (i < COMPACT_OUTFIT_LEGACY_SIZE)
				{
					model_outfits[model][i++] = ((i % 2) ? 0 : -1);
				}

				// Default missing extra data, currently only hair highlight
				SOUP_ASSERT(i <= COMPACT_OUTFIT_SIZE);
				while (i != COMPACT_OUTFIT_SIZE)
				{
					model_outfits[model][i++] = -1;
				}
			}
		}
		player_prev_model = 0;
	}

	void CommandLockOutfit::applyDefaultState()
	{
		m_on = false;
		model_outfits.clear();
		player_prev_model = 0;
	}

	void CommandLockOutfit::updateOutfit(void(*cb)())
	{
		Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
		const bool prev_on = m_on;
		if (prev_on)
		{
			setStateBool(click, false);
		}
		cb();
		if (prev_on)
		{
			setStateBool(click, true);
		}
	}

	void CommandLockOutfit::setHairColourFromCommands()
	{
		int colour = CommandLockOutfit::ped_component_commands[PV_COMP_HAIR].second->value;
		int highlight = CommandLockOutfit::hair_highlight->value;
		if (highlight == -1)
		{
			highlight = colour;
		}
		PED::SET_PED_HAIR_TINT(g_player_ped, colour, highlight);
	}
}
