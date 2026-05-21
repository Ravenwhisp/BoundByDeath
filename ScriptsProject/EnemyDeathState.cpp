#include "pch.h"
#include "EnemyDeathState.h"

IMPLEMENT_SCRIPT_FIELDS(EnemyDeathState,
    SERIALIZED_FLOAT(m_fallbackDestroyDelay, "Fallback Destroy Delay", 0.0f, 10.0f, 0.1f),
    SERIALIZED_BOOL(m_useAnimationDuration, "Use Animation Duration"),
    SERIALIZED_FLOAT(m_extraDestroyDelay, "Extra Destroy Delay", 0.0f, 5.0f, 0.05f),
    SERIALIZED_BOOL(m_debugEnabled, "Debug Enabled")
)

EnemyDeathState::EnemyDeathState(GameObject* owner)
    : StateMachineScript(owner)
{
}

void EnemyDeathState::OnStateEnter()
{
    m_elapsedTime = 0.0f;
    m_destroyRequested = false;

    if (m_debugEnabled)
    {
        Debug::log("[EnemyDeathState] ENTER");
    }
}

void EnemyDeathState::OnStateUpdate()
{
    if (m_destroyRequested)
    {
        return;
    }

    GameObject* owner = getOwner();
    if (!owner)
    {
        return;
    }

    m_elapsedTime += Time::getDeltaTime();

    float destroyDelay = m_fallbackDestroyDelay;

    if (m_useAnimationDuration)
    {
        AnimationComponent* animation = AnimationAPI::getAnimationComponent(owner);
        if (animation)
        {
            const float duration = AnimationAPI::getPlaybackDuration(animation);
            if (duration > 0.0f)
            {
                destroyDelay = duration + m_extraDestroyDelay;
            }
        }
    }

    if (m_elapsedTime >= destroyDelay)
    {
        if (m_debugEnabled)
        {
            Debug::log("[EnemyDeathState] Destroying owner");
        }

        m_destroyRequested = true;
        GameObjectAPI::removeGameObject(owner);
    }
}

void EnemyDeathState::OnStateExit()
{
    if (!m_debugEnabled)
    {
        return;
    }

    Debug::log("[EnemyDeathState] EXIT");
}

IMPLEMENT_SCRIPT(EnemyDeathState)