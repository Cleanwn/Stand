#pragma once

#include "CommandList.hpp"

#include <functional>

#include "CommandDivider.hpp"
#include "nullsub.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandListRefreshable : public CommandList
	{
	private:
		CommandDivider* divider{};
		size_t body_offset = 0;
		bool active = false;

	protected:
		explicit CommandListRefreshable(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL);

	public:
		void populate(); // for createAndPopulate

		void onClick(Click& click) override;
		void onActiveListUpdate() override;
		void refresh(std::function<void()>&& callback = &nullsub<>);
	protected:
		void processChild(CommandList* list, std::vector<std::unique_ptr<Command>>::iterator& i) const;
	private:
		void createDivider();
	public:
		void sort();
	private:
		virtual void sortImpl();
	protected:
		void sortBody(sort_algo_t algo);
		void sortBodyRecursively(sort_algo_t algo);
		static void recursivelySort(std::vector<std::unique_ptr<Command>>::iterator begin, std::vector<std::unique_ptr<Command>>::iterator end, sort_algo_t algo);

		[[nodiscard]] static bool isSubFolder(Command* cmd) noexcept;
	
		virtual void populateHead();
		virtual void populateBody() = 0;

		[[nodiscard]] virtual bool shouldDelete(Command* cmd) const;
		virtual void onItemIsKept(Command* cmd) const;

		[[nodiscard]] size_t getFullBodyOffset() const;
	};
#pragma pack(pop)
}
