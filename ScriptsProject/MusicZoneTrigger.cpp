#include "pch.h"
#include "MusicZoneTrigger.h"
#include "MusicManager.h"
#include "MusicStates.h"

#include <vector>

IMPLEMENT_SCRIPT_FIELDS(MusicZoneTrigger,
    SERIALIZED_FLOAT(m_xWidth, "Trigger Width X", 0.1f, 1000.0f, 0.1f),
    SERIALIZED_FLOAT(m_zWidth, "Trigger Width Z", 0.1f, 1000.0f, 0.1f),
    SERIALIZED_ENUM_INT(m_targetState, "State On Enter", kMusicStateNames, kMusicStateCount),
    SERIALIZED_BOOL(m_changeOnExit, "Change State On Exit"),
    SERIALIZED_ENUM_INT(m_stateOnExit, "State On Exit", kMusicStateNames, kMusicStateCount),
    SERIALIZED_BOOL(m_triggerOnce, "Trigger Once")
)

MusicZoneTrigger::MusicZoneTrigger(GameObject* owner)
    : Script(owner)
{
}

void MusicZoneTrigger::Start()
{
    m_playersInside.clear();
    m_bothWereInside = false;
    m_hasTriggered   = false;
}

void MusicZoneTrigger::Update()
{
    GameObject* owner = getOwner();
    Transform* ownerTransform = GameObjectAPI::getTransform(owner);
    if (!ownerTransform)
    {
        return;
    }

    const Vector3 triggerCenter = TransformAPI::getGlobalPosition(ownerTransform);

    std::vector<GameObject*> players = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER);

    for (GameObject* player : players)
    {
        Transform* pt = GameObjectAPI::getTransform(player);
        const Vector3 pos = TransformAPI::getPosition(pt);

        if (containsPoint(triggerCenter, pos))
            m_playersInside.insert(player);
        else
            m_playersInside.erase(player);
    }

    const bool bothAreInside = !players.empty() && (m_playersInside.size() >= players.size());

    // Flanco de entrada: ambos jugadores acaban de entrar.
    if (bothAreInside && !m_bothWereInside)
    {
        if (!(m_triggerOnce && m_hasTriggered))
        {
            MusicManager* music = MusicManager::Get();
            if (music && m_targetState >= 0 && m_targetState < kMusicStateCount)
            {
                music->SetMusicState(kMusicStateNames[m_targetState]);
            }
            m_hasTriggered = true;
        }
    }
    // Flanco de salida: dejan de estar ambos dentro.
    else if (!bothAreInside && m_bothWereInside && m_changeOnExit)
    {
        MusicManager* music = MusicManager::Get();
        if (music && m_stateOnExit >= 0 && m_stateOnExit < kMusicStateCount)
        {
            music->SetMusicState(kMusicStateNames[m_stateOnExit]);
        }
    }

    m_bothWereInside = bothAreInside;
}

bool MusicZoneTrigger::containsPoint(const Vector3& center, const Vector3& point) const
{
    const float halfX = m_xWidth * 0.5f;
    const float halfZ = m_zWidth * 0.5f;

    return point.x >= center.x - halfX &&
        point.x <= center.x + halfX &&
        point.z >= center.z - halfZ &&
        point.z <= center.z + halfZ;
}

IMPLEMENT_SCRIPT(MusicZoneTrigger)
