#include "pch.h"
#include "MusicStateStepAction.h"

#include "MusicManager.h"
#include "MusicStates.h"

IMPLEMENT_SCRIPT_FIELDS_INHERITED(MusicStateStepAction, CameraTransitionStepAction,
    SERIALIZED_ENUM_INT(m_musicState, "Music State", kMusicStateNames, kMusicStateCount)
)

MusicStateStepAction::MusicStateStepAction(GameObject* owner)
    : CameraTransitionStepAction(owner)
{
}

void MusicStateStepAction::executeAction(CameraTransitionController* controller, CameraTransitionStep* step)
{
    if (m_musicState < 0 || m_musicState >= kMusicStateCount)
    {
        return;
    }

    MusicManager* music = MusicManager::Get();

    if (music == nullptr)
    {
        Debug::warn("MusicStateStepAction on '%s' found no MusicManager in the scene.",
            GameObjectAPI::getName(getOwner()));
        return;
    }

    music->SetMusicState(kMusicStateNames[m_musicState]);
}

IMPLEMENT_SCRIPT(MusicStateStepAction)
