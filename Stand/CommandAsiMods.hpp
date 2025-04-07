#pragma once

#include "CommandListRefreshable.hpp"

#include "AsiMod.hpp"

namespace Stand
{
	class CommandAsiMods : public CommandListRefreshable
	{
	public:
		inline static CommandAsiMods* instance;

		explicit CommandAsiMods(CommandList* parent);

	protected:
		void populateHead() final;
		void populateBody() final;
	private:
		void processAsiDirectory(const std::wstring& path, const AsiDirectory dir);
		[[nodiscard]] bool hasAsiMod(const std::string& name, const AsiDirectory dir) const;
	public:
		[[nodiscard]] std::vector<AsiMod*> getAllAsiMods() const;
		[[nodiscard]] AsiMod* getAsiModByModule(HANDLE hmod) const;

	protected:
		bool shouldDelete(Command* cmd) const final;
	};
}
