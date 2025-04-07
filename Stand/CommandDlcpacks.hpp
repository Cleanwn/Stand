#pragma once

#include "CommandListFolder.hpp"

#include "CommandListSelectVehicleModel.hpp"

namespace Stand
{
	class CommandDlcpacks : public CommandListFolder
	{
	private:
		static inline std::unordered_map<VehicleType, CommandListSelectVehicleModel*> model_spoofs;

	public:
		explicit CommandDlcpacks(CommandList* const parent);

		void onClick(Click& click) final;

		[[nodiscard]] static std::optional<Hash> getSpoofForVehicleType(VehicleType type);

	protected:
		void populateHead() final;

		[[nodiscard]] CommandListFolderItemData* getItemData(Command* cmd) const noexcept final;

		void createItem(CommandList* list, std::wstring&& filename_no_ext, CommandListFolderItemData&& data) const final;
	};
}
