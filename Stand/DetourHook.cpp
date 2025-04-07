#include "DetourHook.hpp"

#include <fmt/core.h>
#include <MinHook.h>

#include <soup/ObfusString.hpp>
#include <soup/Pointer.hpp>
#include <soup/x64.hpp>

#include "ColoadMgr.hpp"
#ifdef STAND_DEV
#include "FileLogger.hpp"
#endif
#include "StringUtils.hpp"

namespace Stand
{
	bool DetourHook::isValid() const noexcept
	{
		return target != nullptr;
	}

	bool DetourHook::isHooked() const noexcept
	{
		return effective_target != nullptr;
	}

	bool DetourHook::isMandatory() const noexcept
	{
		return (flags & DH_MANDATORY) != 0;
	}

	bool DetourHook::canFollowJumps() const noexcept
	{
		return (flags & DH_NOFOLLOWJUMPS) == 0;
	}

	bool DetourHook::isLongjump() const noexcept
	{
		return (flags & DH_LONGJUMP) != 0;
	}

	void DetourHook::createHook()
	{
		if (!isValid() && !isMandatory())
		{
#ifdef STAND_DEBUG
			g_logger.log(fmt::format("Not creating hook {} as it is invalid and optional.", name.toString()));
#endif
			return;
		}
		if (isHooked())
		{
#ifdef STAND_DEBUG
			g_logger.log(fmt::format("Not creating hook {} as it is already created.", name.toString()));
#endif
			return;
		}
		std::vector<soup::Pointer> targets;
		if (canFollowJumps())
		{
			targets = soup::Pointer(target).getJumps();
			if (targets.size() != 1
				&& isLongjump()
				)
			{
				// Longjump hook requires the target function to be vetted first to avoid register corruption.
				// Given that there are hooks in the way, whatever vetting we did is now invalid.
				// Fall back to using MinHook.
				flags &= ~DH_LONGJUMP;
#ifdef STAND_DEBUG
				g_logger.log(fmt::format("{} is already hooked, using MinHook instead of longjump.", name.toString()));
#endif
			}
		}
		else
		{
			targets = { target };
		}
		std::string error{};
	_retry_hook:
		for (auto it = targets.crbegin(); it != targets.crend(); ++it)
		{
			effective_target = it->as<void*>();
			const char* res = createHookDispatch();
			if (res == nullptr)
			{
				break;
			}
			effective_target = nullptr;
			StringUtils::list_append_nl(error, fmt::format(fmt::runtime(soup::ObfusString("Failed to create hook {} at 0x{:X}: {}").str()), name.toString(), it->as<uintptr_t>(), res));
		}
		SOUP_IF_UNLIKELY (!isHooked())
		{
			if (isLongjump())
			{
				// Failed to hook with longjump, try again with MinHook.
				flags &= ~DH_LONGJUMP;
				goto _retry_hook;
			}
			if (isMandatory())
			{
				throw std::runtime_error(std::move(error));
			}
#ifdef STAND_DEV
			g_logger.log(std::move(error));
#endif
		}
	}

	const char* DetourHook::createHookDispatch()
	{
#ifdef STAND_DEBUG
		SOUP_IF_UNLIKELY (flags & DH_FORCEFAIL)
		{
			return "DH_FORCEFAIL";
		}
#endif
		if (isLongjump())
		{
			return createHookLongjump();
		}
		return createHookMH();
	}

	const char* DetourHook::createHookMH()
	{
		MH_STATUS status = MH_CreateHook(effective_target, detour, &original);
		if (status == MH_OK)
		{
			return nullptr;
		}
		return MH_StatusToString(status);
	}

	static uint8_t longjump_trampoline[] = {
		0x49, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // movabs r10, (8 bytes)
		0x41, 0xff, 0xe2, // jmp r10
	};

	//static uint8_t longjump_trampoline[] = {
	//	0x49, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // movabs r11, (8 bytes)
	//	0x41, 0xff, 0xe3, // jmp r11
	//};

	const char* DetourHook::createHookLongjump()
	{
		if (effective_target == nullptr)
		{
			return "Invalid address";
		}
		auto op = (const uint8_t*)effective_target;
		uint8_t og_bytes = 0;
		do
		{
			auto op_start = op;
			auto ins = soup::x64Disasm(op);
			if (!ins.isValid())
			{
				return "Unsupported instruction";
			}
			og_bytes += (uint8_t)(op - op_start);
			for (const auto& opr : ins.operands)
			{
				if (opr.reg == soup::IP
					|| opr.reg == soup::DIS
					)
				{
					return "Instruction interacts with instruction pointer";
				}
			}
		} while (og_bytes < sizeof(longjump_trampoline));

		// create original stub
		original = malloc(og_bytes + sizeof(longjump_trampoline));
		DWORD oldprotect;
		VirtualProtect(original, og_bytes + sizeof(longjump_trampoline), PAGE_EXECUTE_READWRITE, &oldprotect);
		memcpy(original, effective_target, og_bytes);
		longjumpWriteTrampoline((uint8_t*)original + og_bytes, (uint8_t*)effective_target + og_bytes);

		return nullptr;
	}

	void DetourHook::longjumpWriteTrampoline(void* addr, void* target)
	{
		*(void**)(longjump_trampoline + 2) = target;
		memcpy(addr, longjump_trampoline, sizeof(longjump_trampoline));
	}

	void DetourHook::enableHook()
	{
		if (isHooked())
		{
			if (isLongjump())
			{
				longjumpWriteTrampoline(effective_target, detour);
			}
			else
			{
				MH_EnableHook(effective_target);
			}
		}
		else
		{
			ColoadMgr::coloading_with_any_menu = true;

#ifdef STAND_DEBUG
			g_logger.log(fmt::format("Not enabling hook {} as it is not created.", name.toString()));
#endif
		}
	}

	void DetourHook::enableHookQueued()
	{
		if (isHooked())
		{
			if (isLongjump())
			{
				enableHook();
			}
			else
			{
				MH_QueueEnableHook(effective_target);
			}
		}
#ifdef STAND_DEBUG
		else
		{
			g_logger.log(fmt::format("Not enabling hook {} as it is not created.", name.toString()));
		}
#endif
	}

	void DetourHook::disableHook()
	{
		if (isHooked())
		{
			if (isLongjump())
			{
				memcpy(effective_target, original, sizeof(longjump_trampoline));
			}
			else
			{
				MH_DisableHook(effective_target);
			}
		}
	}

	void DetourHook::removeHook()
	{
		if (isHooked())
		{
			if (isLongjump())
			{
				if (original != nullptr)
				{
					free(original);
					original = nullptr;
				}
			}
			else
			{
				MH_RemoveHook(effective_target);
			}
			effective_target = nullptr;
		}
	}

	void DetourHook::disableHookQueued()
	{
		if (isHooked())
		{
			if (isLongjump())
			{
				disableHook();
			}
			else
			{
				MH_QueueDisableHook(effective_target);
			}
		}
	}
}
