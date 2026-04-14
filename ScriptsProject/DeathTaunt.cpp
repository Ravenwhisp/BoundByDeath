#include "pch.h"
#include "DeathTaunt.h"
#include "CharacterBase.h"
#include "DeathCharacter.h"
#include "EnemyDetectionAggro.h"

#include <cmath>

static const ScriptFieldInfo DeathTauntFields[] =
{
    { "FuckOFF Window", ScriptFieldType::Float, offsetof(DeathTaunt, m_TauntCooldownSeconds), { 0.1f, 3.0f, 0.05f } },
};

IMPLEMENT_SCRIPT_FIELDS(DeathTaunt, DeathTauntFields)

DeathTaunt::DeathTaunt(GameObject* owner)
    : AbilityBase(owner)
{
    m_cooldown = m_TauntCooldownSeconds;
}

void DeathTaunt::Start()
{
    m_character = static_cast<CharacterBase*>(
        GameObjectAPI::getScript(m_owner, "DeathCharacter"));

    if (m_character == nullptr)
    {
        Debug::warn("DeathTaunt: DeathCharacter not found on this GameObject.");
    }
}

void DeathTaunt::Update()
{
    // MUST be called first — handles dead guard, force-cancel on death, cooldown tick.
    AbilityBase::Update();

    if (m_character == nullptr)
    {
        return;
    }

    bool leftTriggerPressed = Input::isLeftTriggerJustPressed(getPlayerIndex());
    bool tKeyPressed = Input::isKeyDown(KeyCode::T);//TODO: Delete after finish testing
    bool canActivateNow = canActivate();
    bool isActiveNow = isActive();


    if (!isActiveNow && canActivateNow && (leftTriggerPressed || tKeyPressed))
    {
        onActivate();
        applyTauntToEnemiesInCone();
        onDeactivate();
    }
}

void DeathTaunt::drawGizmo()
{
    Transform* ownerTransform = GameObjectAPI::getTransform(m_owner);
    if (ownerTransform == nullptr)
    {
        return;
    }

    const Vector3 ownerPosition = TransformAPI::getPosition(ownerTransform);
    const Vector3 ownerForward = TransformAPI::getForward(ownerTransform);

    // Dibujar el cono siempre visible
    DebugDrawAPI::drawCone(ownerPosition, ownerForward * m_TauntRange, Vector3(1.0f, 0.0f, 0.0f), m_TauntRange, 0.0f, 0, false);
}

void DeathTaunt::onActivate()
{
    AbilityBase::onActivate();

    // Taunt applies its effect instantly and should not lock Death during the taunt duration.
    m_character->setCanAct(true);
}

void DeathTaunt::onDeactivate()
{
    const bool restoreCanAct = m_character != nullptr && !m_character->isDead();

    AbilityBase::onDeactivate();

    if (restoreCanAct)
    {
        m_character->setCanAct(true);
    }
}

void DeathTaunt::applyTauntToEnemiesInCone() const
{
    Transform* ownerTransform = GameObjectAPI::getTransform(m_owner);
    if (ownerTransform == nullptr)
    {
        return;
    }

    const Vector3 ownerPosition = TransformAPI::getPosition(ownerTransform);
    const Vector3 ownerForward = TransformAPI::getForward(ownerTransform);

    const std::vector<GameObject*> enemies = SceneAPI::findAllGameObjectsByTag(Tag::ENEMY);
    for (GameObject* enemy : enemies)
    {
        if (!isEnemyInsideTauntCone(enemy, ownerPosition, ownerForward))
        {
            continue;
        }

        Script* script = GameObjectAPI::getScript(enemy, "EnemyDetectionAggro");
        if (script == nullptr)
        {
            continue;
        }

        static_cast<EnemyDetectionAggro*>(script)->applyTaunt(ownerTransform, m_TauntDurationSeconds);
    }
}

bool DeathTaunt::isEnemyInsideTauntCone(GameObject* enemy, const Vector3& ownerPosition, const Vector3& ownerForward) const
{
    if (enemy == nullptr)
    {
        return false;
    }

    Transform* enemyTransform = GameObjectAPI::getTransform(enemy);
    if (enemyTransform == nullptr)
    {
        return false;
    }

    Vector3 directionToEnemy = TransformAPI::getPosition(enemyTransform) - ownerPosition;
    directionToEnemy.y = 0.0f;

    const float distanceToEnemy = directionToEnemy.Length();
    if (distanceToEnemy <= 0.0f || distanceToEnemy > m_TauntRange)
    {
        return false;
    }

    Vector3 flattenedForward = ownerForward;
    flattenedForward.y = 0.0f;

    if (flattenedForward.LengthSquared() <= 0.0001f)
    {
        return false;
    }

    flattenedForward.Normalize();
    directionToEnemy.Normalize();

    const float halfAngleRadians = m_TauntHalfAngleDegrees * (3.14159265f / 180.0f);
    const float coneThreshold = std::cos(halfAngleRadians);

    // TODO: Add a line-of-sight / wall check before confirming the taunt hit.
    return flattenedForward.Dot(directionToEnemy) >= coneThreshold;
}

IMPLEMENT_SCRIPT(DeathTaunt)
