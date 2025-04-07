#pragma once

#include <soup/fwd.hpp>

#include "Package.hpp"

namespace Stand
{
	struct PackageRepository
	{
		std::vector<Package> packages;

		void fromJson(const soup::JsonObject& obj);
		void sortPackages();

		[[nodiscard]] Package* find(const std::string& name);
		[[nodiscard]] Package* findByFile(const std::filesystem::path& abs_path);
	};
}
