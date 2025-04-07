#pragma once

#include <string>

#include "Codename.hpp"

namespace Stand
{
	enum Flags : uint8_t
	{
		DH_MANDATORY = 1 << 0,
		DH_NOFOLLOWJUMPS = 1 << 1,
		DH_LONGJUMP = 1 << 2,
#ifdef STAND_DEBUG
		DH_FORCEFAIL = 1 << 7,
#endif
	};

	class DetourHook
	{
	public:
		const Codename name;
		uint8_t flags = 0;
		void* detour = nullptr;
		void* target = nullptr;
		void* original = nullptr;
		void* effective_target = nullptr;

		[[deprecated("A hook was provided in Hooking.hpp, but not registered in Hooking.cpp. This may be intentional for testing, but should be resolved before dev/release.")]]
		DetourHook()
			: name("XX")
		{
		}

		explicit constexpr DetourHook(Codename&& name) // this needs call to .init later!
			: name(std::move(name))
		{
		}

		explicit constexpr DetourHook(Codename&& name, void* detour, uint8_t flags)
			: name(std::move(name)), detour(detour), flags(flags)
		{
		}

		DetourHook(DetourHook&& that) = delete;
		DetourHook& operator=(DetourHook&& that) = delete;
		DetourHook(DetourHook const&) = delete;
		DetourHook& operator=(DetourHook const&) = delete;

		void init(void* detour, void* target, uint8_t flags) noexcept
		{
			this->flags = flags;
			this->detour = detour;
			this->target = target;
		}

		void setTarget(void* target) noexcept
		{
			this->target = target;
		}

		template <typename T>
		[[nodiscard]] T getOriginal() const noexcept
		{
			return static_cast<T>(original);
		}

		[[nodiscard]] bool isValid() const noexcept;
		[[nodiscard]] bool isHooked() const noexcept;
		[[nodiscard]] bool isMandatory() const noexcept;
		[[nodiscard]] bool canFollowJumps() const noexcept;
		[[nodiscard]] bool isLongjump() const noexcept;

		void createHook();
	private:
		[[nodiscard]] const char* createHookDispatch();
		[[nodiscard]] const char* createHookMH();
		[[nodiscard]] const char* createHookLongjump();
		static void longjumpWriteTrampoline(void* addr, void* target);

	public:
		void enableHook();
		void enableHookQueued();
		void disableHook();
		void disableHookQueued();

		void removeHook();
	};
}
