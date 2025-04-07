#pragma once

#include <cstdint>

namespace Stand
{
#pragma pack(push, 1)
	class tbToggleableBehaviour
	{
	private:
		uint8_t enables = 0;

	public:
		[[nodiscard]] bool isEnabled() const noexcept;

		void enable();
		void disable();

		void forceDisable();

	protected:
		virtual void onEnable();
		virtual void onDisable();
	};
#pragma pack(pop)
}
