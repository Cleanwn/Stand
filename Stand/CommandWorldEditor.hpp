#pragma once

#include "CommandList.hpp"

#include <soup/Vector3.hpp>

#include "natives_decl.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandSpawnedObject;

	class CommandWorldEditor final : public CommandList
	{
	private:
		size_t spawned_offset;
	public:
		bool active = false;
		bool freecam = true;

		bool spawnable_focused = false;
		float heading = 0.0f;

		CommandSpawnedObject* spawned_focus = nullptr;
		soup::Vector3 spawned_focus_hit;

		explicit CommandWorldEditor(CommandList* parent);

		~CommandWorldEditor() final;

		void onClick(Click& click) final;

		void onActiveListUpdate() final;
		static void enableFreecam();
	private:
		void setNotActive();

	public:
		void onSpawnableFocus() noexcept;
		void onSpawnableBlur() noexcept;
	};
#pragma pack(pop)
}
