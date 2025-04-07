#pragma once

namespace rage
{
	class pgBase
	{
	public:
		virtual ~pgBase() = default;
	private:
		void* m_pgunk;
	};
	static_assert(sizeof(pgBase) == 0x10);
}
