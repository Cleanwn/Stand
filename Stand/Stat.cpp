#include "Stat.hpp"

#include "Stats.hpp"

namespace Stand
{
  static inline const char* popular[] = {
    "MPPLY_TOTAL_RACES_WON",
    "MPPLY_TOTAL_RACES_LOST",

    "MPPLY_TOTAL_DEATHMATCH_WON",
    "MPPLY_TOTAL_DEATHMATCH_LOST",

    "MPPLY_TOTAL_TDEATHMATCH_WON",
    "MPPLY_TOTAL_TDEATHMATCH_LOST",
  };

  bool Stat::isPopular() const
  {
    for (auto popular_stat : popular)
    {
      if (name == popular_stat)
      {
        return true;
      }
    }
    return false;
  }

  int Stat::getProfileSettingId() const
  {
    SOUP_ASSERT(type == Stats::PROFILESETTING);

    std::string profile_name = name;
    std::string profile_id = profile_name.substr(profile_name.rfind("_") + 1, profile_name.length());

    int profile_setting_id = std::atoi(profile_id.c_str());
    if (profile_setting_id == 0 && profile_id != "0")
    {
      SOUP_ASSERT(false);
    }
    return profile_setting_id;
  }

  Label Stat::getLabel() const
  {
    if (isPopular())
    {
      std::string stat = name;
      std::string without_prefix = stat.substr(stat.find("_") + 1, stat.length());

      return LOC_RT(without_prefix);
    }

    return LIT(name);
  }
}