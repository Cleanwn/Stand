#include "Logger.hpp"

#include <fmt/format.h>

#include "format_time_since_1970_for_user_locale.hpp"
#include "get_seconds_since_unix_epoch.hpp"
#include "lang.hpp"
#include "StringUtils.hpp"

#define LOG_MILLIS false

#if LOG_MILLIS
#include "get_current_time_millis.hpp"
#endif

namespace Stand
{
	void Logger::clearRepetitionData()
	{
		last_message.clear();
		last_message_repetitions = 0;
	}

	void Logger::enterBlockMode()
	{
		clearRepetitionData();
		block_mode = true;
	}

	void Logger::leaveBlockMode()
	{
		block_mode = false;
	}

	void Logger::log(std::string message)
	{
		message.push_back('\n');
		log_no_nl(std::move(message));
	}

	void Logger::log_no_nl(std::string message)
	{
		if (!block_mode)
		{
			if (!ignore_repetitions)
			{
				if (message == last_message)
				{
					++last_message_repetitions;
					return;
				}
				last_message = message;
				if (last_message_repetitions != 0)
				{
					block_mode = true;
					log(last_message_repetitions == 1 ? LANG_GET("LOG_REP_S") : LANG_FMT("LOG_REP_P", last_message_repetitions));
					block_mode = false;
					last_message_repetitions = 0;
				}
			}

			message.insert(0, "] ");
#if LOG_MILLIS
			message.insert(0, fmt::format("{:0>3}", get_current_time_millis() % 1000));
			message.insert(0, 1, '.');
#endif
			message.insert(0, StringUtils::utf16_to_utf8(format_time_since_1970_for_user_locale(get_seconds_since_unix_epoch())));
			message.insert(0, 1, '[');
		}

		write(std::move(message));
	}
}
