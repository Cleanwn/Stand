#include "format_duration.hpp"

#include <fmt/format.h>

#include "lang.hpp"

namespace Stand
{
    static void add_duration_unit(std::string& str, time_t& ms, time_t unit_dur, hash_t unit_label, hash_t unit_label_plural)
    {
        if (ms >= unit_dur || unit_dur == 1000)
        {
            time_t unit = ms / unit_dur;
            ms -= unit * unit_dur;
            if (!str.empty())
            {
                str.append(", ");
            }
            str.append(fmt::to_string(unit)).push_back(' ');
            str.append(Lang::get(unit == 1 ? unit_label : unit_label_plural));
        }
    }

#define ADD_DURATION_UNIT(unit_label_prefix, unit_dur) add_duration_unit(str, ms, unit_dur, ATSTRINGHASH(unit_label_prefix), ATSTRINGHASH(unit_label_prefix "_P"));

    std::string format_duration(time_t ms)
    {
        std::string str{};
		ADD_DURATION_UNIT("DUR_D", (24 * 60 * 60 * 1000));
		ADD_DURATION_UNIT("DUR_H", (60 * 60 * 1000));
		ADD_DURATION_UNIT("DUR_M", (60 * 1000));
		ADD_DURATION_UNIT("DUR_S", (1000));
        return str;
    }
}
