#pragma once

namespace Stand
{
	class TunablesParser
	{
	public:
		[[nodiscard]] static bool canQuery();
		[[nodiscard]] static uint32_t queryOffset(hash_t hash);
	private:
		[[nodiscard]] static uint32_t queryOffset(rage::scrProgram* prog, hash_t hash);

		inline static bool need_cleanup = false;
	};
}
