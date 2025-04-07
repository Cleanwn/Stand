#pragma once

#include <soup/ObfusString.hpp>

#define FMT_ARG(name, ...) ::fmt::arg(::soup::ObfusString(name).c_str(), __VA_ARGS__)
