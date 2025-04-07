#pragma once

#include "CommandToggle.hpp"

#include <stack>

#include "natives_decl.hpp"
#include "vector.hpp"

namespace Stand
{
	struct MarkerBliponly
	{
		const v3 pos{};
		Blip blip = 0;

		explicit MarkerBliponly(v3&& pos) noexcept
			: pos(std::move(pos))
		{
		}

		[[nodiscard]] bool hasBlip() const noexcept;
		void setBlip(bool toggle);
	};

	struct Marker : public MarkerBliponly
	{
		using Flags_t = uint8_t;

		enum Flags : Flags_t
		{
			NONE = 0,

			NO_BLIP = 1 << 0,
			SP_ONLY = 1 << 1,
			PROXIMITY_TRIGGERS_MP_WORLD_STATE = 1 << 2,
			NO_MARKER = 1 << 3,
			PROXIMITY_IS_CONTEXT = 1 << 4,
		};

		const float heading{};
		Marker* const exit = nullptr;
		const Label name = NOLABEL;
		const Flags_t flags;

		explicit Marker(v3&& pos, float&& heading, Label&& name, Flags_t&& flags = NONE) noexcept
			: MarkerBliponly(std::move(pos)), heading(std::move(heading)), name(std::move(name)), flags(std::move(flags))
		{
		}

		explicit Marker(v3&& pos, float&& heading, Marker* const exit, Flags_t&& flags = NONE) noexcept
			: MarkerBliponly(std::move(pos)), heading(std::move(heading)), exit(exit), flags(std::move(flags))
		{
		}

		virtual void onEnter() const;
		virtual void onLeave() const;
		virtual void onTick() const;

		[[nodiscard]] bool isEntrance() const noexcept;
		[[nodiscard]] const Label& getName() const noexcept;

		[[nodiscard]] Marker* findEntrance() const noexcept;

		[[nodiscard]] v3 getRot() const noexcept;
		void draw() const;
		void teleportAndWait() const;
		void postTpMove() const;
	};

	class CommandWorldDoors : public CommandToggle
	{
	public:
		inline static CommandWorldDoors* instance;

		std::stack<Marker*> interiors{};
		CommandToggle* blips;
		bool online = false;

		explicit CommandWorldDoors(CommandList* parent);

		[[nodiscard]] bool isMarkerActive(const Marker* marker) const;

		[[nodiscard]] bool isInInterior() const noexcept;
		[[nodiscard]] Marker* getCurrentInterior() const noexcept;
		void forgetCurrentInterior() noexcept;

		void setBliponly(bool toggle) const;
		void enableEntranceBlips() const;
		void disableAllBlips() const;
		void updateBlips() const;

		void onEnable(Click& click) final;
	};
}
