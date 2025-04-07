#include "PackageMgr.hpp"

#include <soup/os.hpp>

#include <fmt/format.h>

#include <soup/FileReader.hpp>
#include <soup/FileWriter.hpp>

#include "Auth.hpp"
#include "Codename.hpp"
#include "Exceptional.hpp"
#include "ExecCtx.hpp"
#include "get_appdata_path.hpp"
#include "Gui.hpp"
#include "HttpRequestBuilder.hpp"
#include "Label.hpp"
#include "lang.hpp"
#include "NetInterface.hpp"
#include "PackageInstallTask.hpp"
#include "str2int.hpp"
#include "StringUtils.hpp"
#include "ThreadContext.hpp"
#include "Util.hpp"

namespace Stand
{
	bool Package::operator==(const Package& b) const noexcept
	{
		return name == b.name;
	}

	bool Package::operator!=(const Package& b) const noexcept
	{
		return !operator==(b);
	}

	bool Package::isDependency() const noexcept
	{
		return type != STANDALONE;
	}

	bool Package::isInstalled() const noexcept
	{
		return !local.files.empty();
	}

	bool Package::existsOnRemote() const noexcept
	{
		return !remote.files.empty();
	}

	bool Package::isUpToDate() const noexcept
	{
		return remote.id == local.id;
	}

	bool Package::shouldShowToUser() const noexcept
	{
		return !hidden || isInstalled();
	}

	bool Package::dependsOn(const std::string& dep_name) const noexcept
	{
		return std::find(remote.dependencies.cbegin(), remote.dependencies.cend(), dep_name) != remote.dependencies.cend();
	}

	bool Package::isDependedOnByInstalledPackage() const noexcept
	{
		for (const auto& package : PackageMgr::repo.packages)
		{
			if (package.isInstalled() && package.dependsOn(this->name))
			{
				return true;
			}
		}
		return false;
	}

	std::string Package::getType() const
	{
		return name.substr(0, name.find('/'));
	}

	std::string Package::getName() const
	{
		return name.substr(name.find('/') + 1);
	}

	Label Package::getMenuName() const
	{
		auto name = getName();
		if (!existsOnRemote())
		{
			name.insert(0, "[NOT ON REMOTE] ");
		}
		return LIT(std::move(name));
	}

	bool Package::isLua() const noexcept
	{
		return name.substr(0, 4) == "lua/";
	}

	bool Package::isLuaCompatLayer() const noexcept
	{
		return name.substr(0, 17) == "lua_compat_layer/";
	}

	std::filesystem::path Package::getFilePath(const std::string& rel) const
	{
		std::wstring path = get_appdata_path();
		if (isLua() || isLuaCompatLayer())
		{
			path.append(L"Lua Scripts\\");
		}
		else
		{
			SOUP_ASSERT_UNREACHABLE;
		}
		path.append(StringUtils::utf8_to_utf16(rel));
		return path;
	}

	bool Package::doAllLocalFilesExist() const
	{
		for (const auto& file : local.files)
		{
			if (!std::filesystem::exists(getFilePath(file)))
			{
				return false;
			}
		}
		return true;
	}

	void Package::setBusyForInstall()
	{
		status = INSTALLING;
	}

	void Package::install()
	{
		setBusyForInstall();

		for (const auto& dep : remote.dependencies)
		{
			auto pkg = PackageMgr::find(dep);
			if (!pkg->isInstalled())
			{
				pkg->install();
			}
		}

		SOUP_ASSERT(existsOnRemote(), "Can't install local-only package");

		local.id = remote.id;
		local.files.reserve(remote.files.size());

		auto task = g_net_interface.add<PackageInstallTask>(*this);
		while (!task->isWorkDone())
		{
			ExecCtx::get().yield();
		}

		status = NONE;

		SOUP_ASSERT(task->fi == remote.files.size(), "Install task finished abnormally");
		SOUP_ASSERT(isInstalled(), "Package is not installed after install task finished");

		if (!PackageMgr::packages_reported_as_installed.contains(this->name))
		{
			PackageMgr::packages_reported_as_installed.emplace(this->name);
			g_auth.reportEvent("PI", this->name);
		}
	}

	void Package::uninstall(bool keep_trash)
	{
		deleteFiles(local.files);
		local.id.clear();
		local.files.clear();
		if (!keep_trash)
		{
			deleteFiles(remote.trash);
		}
	}

	void Package::deleteFiles(const std::vector<std::string>& files) const
	{
		std::error_code ec{};
		for (const auto& file : files)
		{
			bool used_by_another_package = false;
			for (auto& package : PackageMgr::repo.packages)
			{
				if (package.isInstalled() && *this != package)
				{
					for (const auto& file2 : package.local.files)
					{
						if (file == file2)
						{
							used_by_another_package = true;
							break;
						}
					}
				}
				if (used_by_another_package)
				{
					break;
				}
			}
			if (used_by_another_package)
			{
				continue;
			}

			auto path = getFilePath(file);
			if (std::filesystem::is_regular_file(path))
			{
				std::filesystem::remove(path, ec);
				if (file.find('/') != std::string::npos)
				{
					deleteEmptyUpwards(path.parent_path());
				}
			}
			else if (std::filesystem::is_directory(path))
			{
				deleteEmptyUpwards(path);
			}
		}
	}

	void Package::deleteEmptyUpwards(std::filesystem::path path)
	{
		std::error_code ec{};
		while (std::filesystem::is_empty(path, ec))
		{
			std::filesystem::remove(path, ec);
			path = path.parent_path();
		}
	}

