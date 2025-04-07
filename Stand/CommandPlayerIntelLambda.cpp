#include "CommandPlayerIntelLambda.hpp"

#include "AbstractPlayer.hpp"
#include "CommandListPlayer.hpp"
#include "PlayerProvider.hpp"

namespace Stand
{
	std::wstring CommandPlayerIntelLambda::getValue(const AbstractPlayer p) const
	{
		return getValue_impl(p);
	}
}
