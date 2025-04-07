#pragma once

#include "CommandListRefreshable.hpp"

#include "CommandListSelect.hpp"

namespace Stand
{
	class CommandPersonalVehicles final : public CommandListRefreshable
	{
	public:
		inline static CommandPersonalVehicles* instance = nullptr;

	private:
		bool is_active = false;
		CommandListSelect* sort_algo_select = nullptr;

	public:
		explicit CommandPersonalVehicles(CommandList* parent);
		~CommandPersonalVehicles() final;

		void onActiveListUpdate() final;

		void onClick(Click& click) final;

	private:
		static void forEachPersonalVehicle(const std::function<void(uint32_t, ScriptGlobal&&)>& consumer);

	protected:
		void populateHead() final;
		void populateBody() final;

		void sortImpl() final;
	public:
		static bool sortAlgoSlot(const std::unique_ptr<Command>& a, const std::unique_ptr<Command>& b);
	};
}
