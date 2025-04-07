#pragma once

#include <soup/Task.hpp>

#include <soup/DelayedCtor.hpp>
#include <soup/HttpRequestTask.hpp>
#include "Package.hpp"

namespace Stand
{
	class PackageInstallTask : public soup::Task
	{
	public:
		Package& package;
		size_t fi = 0;
		soup::DelayedCtor<soup::HttpRequestTask> hr;

		PackageInstallTask(Package& package)
			: package(package)
		{
		}

		void onTick() final;
	protected:
		void initiateDownload();
	};
}
