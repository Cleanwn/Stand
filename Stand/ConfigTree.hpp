#pragma once

#include "Config.hpp"

#include <fstream>
#include <functional>

#include "fwddecl.hpp"

namespace Stand
{
	class ConfigTree : public Config
	{
	public:
		using Config::Config;

	protected:
		void loadImpl(std::unordered_map<std::string, std::string>&& data) final;
		virtual void loadImplTree(std::unordered_map<std::string, std::string>&& data) = 0;

		bool canSaveNow() const final;

	public:
		void saveToFileNow() final;
	protected:
		[[nodiscard]] virtual bool getCommandValue(const CommandPhysical* command, const std::string& path, std::string& outValue) const = 0;
	private:
		void saveImpl(std::string& file_out, const std::string& path_prefix, const std::string& file_prefix, const CommandList* list, std::function<void()>&& first_entry_func) const;
	};
}
