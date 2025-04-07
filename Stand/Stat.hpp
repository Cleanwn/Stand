#pragma once

#define NO_PROFILE_SETTING -1

namespace Stand
{
#pragma pack(push, 1)
  class Stat
  {
  public:
    //bool packed;
    uint8_t type;
    const char* name;

    constexpr Stat(const char* name, bool packed, uint8_t type)
      : name(name), /*packed(packed),*/ type(type)
    {
    }

    [[nodiscard]] bool isPopular() const;
    [[nodiscard]] int getProfileSettingId() const;
    [[nodiscard]] Label getLabel() const;
  };
#pragma pack(pop)
}