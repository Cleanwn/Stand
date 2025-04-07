#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace Stand
{
	struct PackageVersion
	{
		std::string id;
	};

	struct PackageVersionRemote : public PackageVersion
	{
		std::vector<std::string> dependencies{};
		std::vector<std::pair<std::string, std::string>> files{};
		std::vector<std::string> trash{};
	};

	struct PackageVersionLocal : public PackageVersion
	{
		std::vector<std::string> files{};
	};

	struct Package
	{
		enum Type : uint8_t
		{
			STANDALONE,
			LIBRARY,
		};

		enum Status : uint8_t
		{
			NONE,
			INSTALLING,
			UPDATING,
		};

		std::string name;
		std::string description{};
		std::string author{};
		int8_t priority = 0;
		PackageVersionRemote remote{};
		PackageVersionLocal local{};
		Type type = STANDALONE;
		Status status = NONE;
		bool hidden = false; // can be used to prevent new installs but still provide updates

		[[nodiscard]] bool operator==(const Package& b) const noexcept;
		[[nodiscard]] bool operator!=(const Package& b) const noexcept;

		[[nodiscard]] bool isDependency() const noexcept;
		[[nodiscard]] bool isInstalled() const noexcept;
		[[nodiscard]] bool existsOnRemote() const noexcept;
		[[nodiscard]] bool isUpToDate() const noexcept;
		[[nodiscard]] bool shouldShowToUser() const noexcept;

		[[nodiscard]] bool dependsOn(const std::string& dep_name) const noexcept;
		[[nodiscard]] bool isDependedOnByInstalledPackage() const noexcept;

		[[nodiscard]] std::string getType() const;
		[[nodiscard]] std::string getName() const;
		[[nodiscard]] Label getMenuName() const;

		[[nodiscard]] bool isLua() const noexcept;
		[[nodiscard]] bool isLuaCompatLayer() const noexcept;

		[[nodiscard]] std::filesystem::path getFilePath(const std::string& rel) const;
		[[nodiscard]] bool doAllLocalFilesExist() const;

		void setBusyForInstall();
		void install();
		void uninstall(bool keep_trash = false);
	private:
		void deleteFiles(const std::vector<std::string>& files) const;
		static void deleteEmptyUpwards(std::filesystem::path path);

	public:
		[[nodiscard]] bool isBusy() const noexcept;

		void userInstall();
	};
}
