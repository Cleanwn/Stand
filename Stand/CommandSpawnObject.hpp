#pragma once

#include "CommandWithEntityPreview.hpp"
#include "CommandSpawnObjectBase.hpp"

#include "ButtonInstructions.hpp"
#include "CommandWorldEditor.hpp"
#include "gta_input.hpp"
#include "tbPositionSelection.hpp"

namespace Stand
{
	class CommandSpawnObject : public CommandWithEntityPreview<CommandSpawnObjectBase<CommandAction>>
	{
	public:
		explicit CommandSpawnObject(CommandList* parent, CommandList* spawned_objects, const char* model_name)
			: CommandWithEntityPreview(parent, spawned_objects, LIT(model_name), {}, NOLABEL, CMDFLAGS_ACTION | CMDFLAG_SEARCH_FINISHLIST | CMDFLAG_FEATURELIST_FINISHLIST | CMDFLAG_STATE_AND_CHECK_FINISHLIST)
		{
		}

		void onFocus(ThreadContext thread_context, Direction momentum) final
		{
			CommandWithEntityPreview<CommandSpawnObjectBase<CommandAction>>::onFocus(thread_context, momentum);

			resolveParent(COMMAND_LIST_CUSTOM_SPECIAL_MEANING)->as<CommandWorldEditor>()->onSpawnableFocus();
		}
		
		void onBlur(ThreadContext thread_context, Direction momentum) final
		{
			CommandWithEntityPreview<CommandSpawnObjectBase<CommandAction>>::onBlur(thread_context, momentum);

			resolveParent(COMMAND_LIST_CUSTOM_SPECIAL_MEANING)->as<CommandWorldEditor>()->onSpawnableBlur();
		}

		void onTickFocused() final
		{
			CommandWithEntityPreview<CommandSpawnObjectBase<CommandAction>>::onTickFocused();

			ButtonInstructions::feature_buttons |= ButtonInstructions::OBJECT_ROTATE;

			PAD::DISABLE_CONTROL_ACTION(2, INPUT_VEH_RADIO_WHEEL, TRUE);

			v3 rot = g_preview_ent.getRot();
			rot.z = resolveParent(COMMAND_LIST_CUSTOM_SPECIAL_MEANING)->as<CommandWorldEditor>()->heading;
			if (PAD::IS_DISABLED_CONTROL_PRESSED(2, INPUT_CONTEXT_SECONDARY)) // Q
			{
				rot.z += 1.0f;
				resolveParent(COMMAND_LIST_CUSTOM_SPECIAL_MEANING)->as<CommandWorldEditor>()->heading = rot.z;
			}
			if (PAD::IS_DISABLED_CONTROL_PRESSED(2, INPUT_CONTEXT)) // E
			{
				rot.z -= 1.0f;
				resolveParent(COMMAND_LIST_CUSTOM_SPECIAL_MEANING)->as<CommandWorldEditor>()->heading = rot.z;
			}
			g_preview_ent.setRot(rot);
		}

		void onClick(Click& click) final
		{
			ensureYieldableScriptThread(click, [this]
			{
				if (g_preview_ent.isValid())
				{
					auto model_name = menu_name.getLiteralUtf8();
					auto model = AbstractModel(rage::atStringHash(model_name));
					auto ent = doSpawnStreamed(LIT(std::move(model_name)), model);
					ent.setPos(g_preview_ent.getPos());
					ent.setRot(g_preview_ent.getRot());
				}
			});
		}

		[[nodiscard]] bool isPreviewEnabled() const noexcept final
		{
			return true;
		}

		[[nodiscard]] Hash getModelHash() final
		{
			return rage::atStringHash(menu_name.getLiteralUtf8());
		}

		[[nodiscard]] AbstractEntity createPreviewEntity() final
		{
			return Util::createObject(getModelHash(), {}, false);
		}

		[[nodiscard]] static v3 calculate_snap_point(const v3& rot, const v3& dimensions, const v3& dir_vec)
		{
			return (dimensions * dir_vec).rotate(rot);
		}

		static void populate_snap_point(const v3& origin, const v3& rot, const v3& dimensions, std::pair<v3, v3>& out, const v3& dir_vec)
		{
			out = { origin + calculate_snap_point(rot, dimensions, dir_vec), dir_vec };
		}

		v3 getEntityOrigin(AbstractEntity& ent) const final
		{
			const auto editor = resolveParent(COMMAND_LIST_CUSTOM_SPECIAL_MEANING)->as<CommandWorldEditor>();

			v3 pos;
			if (editor->spawned_focus != nullptr)
			{
				auto focus_ent = AbstractEntity::get(resolveParent(COMMAND_LIST_CUSTOM_SPECIAL_MEANING)->as<CommandWorldEditor>()->spawned_focus->handle);
				const auto ent_origin = focus_ent.getOrigin();
				const auto ent_rot = focus_ent.getRot();
				const auto ent_dimensions = focus_ent.getDimensions();
				std::pair<v3, v3> snap_points[6];
				populate_snap_point(ent_origin, ent_rot, ent_dimensions, snap_points[0], v3{  0.5f,  0.0f, 0.0f });
				populate_snap_point(ent_origin, ent_rot, ent_dimensions, snap_points[1], v3{ -0.5f,  0.0f, 0.0f });
				populate_snap_point(ent_origin, ent_rot, ent_dimensions, snap_points[2], v3{  0.0f,  0.5f, 0.0f });
				populate_snap_point(ent_origin, ent_rot, ent_dimensions, snap_points[3], v3{  0.0f, -0.5f, 0.0f });
				snap_points[4] = { ent_origin + (ent_dimensions * v3{ 0.0f, 0.0f,  0.5f }), v3{ 0.0f, 0.0f,  0.5f } };
				snap_points[5] = { ent_origin + (ent_dimensions * v3{ 0.0f, 0.0f, -0.5f }), v3{ 0.0f, 0.0f, -0.5f } };
				std::pair<v3, v3> nearest_point;
				float nearest_dist = FLT_MAX;
				for (auto& point : snap_points)
				{
					//g_renderer.draw3dTextThisTickH(point.first, L"X", 1.0f);
					auto dist = editor->spawned_focus_hit.distance(point.first);
					if (dist < nearest_dist)
					{
						nearest_point = std::move(point);
						nearest_dist = dist;
					}
				}
				//g_renderer.draw3dTextThisTickH(nearest_point.first, L"X", 1.0f);
				pos = nearest_point.first + calculate_snap_point(ent_rot, ent.getDimensions(), nearest_point.second);
			}
			else
			{
				pos = g_tb_position_selection.pos;
				pos.z += (ent.getDimensions().z * 0.5f);
			}
			return pos;
		}
	};
}