	bool Package::isBusy() const noexcept
	{
		return status != NONE;
	}

	void Package::userInstall()
	{
		install();
		if (isDependency())
		{
			PackageMgr::deps_manually_installed.emplace_back(name);
		}
		PackageMgr::saveInstalledPackages();
	}

	std::wstring PackageMgr::getInstalledPackagesFile()
	{
		return get_appdata_path("Installed Packages.bin");
	}

	void PackageMgr::tunablesData(const soup::JsonObject& obj)
	{
		loadInstalledPackages();
		repo.fromJson(obj);
		repo.sortPackages();
		Exceptional::createManagedExceptionalThread(__FUNCTION__, []
		{
			// If we were to update sooner, some scripts might be missing for populate & load state.
			while (!g_gui.worker_thread_running)
			{
				if (g_gui.isUnloadPending())
				{
					return;
				}
				soup::os::sleep(50);
			}

			bool any_updates = false;
			for (auto& pkg : repo.packages)
			{
				if (!pkg.isInstalled())
				{
					continue;
				}

				packages_reported_as_installed.emplace(pkg.name);

				if (pkg.existsOnRemote())
				{
					bool should_update = false;
					if (!pkg.isUpToDate())
					{
						should_update = true;
#ifndef STAND_DEBUG
						if (g_gui.meta_state.data.contains(soup::ObfusString("Packages Source")))
#endif
						{
							Util::toast(fmt::format(fmt::runtime(soup::ObfusString("Updating package {} from {} to {}").str()), pkg.name, pkg.local.id, pkg.remote.id));
						}
					}
					else if (!pkg.doAllLocalFilesExist())
					{
						should_update = true;
						Util::toast(LANG_FMT("REPO_MISNGLCL", pkg.name));
					}
					if (should_update)
					{
						pkg.status = Package::UPDATING;
						pkg.uninstall(true);
						pkg.install();
						any_updates = true;
					}
				}
			}
			if (any_updates)
			{
				autoRemoveDependencies();
				saveInstalledPackages();
			}

			updating_completed.fulfil();
		});
	}

	void PackageMgr::tunablesFail()
	{
		loadInstalledPackages();
		repo.sortPackages();
		updating_completed.fulfil();
	}

	void PackageMgr::loadInstalledPackages()
	{
		soup::FileReader in(getInstalledPackagesFile());
		if (in.hasMore()
			&& in.vec_str_nt_u64_dyn(deps_manually_installed)
			)
		{
			while (in.hasMore())
			{
				Package pkg{};
				uint64_t num_files;
				if (!in.str_nt(pkg.name)
					|| !in.str_nt(pkg.local.id)
					|| !in.u64_dyn(num_files)
					)
				{
					break;
				}
				while (num_files-- > 0)
				{
					std::string fn;
					if (!in.str_nt(fn))
					{
						break;
					}
					if (fn.empty())
					{
						continue;
					}
					if (fn.substr(0, 4) != "EXT:")
					{
						pkg.local.files.emplace_back(std::move(fn));
						continue;
					}
					if (fn.substr(4, 4) == "TYP:")
					{
						pkg.type = static_cast<Package::Type>(str2int<uint8_t>(fn.substr(8)).value());
					}
					else if (fn.substr(4, 4) == "FLG:")
					{
						auto flags = str2int<uint8_t>(fn.substr(8)).value();
						if ((flags >> 1) & 1)
						{
							pkg.type = Package::LIBRARY;
						}
					}
				}
				if (pkg.type == Package::STANDALONE
					&& pkg.local.id.empty()
					)
				{
					pkg.type = Package::LIBRARY;
				}
				repo.packages.emplace_back(std::move(pkg));
			}
		}
	}

	Package* PackageMgr::find(const std::string& name)
	{
		return repo.find(name);
	}

	Package* PackageMgr::findByFile(const std::filesystem::path& abs_path)
	{
		return repo.findByFile(abs_path);
	}

	void PackageMgr::autoRemoveDependencies()
	{
		bool uninstalled_any = false;
		for (auto& package : repo.packages)
		{
			if (package.isInstalled()
				&& package.isDependency()
				&& std::find(deps_manually_installed.cbegin(), deps_manually_installed.cend(), package.name) == deps_manually_installed.cend()
				&& !package.isDependedOnByInstalledPackage()
				)
			{
				package.uninstall();
				uninstalled_any = true;
			}
		}
		if (uninstalled_any)
		{
			autoRemoveDependencies();
		}
	}

	void PackageMgr::saveInstalledPackages()
	{
		soup::FileWriter of(getInstalledPackagesFile());

		for (auto i = deps_manually_installed.begin(); i != deps_manually_installed.end(); )
		{
			if (find(*i) == nullptr)
			{
				deps_manually_installed.erase(i);
			}
			else
			{
				++i;
			}
		}

		if (of.vec_str_nt_u64_dyn(deps_manually_installed))
		{
			for (const auto& package : repo.packages)
			{
				if (package.isInstalled())
				{
					auto data = package.local.files;
					data.emplace_back(std::move(std::string("EXT:TYP:").append(fmt::to_string(package.type))));
					if (!of.str_nt(package.name)
						|| !of.str_nt(package.local.id)
						|| !of.vec_str_nt_u64_dyn(data)
						)
					{
						break;
					}
				}
			}
		}
	}
}
