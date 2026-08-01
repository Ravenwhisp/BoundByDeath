#include "pch.h"
#include "ElevatorManager.h"
#include "CombatAreaEvent.h"

IMPLEMENT_SCRIPT_FIELDS(ElevatorManager,
    SERIALIZED_COMPONENT_REF_VECTOR(m_combatAreaRoots, "Combat Area Roots", ComponentType::TRANSFORM),
    SERIALIZED_COMPONENT_REF(m_elevator, "Elevator", ComponentType::TRANSFORM),
    SERIALIZED_COMPONENT_REF_VECTOR(m_elevatorTargets, "Elevator Targets", ComponentType::TRANSFORM),
    SERIALIZED_FLOAT(m_moveDuration, "Move Duration", 0.0f, 30.0f, 0.05f)
)

ElevatorManager::ElevatorManager(GameObject* owner)
    : Script(owner)
{
}

void ElevatorManager::Start()
{
    resolveCombatAreas();

    for (int i = 1; i < static_cast<int>(m_combatAreas.size()); i++)
        disableArea(i);
}

void ElevatorManager::Update()
{
    if (m_elevatorMoving)
    {
        updateElevatorMove();
        return;
    }

    const int areaCount = static_cast<int>(m_combatAreas.size());

    for (int i = m_wavesStarted; i < areaCount; i++)
    {
        CombatAreaEvent* area = m_combatAreas[i];
        if (area == nullptr)
            continue;

        if (area->isActive())
        {
            m_wavesStarted = i + 1;

            if (m_wavesStarted < areaCount)
                enableArea(m_wavesStarted);
            break;
        }
    }

    if (m_wavesCompleted < m_wavesStarted && m_wavesCompleted < areaCount)
    {
        CombatAreaEvent* area = m_combatAreas[m_wavesCompleted];
        if (area != nullptr && area->hasCompleted())
        {
            m_wavesCompleted++;

            if (m_elevatorTargetIndex < static_cast<int>(m_elevatorTargets.size()))
            {
                startElevatorMove(m_elevatorTargetIndex);
                m_elevatorTargetIndex++;
            }
        }
    }
}

void ElevatorManager::resolveCombatAreas()
{
    m_combatAreas.clear();

    for (auto& rootRef : m_combatAreaRoots)
    {
        Transform* rootTransform = rootRef.getReferencedComponent();
        if (rootTransform == nullptr)
        {
            m_combatAreas.push_back(nullptr);
            continue;
        }

        GameObject* rootObject = ComponentAPI::getOwner(rootTransform);
        if (rootObject == nullptr)
        {
            m_combatAreas.push_back(nullptr);
            continue;
        }

        CombatAreaEvent* area = GameObjectAPI::findScript<CombatAreaEvent>(rootObject);
        m_combatAreas.push_back(area);
    }
}

void ElevatorManager::enableArea(int waveIndex)
{
    if (waveIndex < 0 || waveIndex >= static_cast<int>(m_combatAreas.size()))
        return;

    CombatAreaEvent* area = m_combatAreas[waveIndex];
    if (area == nullptr)
        return;

    for (auto& enemyRef : area->m_enemies)
    {
        Transform* t = enemyRef.getReferencedComponent();
        if (t == nullptr) continue;
        GameObject* obj = ComponentAPI::getOwner(t);
        if (obj == nullptr) continue;
        GameObjectAPI::setActive(obj, true);
    }
}

void ElevatorManager::disableArea(int waveIndex)
{
    if (waveIndex < 0 || waveIndex >= static_cast<int>(m_combatAreas.size()))
        return;

    CombatAreaEvent* area = m_combatAreas[waveIndex];
    if (area == nullptr)
        return;

    for (auto& enemyRef : area->m_enemies)
    {
        Transform* t = enemyRef.getReferencedComponent();
        if (t == nullptr) continue;
        GameObject* obj = ComponentAPI::getOwner(t);
        if (obj == nullptr) continue;
        GameObjectAPI::setActive(obj, false);
    }
}

void ElevatorManager::startElevatorMove(int targetIndex)
{
    if (targetIndex < 0 || targetIndex >= static_cast<int>(m_elevatorTargets.size()))
        return;

    Transform* elevatorTransform = m_elevator.getReferencedComponent();
    if (elevatorTransform == nullptr)
        return;

    Transform* targetTransform = m_elevatorTargets[targetIndex].getReferencedComponent();
    if (targetTransform == nullptr)
        return;

    m_elevatorStartPos = TransformAPI::getGlobalPosition(elevatorTransform);
    m_elevatorTargetPos = TransformAPI::getGlobalPosition(targetTransform);
    m_elevatorTimer = 0.0f;
    m_elevatorMoving = true;
}

void ElevatorManager::updateElevatorMove()
{
    Transform* elevatorTransform = m_elevator.getReferencedComponent();
    if (elevatorTransform == nullptr)
    {
        m_elevatorMoving = false;
        return;
    }

    float dt = Time::getDeltaTime();
    m_elevatorTimer += dt;
    float alpha = m_elevatorTimer / m_moveDuration;
    if (alpha > 1.0f)
        alpha = 1.0f;

    alpha = MathAPI::smoothStep(0.0f, 1.0f, alpha);

    Vector3 newPos = MathAPI::lerp(m_elevatorStartPos, m_elevatorTargetPos, alpha);
    TransformAPI::setGlobalPosition(elevatorTransform, newPos);

    if (m_elevatorTimer >= m_moveDuration)
        m_elevatorMoving = false;
}

IMPLEMENT_SCRIPT(ElevatorManager)
