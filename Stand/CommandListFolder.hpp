#pragma once

#include "CommandListRefreshable.hpp"

namespace Stand
{
	class CommandListFolder : public CommandListRefreshable
	{
	public:
		const std::wstring folder_name;
		const std::vector<std::wstring> ext;

	protected:
		explicit CommandListFolder(CommandList* const parent, std::wstring&& folder_name, std::vector<std::wstring>&& ext, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL);

	public:
		[[nodiscard]] std::wstring getFolderPath() const;
		[[nodiscard]] std::wstring getFilePath(const CommandListFolderItemData* item_data) const;

	protected:
		void populateHead() override;
		void createOpenFolderAction(Label&& help_text = NOLABEL);

	public:
		std::unordered_set<std::wstring> getListedItems() const;
	private:
		void recursivelyGetListedItems(std::unordered_set<std::wstring>& listed_items, const CommandList* list) const;

	public:
		[[nodiscard]] CommandList* findFolder(const std::string& name) const;

	protected:
		void populateBody() final;
	private:
		void recursivelyPopulateBody(CommandList* list, const std::unordered_set<std::wstring>& listed_items, const std::wstring& path, const std::wstring& rel_path) const;
	protected:
		virtual void postPopulateBody() const;

		void sortImpl() override;

		bool shouldDelete(Command* cmd) const final;

		template <typename T>
		[[nodiscard]] static CommandListFolderItemData* getItemDataImpl(Command* cmd) noexcept
		{
			return &((T*)cmd)->folder_item_data;
		}

		[[nodiscard]] virtual CommandListFolderItemData* getItemData(Command* cmd) const noexcept = 0;

		[[nodiscard]] virtual bool canShowFolder(const std::wstring& folder_rel_path) const;

		virtual void createItem(CommandList* list, std::wstring&& filename_no_ext, CommandListFolderItemData&& data) const = 0;
	};
}
