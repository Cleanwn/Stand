#include "CommandPlayerClassifierDetection.hpp"

#include "CommandPlayer.hpp"
#include "FlowEvent.hpp"
#include "Tunables.hpp"

namespace Stand
{
	CommandPlayerClassifierDetection::CommandPlayerClassifierDetection(CommandList* const parent, const flowevent_t event_type, Label&& name, uint8_t severity)
		: CommandAction(parent, std::move(name)), event_type(event_type), preemptive(FlowEvent::is_preemptive(event_type, severity))
	{
		if (preemptive)
		{
			this->help_text = LOC("DT_H_PREMPT");
		}
		else if (PlayerClassifier::canRemoveEvent(event_type))
		{
			this->help_text = LOC("MDDRDT_H");
		}
		updateSeverity(severity);
	}

	void CommandPlayerClassifierDetection::updateSeverity(uint8_t severity)
	{
		preemptive = FlowEvent::is_preemptive(event_type, severity);
		if (preemptive)
		{
			setHelpText(LOC("DT_H_PREMPT"));
		}
		else if (PlayerClassifier::canRemoveEvent(event_type))
		{
			setHelpText(LOC("MDDRDT_H"));
		}
	}

	void CommandPlayerClassifierDetection::onClick(Click& click)
	{
		if (!preemptive
			&& PlayerClassifier::canRemoveEvent(event_type)
			&& (click.type != CLICK_SCRIPTED || FlowEvent::is_custom(event_type) || g_tunables.getBool(TUNAHASH("scripts can pop all detections")))
			)
		{
			auto* const player = parent->parent->as<CommandPlayer>();
			player->classifier.mtx.lock();
			auto it = player->classifier.detections.begin();
			for (; it != player->classifier.detections.end(); ++it)
			{
				if (it->type == event_type)
				{
					break;
				}
			}
			if (it != player->classifier.detections.end())
			{
				player->classifier.detections.erase(it);
				player->classifier.mtx.unlock();
				player->updateClassification(parent->as<CommandPlayerClassifier>());
			}
			else
			{
				player->classifier.mtx.unlock();
			}
			onDetectionRemoved(click.thread_context);
		}
	}

	void CommandPlayerClassifierDetection::onDetectionRemoved(ThreadContext tc)
	{
		this->beGone();
		if (parent->children.empty())
		{
			parent->goBackIfActive(tc);
		}
		else
		{
			parent->processChildrenUpdate();
		}
	}
}
