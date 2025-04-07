#pragma once

#include <string>

namespace Stand
{
	class Logger
	{
	protected:
		bool block_mode = false;

		bool ignore_repetitions = false;
		std::string last_message = {};
		size_t last_message_repetitions = 0;

	public:
		void clearRepetitionData();

		void enterBlockMode();
		void leaveBlockMode();

		void log(std::string message);
		void log_no_nl(std::string message);

	protected:
		virtual void write(std::string&& message) = 0;
	};
}
