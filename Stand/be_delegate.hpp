#pragma once

#include <vector>

#include "be_client.hpp"
#include "be_packet.hpp"

namespace battleye::delegate
{
	extern battleye::becl_game_data::send_packet_t o_send_packet;

	void received_packet(uint8_t* received_packet, uint32_t length);

	void respond(uint8_t response_index, std::initializer_list<uint8_t> data);
	void respond_fragmented(uint8_t response_index, battleye::be_fragment fragment, std::initializer_list<uint8_t> data);
}