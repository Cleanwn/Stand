#include "tbToggleableBehaviour.hpp"

namespace Stand
{
	bool tbToggleableBehaviour::isEnabled() const noexcept
	{
		return enables != 0;
	}

	void tbToggleableBehaviour::enable()
	{
		if (enables++ == 0)
		{
			onEnable();
		}
	}

	void tbToggleableBehaviour::disable()
	{
		if (--enables == 0)
		{
			onDisable();
		}
	}

	void tbToggleableBehaviour::forceDisable()
	{
		if (isEnabled())
		{
			enables = 1;
			disable();
		}
	}

	void tbToggleableBehaviour::onEnable()
	{
	}

	void tbToggleableBehaviour::onDisable()
	{
	}
}
