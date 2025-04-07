#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListWarning : public CommandList
	{
	private:
		const int32_t m_warning;

	public:
		bool m_warning_acknowledged = false;

		explicit CommandListWarning(CommandList* const parent, Label&& menu_name, const int32_t warning, std::vector<CommandName>&& command_names = {})
			: CommandList(parent, std::move(menu_name), std::move(command_names), NOLABEL, CMDFLAGS_LIST_WARNING), m_warning(warning)
		{
		}

		void onClick(Click& click) final;

		std::string getState() const final
		{
			if (m_warning_acknowledged)
			{
				return "Warning Acknowledged";
			}
			return {};
		}

		void setState(Click& click, const std::string& state) final
		{
			m_warning_acknowledged = (state == "Warning Acknowledged");
		}

		void applyDefaultState() final
		{
			m_warning_acknowledged = false;
		}

	protected:
		virtual bool shouldShowWarning() const;
	};
}
