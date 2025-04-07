#include "get_next_arg.hpp"

#include "extract_delimited_string_with_escaping.hpp"

namespace Stand
{
    std::wstring get_next_arg(std::wstring& args)
    {
        return extract_delimited_string_with_escaping(args, L' ');
    }
}
