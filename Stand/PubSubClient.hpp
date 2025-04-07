#pragma once

#include <string>

namespace Stand
{
	struct PubSubClient
	{
		using callback_t = void(*)(std::string&& data);

		static void subscribe(std::string topic, callback_t cb);
		static void unsubscribe(std::string topic);
		static void unsubscribeAll();
	};
}
