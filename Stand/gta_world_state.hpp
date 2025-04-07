#pragma once

#include "gta_net_game_event.hpp"

// This is based off 323

class CNetworkWorldStateData
{
	virtual CNetworkWorldStateData() = default;
	virtual unsigned int GetTypeId() const = 0;
};

class CNetworkRopeWorldStateData : public CNetworkWorldStateData
{
	unsigned int GetTypeId() const final { return 6; }
};

class CScriptWorldStateEvent : public rage::netGameEvent
{
public:
	/* 0x30 */ CNetworkWorldStateData* world_state_data;
};
