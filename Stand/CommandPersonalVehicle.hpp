#pragma once

#include "CommandWithEntityPreview.hpp"
#include "CommandList.hpp"

namespace Stand
{
	class CommandPersonalVehicle : public CommandWithEntityPreview<CommandList>
	{
	public:
		const uint32_t i;

		explicit CommandPersonalVehicle(CommandList* parent, const uint32_t i, const Hash model);
		
		void updateHelpText();

	protected:
		[[nodiscard]] Hash getModelHash() final;
		[[nodiscard]] Hash getModelHashConst() const;
		[[nodiscard]] AbstractEntity createPreviewEntity() final;

	private:
		void request(bool reuse_if_out = false);
		[[nodiscard]] AbstractEntity requestAndAwait(bool reuse_if_out = false);
	};
}
