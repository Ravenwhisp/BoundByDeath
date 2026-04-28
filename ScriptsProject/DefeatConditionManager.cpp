#include "pch.h"
#include "DefeatConditionManager.h"
#include "PlayerState.h"

IMPLEMENT_SCRIPT_FIELDS(DefeatConditionManager,
    SERIALIZED_COMPONENT_REF(m_player1Transform, "Player 1 Transform", ComponentType::TRANSFORM),
    SERIALIZED_COMPONENT_REF(m_player2Transform, "Player 2 Transform", ComponentType::TRANSFORM)
)

DefeatConditionManager::DefeatConditionManager(GameObject* owner)
    : Script(owner)
{
}

void DefeatConditionManager::Start()
{
    Transform* player1Transform = m_player1Transform.getReferencedComponent();
    Transform* player2Transform = m_player2Transform.getReferencedComponent();

    m_player1State = findPlayerStateFromReference(player1Transform);
    m_player2State = findPlayerStateFromReference(player2Transform);

    if (!m_player1State)
    {
        Debug::warn("DefeatConditionManager: Could not find PlayerState for Player 1.");
    }

    if (!m_player2State)
    {
        Debug::warn("DefeatConditionManager: Could not find PlayerState for Player 2.");
    }
}

void DefeatConditionManager::Update()
{
    if (m_hasTriggeredDefeat)
    {
        return;
    }

    if (!m_player1State || !m_player2State)
    {
        return;
    }

    const bool bothPlayersDowned = m_player1State->isDowned() && m_player2State->isDowned();

    if (!bothPlayersDowned)
    {
        m_defeatCountdownStarted = false;
        m_defeatTimer = 0.0f;
        return;
    }

    if (!m_defeatCountdownStarted)
    {
        m_defeatCountdownStarted = true;
        m_defeatTimer = 0.0f;
    }

    m_defeatTimer += Time::getDeltaTime();

    if (m_defeatTimer >= m_defeatDelay)
    {
        triggerDefeat();
    }
}

PlayerState* DefeatConditionManager::findPlayerStateFromReference(Transform* transform) const
{
    if (!transform)
    {
        return nullptr;
    }

    GameObject* player = ComponentAPI::getOwner(transform);
    if (!player)
    {
        return nullptr;
    }

    Script* script = GameObjectAPI::getScript(player, "PlayerState");
    return dynamic_cast<PlayerState*>(script);
}

void DefeatConditionManager::triggerDefeat()
{
    m_hasTriggeredDefeat = true;

    SceneAPI::requestSceneChange("LoseScene");
}

IMPLEMENT_SCRIPT(DefeatConditionManager)

