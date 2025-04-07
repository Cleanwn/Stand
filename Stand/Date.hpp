#pragma once

#include "scrValue.hpp"

namespace Stand
{
  struct Date
  {
    rage::scrValue year;
    rage::scrValue month;
    rage::scrValue day;
    rage::scrValue hour;
    rage::scrValue minute;
    rage::scrValue second;
    rage::scrValue millisecond;
  };
}