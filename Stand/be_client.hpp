#pragma once

#include <cstdint>

namespace battleye
{
    struct becl_game_data
    {
        using send_packet_t = void(*)(void* packet, uint32_t length);
        send_packet_t send_packet;
    };
}