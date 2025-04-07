#include "get_seconds_since_unix_epoch.hpp"

#include <soup/time.hpp>

namespace Stand
{
    unixtime_t get_seconds_since_unix_epoch() noexcept
    {
        return soup::time::unixSeconds();
    }
}
