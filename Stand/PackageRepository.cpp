#include "PackageRepository.hpp"

#include <soup/JsonArray.hpp>
#include <soup/JsonBool.hpp>
#include <soup/JsonInt.hpp>
#include <soup/JsonObject.hpp>
#include <soup/JsonString.hpp>

#include "StringUtils.hpp"

namespace Stand
{
	void PackageRepository::fromJson(const soup::JsonObject& obj)
	{
		for (const auto& entry : obj)
		{
			std::string name = entry.first->asStr();
			Package* pkg = find(name);
			if (pkg == nullptr)
			{
				pkg = &packages.emplace_back(Package{ std::move(name) });
			}
			if (auto mem_description = entry.second->asObj().find("description"))
			{
				pkg->description = mem_description->asStr();
			}
			if (auto mem_author = entry.second->asObj().find("author"))
			{
				pkg->author = mem_author->asStr();
			}
			if (auto mem_priority = entry.second->asObj().find("priority"))
			{
				pkg->priority = (int8_t)mem_priority->asInt();
			}
			if (auto mem_version = entry.second->asObj().find("version"))
			{
				pkg->remote.id = mem_version->asStr();
			}
			if (auto mem_depends = entry.second->asObj().find("depends"))
			{
				auto& depends_arr = mem_depends->asArr();
				pkg->remote.dependencies.reserve(depends_arr.size());
				for (const auto& dep : depends_arr)
				{
					pkg->remote.dependencies.emplace_back(dep.asStr());
				}
			}
			for (const auto& file : entry.second->asObj().at("files").asObj())
			{
				pkg->remote.files.emplace_back(file.first->asStr(), file.second->asStr());
			}
			if (auto mem_trash = entry.second->asObj().find("trash"))
			{
				auto& trash_arr = mem_trash->asArr();
				pkg->remote.trash.reserve(trash_arr.size());
				for (const auto& file : trash_arr)
				{
					pkg->remote.trash.emplace_back(file.asStr());
				}
			}

			pkg->type = Package::STANDALONE;
			if (auto mem_type = entry.second->asObj().find("type")) // Replaced is_dependency in 114
			{
				switch (rage::atStringHash(mem_type->asStr()))
				{
				case ATSTRINGHASH("library"):
					pkg->type = Package::LIBRARY;
					break;

				default:
					// Handle unrecognised type:
					// - If "is_dependency" is set to true, use type "library" so it is added to the "Dependencies" pile in repository.
					// - If  is_dependency" is not set to true, use type "standalone" so it is added to the main pile in repository.
					if (auto mem_isdep = entry.second->asObj().find("is_dependency"))
					{
						if (mem_isdep->asBool())
						{
							pkg->type = Package::LIBRARY;
						}
					}
					break;
				}
			}
			else
			{
				if (pkg->remote.id.empty())
				{
					pkg->type = Package::LIBRARY;
					//Util::toast(fmt::format("{} still uses id.empty()", entry.first->encode()), TOAST_ALL);
				}
			}

			if (auto mem_hidden = entry.second->asObj().find("hidden"))
			{
				pkg->hidden = mem_hidden->asBool();
			}
		}
	}

	void PackageRepository::sortPackages()
	{
		std::sort(packages.begin(), packages.end(), [](const Package& a, const Package& b)
		{
			if (a.priority != b.priority)
			{
				return a.priority > b.priority;
			}
			auto a_name = StringUtils::simplify(a.name);
			auto b_name = StringUtils::simplify(b.name);
			return a_name < b_name;
		});
	}

	Package* PackageRepository::find(const std::string& name)
	{
		for (auto& package : packages)
		{
			if (package.name == name)
			{
				return &package;
			}
		}
		return nullptr;
	}

	Package* PackageRepository::findByFile(const std::filesystem::path& abs_path)
	{
		for (auto& package : packages)
		{
			for (const auto& file : package.local.files)
			{
				if (package.getFilePath(file) == abs_path)
				{
					return &package;
				}
			}
		}
		return nullptr;
	}
}
