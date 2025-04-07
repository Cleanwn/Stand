#pragma once

#include <cstdint>

#include "RsonWriter.hpp"

namespace rage
{
	struct rlMetric
	{
		virtual ~rlMetric() = 0;
		virtual void _1() = 0;
		virtual void _2() = 0;
		virtual void _3() = 0;
		virtual const char* getName() = 0;
		virtual bool Write(rage::RsonWriter&) = 0;
		virtual int getId() = 0;
		virtual bool _7() = 0;

		/* 0x08 */ uint64_t unk1;
		/* 0x10 */ uint64_t unk2;

		[[nodiscard]] std::string getLogData()
		{
			char data[0x4000] = { 0 };
			RsonWriter buf(data, sizeof(data));
			Write(buf);
			return std::string(data);
		}
	};
	static_assert(sizeof(rlMetric) == 0x18);
}

struct MetricRemoteCheat : public rage::rlMetric
{
	virtual const char* getSubName() = 0;
	virtual bool _9() = 0;

	/* 0x18 */ char gamer_handle_string[32];
};

