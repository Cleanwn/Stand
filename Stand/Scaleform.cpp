#include "Scaleform.hpp"

namespace Stand
{
	bool Scaleform::ensureLoaded()
	{
		if (isLoaded())
		{
			return true;
		}
		inst = GRAPHICS::REQUEST_SCALEFORM_MOVIE(name);
		return GRAPHICS::HAS_SCALEFORM_MOVIE_LOADED(inst);
	}
}
