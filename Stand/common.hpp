#pragma once

// This is the PCH (pre-compiled header). It makes Stand ~33% faster to compile.

// Add headers here that are: Commonly used, mid-large size, and seldom modified.
// - Headers that are too rare or simple might reduce compile time if added here.
// - Headers that are often modified will make working on the project more painful.

// C
#include <cstddef>
#include <cstdint>

// C++
#include <array>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// GTA V
#include "gta_fwddecl.hpp"

#include "rlGamerInfo.hpp"

// Soup
#include <soup/ObfusString.hpp>

// Stand
#include "fwddecl.hpp"
#include "typedecl.hpp"

#include "CommandPhysical.hpp"
#include "CommandList.hpp"
#include "CommandToggle.hpp"

#include "lang.hpp"

#include "natives.hpp"
