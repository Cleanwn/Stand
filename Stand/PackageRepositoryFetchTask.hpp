#pragma once

#include <soup/Task.hpp>

#include <soup/GetFileContentsTask.hpp>
#include "PackageRepository.hpp"

namespace Stand
{
	struct PackageRepositoryFetchTask : public soup::Task
	{
		soup::GetFileContentsTask gfc;
		PackageRepository repo;

		PackageRepositoryFetchTask(const soup::Uri& uri)
			: gfc(uri)
		{
		}

		void onTick() final;
	};

	struct InitRepoFromCustomSourceTask : public soup::Task
	{
		soup::GetFileContentsTask gfc;

		InitRepoFromCustomSourceTask(const soup::Uri& uri)
			: gfc(uri)
		{
		}

		void onTick() final;

		[[nodiscard]] std::string toString() const SOUP_EXCAL final;
	};
}
