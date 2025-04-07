#include "PubSubClient.hpp"

#include <soup/DelayedCtor.hpp>
#include <soup/EstablishWebSocketConnectionTask.hpp>
#include <soup/MaintainWebSocketConnectionTask.hpp>
#include <soup/ObfusString.hpp>

#include "NetInterface.hpp"

namespace Stand
{
	struct PubSubSubscription
	{
		std::string topic;
		PubSubClient::callback_t cb;
	};

	static std::vector<PubSubSubscription> s_subscriptions{};
	static soup::SharedPtr<soup::WebSocketConnection> s_sock{};
	static bool s_connecting = false;

	static void onWebSocketFrame(soup::WebSocketConnection&, soup::WebSocketMessage&& msg) SOUP_EXCAL
	{
		size_t topic_off = msg.data.find_first_of(':');
		if (topic_off != std::string::npos)
		{
			std::string topic = msg.data.substr(0, topic_off);
			for (const auto& subscription : s_subscriptions)
			{
				if (subscription.topic == topic)
				{
					subscription.cb(msg.data.substr(topic_off + 1));
					break;
				}
			}
		}
	}

	struct PubSubClientConnectTask : public soup::Task
	{
		soup::EstablishWebSocketConnectionTask underlying;

		PubSubClientConnectTask()
			: underlying(false, "198.251.89.45", 80, "/sub")
		{
		}

		void onTick() final
		{
			if (underlying.tickUntilDone())
			{
				for (const auto& subscription : s_subscriptions)
				{
					std::string msg(1, '+');
					msg.append(subscription.topic);
#ifdef STAND_DEBUG
					//Util::toast(std::string(msg));
#endif
					underlying.sock->wsSend(std::move(msg));
				}
				g_net_interface.add<soup::MaintainWebSocketConnectionTask>(underlying.sock, &onWebSocketFrame);
				s_sock = std::move(underlying.sock);
				s_connecting = false;

				setWorkDone();
			}
		}

		std::string toString() const SOUP_EXCAL final
		{
			std::string str = soup::ObfusString("PubSubClientConnectTask: [");
			str.append(underlying.toString());
			str.push_back(']');
			return str;
		}
	};

	struct PubSubClientSubscribeTask : public soup::Task
	{
		std::string topic;
		PubSubClient::callback_t cb;

		PubSubClientSubscribeTask(std::string&& topic, PubSubClient::callback_t cb)
			: topic(std::move(topic)), cb(cb)
		{
		}

		void onTick() final
		{
			bool is_new = true;
			for (const auto& subscription : s_subscriptions)
			{
				if (subscription.topic == topic)
				{
					is_new = false;
#ifdef STAND_DEBUG
					//Util::toast(fmt::format("PubSubClientSubscribeTask: Topic \"{}\" is already subscribed.", topic));
#endif
					break;
				}
			}
			if (is_new)
			{
				s_subscriptions.emplace_back(topic, cb);
			}

			if (s_sock)
			{
				std::string msg(1, '+');
				msg.append(topic);
				s_sock->wsSend(std::move(msg));
			}
			else if (!s_connecting)
			{
				s_connecting = true;
				g_net_interface.add<PubSubClientConnectTask>();
			}

			setWorkDone();
		}

		std::string toString() const SOUP_EXCAL final
		{
			return soup::ObfusString("PubSubClientSubscribeTask").str();
		}
	};

	void PubSubClient::subscribe(std::string topic, callback_t cb)
	{
		g_net_interface.add<PubSubClientSubscribeTask>(std::move(topic), cb);
	}

	struct PubSubClientUnsubscribeTask : public soup::Task
	{
		std::string topic;

		PubSubClientUnsubscribeTask(std::string&& topic)
			: topic(std::move(topic))
		{
		}

		void onTick() final
		{
			for (auto i = s_subscriptions.begin(); i != s_subscriptions.end(); ++i)
			{
				if (i->topic == topic)
				{
					s_subscriptions.erase(i);
					break;
				}
			}

			if (s_sock)
			{
				if (s_subscriptions.empty())
				{
					s_sock->close();
					s_sock.reset();
				}
				else
				{
					std::string msg(1, '-');
					msg.append(topic);
					s_sock->wsSend(std::move(msg));
				}
			}

			setWorkDone();
		}

		std::string toString() const SOUP_EXCAL final
		{
			return soup::ObfusString("PubSubClientUnsubscribeTask").str();
		}
	};

	void PubSubClient::unsubscribe(std::string topic)
	{
		g_net_interface.add<PubSubClientUnsubscribeTask>(std::move(topic));
	}

	struct PubSubClientUnsubscribeAllTask : public soup::Task
	{
		void onTick() final
		{
			s_subscriptions.clear();

			if (s_sock)
			{
				s_sock->close();
				s_sock.reset();
			}

			setWorkDone();
		}

		std::string toString() const SOUP_EXCAL final
		{
			return soup::ObfusString("PubSubClientUnsubscribeAllTask").str();
		}
	};

	void PubSubClient::unsubscribeAll()
	{
		g_net_interface.add<PubSubClientUnsubscribeAllTask>();
	}
}
