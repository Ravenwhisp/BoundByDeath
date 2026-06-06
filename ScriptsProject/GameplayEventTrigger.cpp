#include "pch.h"
#include "GameplayEventTrigger.h"

IMPLEMENT_SCRIPT_FIELDS(GameplayEventTrigger,
    SERIALIZED_BOOL(m_triggerOnlyOnce, "Trigger Only Once")
)

GameplayEventTrigger::GameplayEventTrigger(GameObject* owner)
    : Script(owner)
{
}

void GameplayEventTrigger::Start()
{
    findPlayers();
}

void GameplayEventTrigger::OnTriggerEnter(GameObject* gameObject)
{
    if (gameObject == nullptr)
    {
        return;
    }

    if (m_triggerOnlyOnce && m_hasTriggered)
    {
        return;
    }

    if (!isTrackedPlayer(gameObject))
    {
        return;
    }

    setPlayerInside(gameObject, true);
    tryActivate();
}

void GameplayEventTrigger::OnTriggerExit(GameObject* gameObject)
{
    if (gameObject == nullptr)
    {
        return;
    }

    if (!isTrackedPlayer(gameObject))
    {
        return;
    }

    setPlayerInside(gameObject, false);
}

void GameplayEventTrigger::findPlayers()
{
    m_player1 = nullptr;
    m_player2 = nullptr;

    m_player1Inside = false;
    m_player2Inside = false;

    const std::vector<GameObject*> players = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER, true);

    if (!players.empty())
    {
        m_player1 = players[0];
    }

    if (players.size() > 1)
    {
        m_player2 = players[1];
    }

    if (m_player1 == nullptr || m_player2 == nullptr)
    {
        Debug::warn( "GameplayEventTrigger on '%s' requires two active players with tag PLAYER.", GameObjectAPI::getName(getOwner()));
    }
}

void GameplayEventTrigger::setPlayerInside(GameObject* gameObject, bool inside)
{
    if (gameObject == m_player1)
    {
        m_player1Inside = inside;
        return;
    }

    if (gameObject == m_player2)
    {
        m_player2Inside = inside;
        return;
    }
}

bool GameplayEventTrigger::isTrackedPlayer(GameObject* gameObject) const
{
    return gameObject == m_player1 || gameObject == m_player2;
}

bool GameplayEventTrigger::canActivate() const
{
    return m_player1Inside && m_player2Inside;
}

void GameplayEventTrigger::tryActivate()
{
    if (m_triggerOnlyOnce && m_hasTriggered)
    {
        return;
    }

    if (!canActivate())
    {
        return;
    }

    activateEvent();

    if (m_triggerOnlyOnce)
    {
        m_hasTriggered = true;
    }
}

void GameplayEventTrigger::activateEvent()
{
    Debug::log("GameplayEventTrigger '%s' activated.", GameObjectAPI::getName(getOwner()));
}

IMPLEMENT_SCRIPT(GameplayEventTrigger)