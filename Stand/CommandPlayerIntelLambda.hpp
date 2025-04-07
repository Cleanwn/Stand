#pragma once

#include "CommandPlayerIntel.hpp"

namespace Stand
{
	class CommandPlayerIntelLambda : public CommandPlayerIntel
	{
	private:
		std::function<std::wstring(const AbstractPlayer)> getValue_impl;

	public:
		explicit CommandPlayerIntelLambda(CommandList* const parent, Label&& label, std::function<std::wstring(const AbstractPlayer)>&& getValue_impl, const commandflags_t flags = CMDFLAGS_READONLY_VALUE_COPY)
			: CommandPlayerIntel(parent, std::move(label), flags), getValue_impl(std::move(getValue_impl))
		{
		}

		std::wstring getValue(const AbstractPlayer p) const final;
	};
}
