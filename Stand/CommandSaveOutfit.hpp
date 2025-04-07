#pragma once

#include "CommandListSave.hpp"

namespace Stand
{
	class CommandSaveOutfit : public CommandListSave
	{
	public:
		explicit CommandSaveOutfit(CommandList* const parent);

		[[nodiscard]] std::wstring getFolder() const final;

		[[nodiscard]] std::unordered_map<std::string, std::string> getDataEn() const final;
	protected:
		[[nodiscard]] bool canEntryBeOmitted(hash_t key) const final;
	public:
		[[nodiscard]] std::vector<hash_t> getDataOrderRaw() const final;

		void onSaved() const final;
	};
}
