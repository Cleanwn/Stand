#include "PackageInstallTask.hpp"

#include <fstream>

#include <fmt/core.h>

#include <soup/Regex.hpp>

#include "fmt_arg.hpp"
#include "HttpRequestBuilder.hpp"
#include "Tunables.hpp"
#include "Util.hpp"

namespace Stand
{
	void PackageInstallTask::onTick()
	{
		if (hr.isConstructed())
		{
			if (hr->tickUntilDone())
			{
				const std::pair<std::string, std::string>& file = package.remote.files.at(fi);
				if (hr->result.has_value())
				{
					auto path = package.getFilePath(file.first);
					auto parent = path.parent_path();
					if (!std::filesystem::is_directory(parent))
					{
						std::filesystem::create_directories(parent);
					}
					{
						std::ofstream of(path, std::ios::binary);
						of << std::move(hr->result->body);
					}
					SOUP_ASSERT(std::filesystem::is_regular_file(path), "I/O Error. An anti-virus may be interfering.");
				}
				else
				{
					Util::toast(LANG_FMT("REPO_DLFAIL", FMT_ARG("file", file.first), FMT_ARG("package", package.name)), TOAST_ALL);
				}

				// Registering it even in failure case so "missing locally" logic can pick it up.
				package.local.files.emplace_back(file.first);

				++fi;
				hr.destroy();
				initiateDownload();
			}
		}
		else
		{
			initiateDownload();
		}
	}

	void PackageInstallTask::initiateDownload()
	{
		if (fi == package.remote.files.size())
		{
			return setWorkDone();
		}
		const std::pair<std::string, std::string>& file = package.remote.files.at(fi);
		//Util::toast(fmt::format("Initiating download of {} for package {}", file.first, package.name));
		std::string url = file.second;
		if (g_tunables.getBool(TUNAHASH("gh-via-jsdelivr")))
		{
			static soup::Regex r(R"(^raw\.githubusercontent\.com/([^/]+)/([^/]+)/(.+)$)", soup::RE_DOLLAR_ENDONLY);
			auto res = r.match(url);
			if (res.isSuccess())
			{
				// cdn.jsdelivr.net/gh/$1/$2@$3
				url = "cdn.jsdelivr.net/gh/" + res.findGroupByIndex(1)->toString() + "/" + res.findGroupByIndex(2)->toString() + "@" + res.findGroupByIndex(3)->toString();
			}
		}
		//Util::toast(fmt::format("Downloading {} for package {} from {}", file.first, package.name, url), TOAST_LOGGER);
		auto [host, path] = HttpRequestBuilder::parseHostPathString(url);
		hr.construct(soup::HttpRequest(std::move(host), std::move(path)));
	}
}
