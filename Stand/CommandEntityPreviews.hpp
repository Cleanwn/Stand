#pragma once

#include "CommandList.hpp"

#include "AbstractModel.hpp"
#include "CommandToggle.hpp"
#include "CommandSliderFloat.hpp"

namespace Stand
{
	class CommandEntityPreviews : public CommandList
	{
	public:
		static inline CommandEntityPreviews* instance = nullptr;

		CommandToggle* disabled;
		CommandToggle* opaque;
		CommandSliderFloat* rotation_speed;
		CommandSliderFloat* additional_ped_dist;
		CommandSliderFloat* additional_obj_dist;
		CommandSliderFloat* additional_veh_dist;

		explicit CommandEntityPreviews(CommandList* const parent)
			: CommandList(parent, LOC("ENTPREVS"))
		{
			instance = this;

			disabled = createChild<CommandToggle>(LOC("DENTPREVS"), CMDNAMES("disableentitypreviews", "disablentitypreviews", "disablepreviewentities"));
			opaque = createChild<CommandToggle>(LOC("OPQ"), CMDNAMES("previewopaque", "opaquepreviews"));
			rotation_speed = createChild<CommandSliderFloat>(LOC("ENTPREVROTSPD"), CMDNAMES("previewrotationspeed"), NOLABEL, 0, 400, 100, 5);
			additional_ped_dist = createChild<CommandSliderFloat>(LOC("ADDPREVDISTP"), CMDNAMES("previewpeddist"), NOLABEL, 0, 500, 0, 25);
			additional_obj_dist = createChild<CommandSliderFloat>(LOC("ADDPREVDISTO"), CMDNAMES("previewobjdist"), NOLABEL, 0, 1000, 0, 25);
			additional_veh_dist = createChild<CommandSliderFloat>(LOC("ADDPREVDISTV"), CMDNAMES("previewvehdist"), NOLABEL, 0, 1000, 0, 25);
		}

		~CommandEntityPreviews() noexcept
		{
			if (instance == this)
			{
				instance = nullptr;
			}
		}

		[[nodiscard]] static bool isDisabled() noexcept
		{
			SOUP_IF_LIKELY (instance)
			{
				return instance->disabled->m_on;
			}
			return false;
		}

		[[nodiscard]] static bool isOpaque() noexcept
		{
			SOUP_IF_LIKELY (instance)
			{
				return instance->opaque->m_on;
			}
			return false;
		}

		[[nodiscard]] static float getRotationSpeed()
		{
			SOUP_IF_LIKELY (instance)
			{
				return instance->rotation_speed->getFloatValue();
			}
			return 1.0f;
		}

		[[nodiscard]] static float getAdditionalOffset(const AbstractModel&& model)
		{
			SOUP_IF_UNLIKELY (instance == nullptr)
			{
				return 0.0f;
			}

			if (model.isPed())
			{
				return instance->additional_ped_dist->getFloatValue();
			}
			else if (model.isVehicle())
			{
				return instance->additional_veh_dist->getFloatValue();
			}
			else if (model.isObject())
			{
				return instance->additional_obj_dist->getFloatValue();
			}

#ifdef STAND_DEV
			SOUP_ASSERT_UNREACHABLE;
#else
			return 0.0f;
#endif
		}
	};
}