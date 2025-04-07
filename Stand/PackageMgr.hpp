#pragma once

#include <filesystem>
#include <string>
#include <unordered_set>
#include <vector>

#include <soup/fwd.hpp>
#include <soup/Promise.hpp>

#include "fwddecl.hpp"

#include "Package.hpp"
#include "PackageRepository.hpp"

namespace Stand
{
	class PackageMgr
	{
	public:
		inline static soup::Promise<> updating_completed;
		inline static PackageRepository repo;
		inline static std::vector<std::string> deps_manually_installed;
		inline static std::unordered_set<std::string> packages_reported_as_installed;

		[[nodiscard]] static std::wstring getInstalledPackagesFile();

		static void tunablesData(const soup::JsonObject& obj);
		static void tunablesFail();
	private:
		static void loadInstalledPackages();

	public:
		[[nodiscard]] static Package* find(const std::string& name);
		[[nodiscard]] static Package* findByFile(const std::filesystem::path& abs_path);

		static void autoRemoveDependencies();

		static void saveInstalledPackages();
	};
}
