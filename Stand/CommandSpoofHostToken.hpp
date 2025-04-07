#pragma once

#include "CommandToggle.hpp"

#include "gta_fwddecl.hpp"

namespace Stand
{
	class CommandSpoofHostToken : public CommandToggle
	{
	private:
		inline static CommandSpoofHostToken* instance = nullptr;
		inline static bool change_queued = false;

	public:
		uint64_t spoofed_host_token = 0;

		explicit CommandSpoofHostToken(CommandList* const parent);

		void onChange(Click& click) final;

	private:
		void applyValue() const;
		static void changePeerId(const rage::netPeerId old_peerId, const rage::netPeerId peerId);
		static void changePeerId(const rage::netPeerId old_peerId, const rage::netPeerId peerId, rage::netPeerId* addr);
	};
}
