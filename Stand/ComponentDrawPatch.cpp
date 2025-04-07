#include "ComponentDrawPatch.hpp"

#include "dlDrawListMgr.hpp"
#include "eEntityType.hpp"
#include "gta_entity.hpp"
#include "Renderer.hpp"

#include "ComponentImpl.hpp"

namespace Stand
{
	static CDrawListMgr** draw_list_mgr{}; // rage::gDrawListMgr

	static DetourHook CDynamicEntityDrawHandler_AddSkinnedToDrawList_hook{ "V7" };
	static DetourHook CDynamicEntityDrawHandler_AddFragmentToDrawList_hook{ "V8" };
	static DetourHook CEntityDrawHandler_AddBasicToDrawList_hook{ "V9" };
	static DetourHook CPedDrawHandler_AddToDrawList_hook{ "VA" };

	static constexpr auto MAX_DRAWS = 400; // 420 is too much

	static void* __fastcall CDynamicEntityDrawHandler_AddSkinnedToDrawList(void* CDynamicEntityDrawHandler, rage::fwEntity* entity, void* fwDrawDataAddParams)
	{
		SOUP_IF_UNLIKELY ((*draw_list_mgr)->free_slot >= MAX_DRAWS)
		{
			if (g_comp_drawpatch.enabled)
			{
				g_renderer.throttling_rendering = true;
				return nullptr;
			}
		}
		if (g_comp_drawpatch.block_all)
		{
			return nullptr;
		}
		return COMP_OG(CDynamicEntityDrawHandler_AddSkinnedToDrawList)(CDynamicEntityDrawHandler, entity, fwDrawDataAddParams);
	}

	// Vehicles
	static void* __fastcall CDynamicEntityDrawHandler_AddFragmentToDrawList(void* CDynamicEntityDrawHandler, rage::fwEntity* entity, void* fwDrawDataAddParams)
	{
		SOUP_IF_UNLIKELY ((*draw_list_mgr)->free_slot >= MAX_DRAWS)
		{
			if (g_comp_drawpatch.enabled)
			{
				g_renderer.throttling_rendering = true;
				return nullptr;
			}
		}
		if (g_comp_drawpatch.block_all)
		{
			return nullptr;
		}
		return COMP_OG(CDynamicEntityDrawHandler_AddFragmentToDrawList)(CDynamicEntityDrawHandler, entity, fwDrawDataAddParams);
	}

	// Objects
	static void* __fastcall CEntityDrawHandler_AddBasicToDrawList(void* CEntityDrawHandler, rage::fwEntity* entity, void* fwDrawDataAddParams)
	{
		SOUP_IF_UNLIKELY ((*draw_list_mgr)->free_slot >= MAX_DRAWS)
		{
			if (g_comp_drawpatch.enabled
				&& entity->type != ENTITY_TYPE_BUILDING
				)
			{
				g_renderer.throttling_rendering = true;
				return nullptr;
			}
		}
		if (g_comp_drawpatch.block_all)
		{
			return nullptr;
		}
		return COMP_OG(CEntityDrawHandler_AddBasicToDrawList)(CEntityDrawHandler, entity, fwDrawDataAddParams);
	}

	static void* __fastcall CPedDrawHandler_AddToDrawList(void* CPedDrawHandler, rage::fwEntity* entity, void* fwDrawDataAddParams)
	{
		SOUP_IF_UNLIKELY ((*draw_list_mgr)->free_slot >= MAX_DRAWS)
		{
			if (g_comp_drawpatch.enabled)
			{
				g_renderer.throttling_rendering = true;
				return nullptr;
			}
		}
		if (g_comp_drawpatch.block_all)
		{
			return nullptr;
		}
		return COMP_OG(CPedDrawHandler_AddToDrawList)(CPedDrawHandler, entity, fwDrawDataAddParams);
	}

	void ComponentDrawPatch::addPatterns(PatternBatch& batch)
	{
		BATCH_ADD("V7", "44 01 6A 08 4C 8B 05 ? ? ? ? 48 8B 05", [](soup::Pointer p)
		{
			draw_list_mgr = p.add(7).rip().as<CDrawListMgr**>();
		});
		BATCH_ADD("V8", "EB 35 F6 83 C0 00 00 00 40 74 17 33 C9 E8 ? ? ? ? 4C 8B C7 48 8B D3 48 8B CE E8 ? ? ? ? EB 15 33 C9 E8 ? ? ? ? 4C 8B C7 48 8B D3 48 8B CE E8", [](soup::Pointer p)
		{
			INIT_HOOK_2(CEntityDrawHandler_AddBasicToDrawList, p.sub(4).rip(), 0);
			INIT_HOOK_2(CDynamicEntityDrawHandler_AddFragmentToDrawList, p.add(28).rip(), 0);
			INIT_HOOK_2(CDynamicEntityDrawHandler_AddSkinnedToDrawList, p.add(51).rip(), 0);
		});
		BATCH_ADD_OPTIONAL_HOOK(CPedDrawHandler_AddToDrawList, "48 89 5C 24 08", "48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 81 EC 80 00 00 00 48 8B FA 48 8B 52");
	}

	std::vector<DetourHook*> ComponentDrawPatch::getHooks()
	{
		return {
			&CDynamicEntityDrawHandler_AddSkinnedToDrawList_hook,
			&CDynamicEntityDrawHandler_AddFragmentToDrawList_hook,
			&CEntityDrawHandler_AddBasicToDrawList_hook,
			&CPedDrawHandler_AddToDrawList_hook,
		};
	}
}
