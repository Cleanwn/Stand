#pragma once

#include "Component.hpp"

#include "AbstractPlayer.hpp"
#include "ReactionExemption.hpp"
#include "ToastConfig.hpp"

namespace Stand
{
	struct ComponentNetcode : public Component
	{
		void addPatterns(PatternBatch& batch) final;
		[[nodiscard]] std::vector<DetourHook*> getHooks() final;

#ifdef STAND_DEBUG
		bool log_packet_receive = false;
#endif

#if HAVE_BREAKUP_KICK
		bool host_kick_karma = false;
#endif
		bool host_kick_protex = true;

#if HAVE_BREAKUP_KICK
		ToastConfig breakup_someoneelse_toasts{ true };
		ReactionExemption breakup_someoneelse_block = {};
		ReactionExemption breakup_someoneelse_karma = {};

		ToastConfig badbreakup_myself_toasts{ true };
		bool badbreakup_myself_karma = true;

		ToastConfig badbreakup_someoneelse_toasts{};
		ReactionExemption badbreakup_someoneelse_kick = {};
#endif

#if false
		ToastConfig buttplug_myself_toasts{ true };
		bool buttplug_myself_block = true;

		ToastConfig buttplug_someoneelse_toasts{};
		ReactionExemption buttplug_someoneelse_block = {};
#endif

#ifdef STAND_DEBUG
		bool trans_target_acquired = false;
		bool trans_target_lost = false;
#endif
	};
	inline ComponentNetcode g_comp_netcode;

	inline AbstractPlayer packet_src = AbstractPlayer::invalid();
}
