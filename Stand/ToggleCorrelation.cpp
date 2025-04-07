#include "ToggleCorrelation.hpp"

#include "AbstractEntity.hpp"
#include "fmt_arg.hpp"
#include "Gui.hpp"
#include "is_session.hpp"
#include "Label.hpp"
#include "lang.hpp"

namespace Stand
{
	Label ToggleCorrelation::getCondition(ToggleCorrelation::Type type)
	{
		switch (type)
		{
		case MENU_OPEN:
			return LOC("AUTOSTATE_H_OPN");

		case ON_FOOT:
			return LOC("AUTOSTATE_H_OFT");

		case AIMING:
			return LOC("AUTOSTATE_H_AIM");

		case FREEROAM:
			return LOC("AUTOSTATE_H_FRE");

		case CHATTING:
			return LOC("AUTOSTATE_H_CHT");

		case SESSION_HOST:
			return LOC("AUTOSTATE_H_HST");
		}
		return NOLABEL;
	}

	Label ToggleCorrelation::getCondition() const
	{
		return getCondition(type);
	}

	bool ToggleCorrelation::getCurrentValue(ToggleCorrelation::Type correlation)
	{
		bool value = false;
		switch (correlation)
		{
		case ToggleCorrelation::MENU_OPEN:
			value = g_gui.opened;
			break;

		case ToggleCorrelation::ON_FOOT:
			value = !g_player_ped.isInAnyVehicle();
			break;

		case ToggleCorrelation::AIMING:
			value = g_player_ped.isAiming();
			break;

		case ToggleCorrelation::FREEROAM:
			value = is_session_freeroam();
			break;

		case ToggleCorrelation::SESSION_HOST:
			value = g_player.isHost();
			break;
		}
		return value;
	}

	bool ToggleCorrelation::getCurrentValue(ToggleCorrelation::Type correlation, bool correlation_invert)
	{
		bool value = getCurrentValue(correlation);
		if (correlation_invert)
		{
			value = !value;
		}
		return value;
	}

	bool ToggleCorrelation::getCurrentValue() const
	{
		return getCurrentValue(type, invert);
	}

	bool ToggleCorrelation::isActive() const
	{
		return type != NONE;
	}

	std::string ToggleCorrelation::getState() const
	{
		return std::string(invert ? "Off" : "On").append(" while ").append(getCondition().getEnglishUtf8());
	}

	std::string ToggleCorrelation::getExplanation() const
	{
		if (isActive())
		{
			return LANG_FMT(
				"AUTOSTATE_H",
				FMT_ARG("command", g_gui.getCurrentMenuFocus()->getPhysical()->menu_name.getLocalisedUtf8()),
				FMT_ARG("state", Lang::get(invert ? ATSTRINGHASH("AUTOSTATE_H_OFF") : ATSTRINGHASH("AUTOSTATE_H_ON"))),
				FMT_ARG("condition", getCondition().getLocalisedUtf8())
			);
		}
		return {};
	}
}
