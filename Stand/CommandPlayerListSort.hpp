#pragma once

#include "CommandListSelect.hpp"

#include "PlayerListSort.hpp"

namespace Stand
{
	class CommandPlayerListSort : public CommandListSelect
	{
	public:
		explicit CommandPlayerListSort(CommandList* const parent)
			: CommandListSelect(parent, LOC("SRTBY"), {}, NOLABEL, {
				{reinterpret_cast<long long>(&PlayerListSort::name), LOC("NME")},
				{reinterpret_cast<long long>(&PlayerListSort::distance), LOC("DIST")},
				{reinterpret_cast<long long>(&PlayerListSort::rank), LOC("RNK")},
				{reinterpret_cast<long long>(&PlayerListSort::money), LOC("MNY_A")},
				{reinterpret_cast<long long>(&PlayerListSort::kd), LOC("KD")},
				{reinterpret_cast<long long>(&PlayerListSort::discovery), LOC("DSCVYTME")},
				{reinterpret_cast<long long>(&PlayerListSort::slot), LOC("SPID2")},
				{reinterpret_cast<long long>(&PlayerListSort::host_queue), LOC("HOSTQPOS")},
			}, reinterpret_cast<long long>(&PlayerListSort::name))
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			PlayerListSort::algo = reinterpret_cast<player_sort_algo_t>(value);
			PlayerListSort::update();
		}
	};
}
