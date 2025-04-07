#include "get_current_time_millis.hpp"

#include <soup/time.hpp>

namespace Stand
{
    time_t get_current_time_millis() noexcept
    {
        return soup::time::millis();
    }
}
