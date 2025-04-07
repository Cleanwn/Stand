#include "CommandChatGadget.hpp"

#include <soup/lyoDocument.hpp>
#include <soup/lyoFlatDocument.hpp>
#include <soup/lyoStylesheet.hpp>
#include <soup/RenderTarget.hpp>
#include <soup/xml.hpp>

#include "ChatHistory.hpp"
#include "format_time_since_1970_for_user_locale.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandChatGadget::CommandChatGadget(CommandList* const parent)
		: CommandCanvas(parent, LIT_OBF("Chat Gadget"), CMDNAMES_OBF("chatgadget"))
	{
	}

	void CommandChatGadget::draw(soup::RenderTarget& rt)
	{
		std::string markup;

		for (const auto& msg : ChatHistory::history)
		{
			markup.append("<p>");
			if (msg.time != 0)
			{
				markup.append("<time>[");
				markup.append(StringUtils::utf16_to_utf8(format_time_since_1970_for_user_locale(msg.time)));
				markup.append("]</time> ");
			}
			markup.append("<sender>");
			markup.append(msg.sender_name);
			markup.append("</sender> <meta>[");
			markup.append(msg.team_chat ? "TEAM" : "ALL");
			if (msg.is_auto)
			{
				markup.append(", AUTO");
			}
			markup.append("]</meta> ");
			markup.append(msg.contents);
			markup.append("</p>");
		}

		auto doc = soup::lyoDocument::fromMarkup(markup);

#if false
		doc->stylesheets.back().addRulesFromCss(R"EOC(
sender {
	colour: red;
}

time {
	colour: grey;
}

meta {
	colour: grey;
}
)EOC");
#else
		soup::lyoRule sender_rule;
		sender_rule.selector = "sender";
		sender_rule.style.color = soup::Rgb::RED;
		doc->stylesheets.at(0).rules.emplace_back(std::move(sender_rule));

		soup::lyoRule time_rule;
		time_rule.selector = "time";
		time_rule.style.color = soup::Rgb::GREY;
		doc->stylesheets.at(0).rules.emplace_back(std::move(time_rule));

		soup::lyoRule meta_rule;
		meta_rule.selector = "meta";
		meta_rule.style.color = soup::Rgb::GREY;
		doc->stylesheets.at(0).rules.emplace_back(std::move(meta_rule));
#endif

		doc->propagateStyle();

		doc->flatten(rt.width, rt.height).draw(rt);
	}
}
