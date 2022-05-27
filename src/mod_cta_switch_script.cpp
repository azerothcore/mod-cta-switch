/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "loader.h"
#include "ScriptMgr.h"
#include "Config.h"

enum EventIds
{
    EVENT_CTA_ALTERAC_VALLEY         = 18,
    EVENT_CTA_WARSONG_GULCH          = 19,
    EVENT_CTA_ARATHI_BASIN           = 20,
    EVENT_CTA_EYE_OF_THE_STORM       = 21,
    EVENT_CTA_STRAND_OF_THE_ANCIENTS = 53,
    EVENT_CTA_ISLE_OF_CONQUEST       = 54,
};

class cta_switch_event_script : public GameEventScript
{
public:
    cta_switch_event_script() : GameEventScript("cta_switch_event_script") { }

    void OnStart(uint16 eventId) override
    {
        if (!sConfigMgr->GetOption<int>("ModCTASwitch.Enable", 0))
        {
            return;
        }

        switch (eventId)
        {
            case EVENT_CTA_EYE_OF_THE_STORM:
                sGameEventMgr->StopEvent(eventId, true);
                sGameEventMgr->StartEvent(sConfigMgr->GetOption<int>("ModCTASwitch.SwitchEots", EVENT_CTA_WARSONG_GULCH), true);
                break;
            case EVENT_CTA_STRAND_OF_THE_ANCIENTS:
                sGameEventMgr->StopEvent(eventId, true);
                sGameEventMgr->StartEvent(sConfigMgr->GetOption<int>("ModCTASwitch.SwitchSota", EVENT_CTA_ARATHI_BASIN), true);
                break;
            case EVENT_CTA_ISLE_OF_CONQUEST:
                sGameEventMgr->StopEvent(eventId, true);
                sGameEventMgr->StartEvent(sConfigMgr->GetOption<int>("ModCTASwitch.SwitchIoc", EVENT_CTA_ALTERAC_VALLEY), true);
                break;
        }
    }

    void OnEventCheck(uint16 /*eventId*/) override
    {
        if (!sConfigMgr->GetOption<int>("ModCTASwitch.Enable", 0))
        {
            return;
        }

        std::vector<uint16> eventIds = { EVENT_CTA_ALTERAC_VALLEY, EVENT_CTA_ARATHI_BASIN, EVENT_CTA_WARSONG_GULCH,
            EVENT_CTA_EYE_OF_THE_STORM, EVENT_CTA_ISLE_OF_CONQUEST, EVENT_CTA_STRAND_OF_THE_ANCIENTS };

        for (uint16 const& activeEvent : eventIds)
        {
            if (sGameEventMgr->IsActiveEvent(activeEvent))
            {
                time_t t = time(nullptr);
                tm* now = localtime(&t);

                if (now->tm_wday == 2 /* Tuesday */)
                {
                    GameEventMgr::GameEventDataMap const& events = sGameEventMgr->GetEventMap();

                    if (std::size_t(activeEvent) >= events.size())
                    {
                        LOG_ERROR("module", "[CTA-Switch]: Error, tried to stop unexisting event. ID: {}", activeEvent);
                        return;
                    }

                    GameEventData const& eventData = events[activeEvent];

                    sGameEventMgr->StopEvent(activeEvent, true);
                    LOG_INFO("module", "[CTA-Switch]: Stopping {} ({})", eventData.description, activeEvent);
                }
            }
        }
    }
};

void Addmod_cta_switch_eventScripts()
{
    new cta_switch_event_script();
}
