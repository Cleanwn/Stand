#include "UniqueThread.hpp"

#include "Exceptional.hpp"

namespace Stand
{
	UniqueThread::UniqueThread()
	{
	}

	UniqueThread* UniqueThread::create(std::function<void(UniqueThread*)>&& func)
	{
		++Exceptional::counted_threads;
		auto inst = new UniqueThread();
		Exceptional::createManagedThread([inst, func{ std::move(func) }]
		{
			__try
			{
				func(inst);
			}
			__EXCEPTIONAL()
			{
			}
			delete inst;
			--Exceptional::counted_threads;
		});
		return inst;
	}

	void UniqueThread::stop()
	{
		stopped = true;
	}
}
