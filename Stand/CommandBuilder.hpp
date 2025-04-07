#pragma once

#include "fwddecl.hpp"

namespace Stand
{
	template <typename T>
	class CommandBuilder
	{
	private:
		T* cmd;

	public:
		CommandBuilder(T* const cmd) noexcept
			: cmd(cmd)
		{
		}

		CommandBuilder(CommandBuilder&& b) noexcept
			: cmd(b.cmd)
		{
			b.cmd = nullptr;
		}

		void operator=(CommandBuilder&& b)
		{
			this->cmd = b.cmd;
			b.cmd = nullptr;
		}

		CommandBuilder(const CommandBuilder& b) = delete;
		void operator=(const CommandBuilder& b) = delete;

		T* operator->() const noexcept
		{
			return cmd;
		}

		constexpr operator T* () const noexcept
		{
			return cmd;
		}

		~CommandBuilder()
		{
			if (cmd != nullptr)
			{
				finishBuilding();
			}
		}

	private:
		void finishBuilding() const
		{
			cmd->as<T>()->populate();
		}
	};
}
