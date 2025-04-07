#pragma once

#include <soup/DetachedScheduler.hpp>

#define NETINTERFACE_STATS true

#if NETINTERFACE_STATS
#include <soup/SchedulerStats.hpp>
#endif

#include "Exceptional.hpp"

namespace Stand
{
	class NetInterface : public soup::DetachedScheduler
	{
#if NETINTERFACE_STATS
	private:
		soup::SchedulerStats* stats = nullptr;
#endif

	protected:
		void run() final
		{
#if NETINTERFACE_STATS
			stats = add<soup::SchedulerStats>().get(); ++passive_workers;
#endif
			__try
			{
				soup::DetachedScheduler::run();
			}
			__EXCEPTIONAL()
			{
			}
#if NETINTERFACE_STATS
			stats = nullptr;
#endif
		}

#if NETINTERFACE_STATS
	public:
		[[nodiscard]] size_t getNumWorkers() const noexcept
		{
			if (stats)
			{
				return stats->num_workers - passive_workers;
			}
			return 0;
		}

		[[nodiscard]] size_t getNumSockets() const noexcept
		{
			if (stats)
			{
				return stats->num_sockets;
			}
			return 0;
		}
#endif
	};
	inline NetInterface g_net_interface{};
}
