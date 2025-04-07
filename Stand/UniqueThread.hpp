#pragma once

#include <atomic>
#include <functional>

namespace Stand
{
	class UniqueThread
	{
	public:
		std::atomic_bool stopped = false;
		
	private:
		explicit UniqueThread();
	public:
		static UniqueThread* create(std::function<void(UniqueThread*)>&& func);

		void stop();
	};
}
