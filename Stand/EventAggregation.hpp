#pragma once

#include <vector>

#include "fwddecl.hpp"
#include "typedecl.hpp"

#include "Codename.hpp"
#include "Label.hpp"

namespace Stand
{
#pragma pack(push, 1)
	struct EventAggregation
	{
		std::vector<flowevent_t> types{};
		Label name{};
		std::string extra_data{};

		EventAggregation(Label&& name);
		EventAggregation(flowevent_t event, Label name);
		EventAggregation(std::vector<flowevent_t>&& types, Label name);

		void add(flowevent_t type);
		void add(flowevent_t type, const Codename& extra_data);
		void addRaw(flowevent_t type, std::string extra_data);
		void addSelfIdentification(std::string menu_name);

		void setName(Label&& name);
		void setName(flowevent_t type, const Codename& extra_data);
		void setNameRaw(flowevent_t type, std::string extra_data);

		[[nodiscard]] Label getFullName() const;
		[[nodiscard]] floweventreaction_t getReactions(AbstractPlayer p) const noexcept;
		[[nodiscard]] flowevent_t getCanonicalType() const;
	};
#pragma pack(pop)
}
