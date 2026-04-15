#include "pch.h"
#include "DeathTaunt.h"
#include "CharacterBase.h"
#include "DeathCharacter.h"
#include "EnemyDetectionAggro.h"

#include <cmath>

static Vector3 getHorizontalForward(const Transform* transform)
{
    Vector3 forward = TransformAPI::getForward(transform);
    forward.y = 0.0f;

    if (forward.LengthSquared() <= 0.0001f)
    {
        const Vector3 euler = TransformAPI::getEulerDegrees(transform);
        const float radians = euler.y * (3.14159265f / 180.0f);
        forward = Vector3(std::sin(radians), 0.0f, std::cos(radians));
    }

    forward.Normalize();
    return forward;
}

static const ScriptFieldInfo DeathTauntFields[] =
{
    { "Cooldown", ScriptFieldType::Float, offsetof(DeathTaunt, m_TauntCooldownSeconds), { 1.0f, 10.0f, 0.05f } },
    { "Duration", ScriptFieldType::Float, offsetof(DeathTaunt, m_TauntDurationSeconds), { 1.0f, 10.0f, 0.05f } },
    { "Range", ScriptFieldType::Float, offsetof(DeathTaunt, m_TauntRange), { 1.0f, 10.0f, 0.1f } },
    { "Half Angle Degrees", ScriptFieldType::Float, offsetof(DeathTaunt, m_TauntHalfAngleDegrees), { 1.0f, 180.0f, 1.0f } }
    
};

IMPLEMENT_SCRIPT_FIELDS(DeathTaunt, DeathTauntFields)

DeathTaunt::DeathTaunt(GameObject* owner)
    : AbilityBase(owner)
{
}

void DeathTaunt::Start()
{
    m_cooldown = (m_TauntCooldownSeconds < 0.0f ? 0.0f : m_TauntCooldownSeconds);

    m_character = static_cast<CharacterBase*>(
        GameObjectAPI::getScript(m_owner, "DeathCharacter"));

    if (m_character == nullptr)
    {
        Debug::warn("DeathTaunt: DeathCharacter not found on this GameObject.");
    }
}

void DeathTaunt::Update()
{
    m_cooldown = (m_TauntCooldownSeconds < 0.0f ? 0.0f : m_TauntCooldownSeconds);

    // MUST be called first — handles dead guard, force-cancel on death, cooldown tick.
    AbilityBase::Update();

    if (m_character == nullptr)
    {
        return;
    }

    bool leftTriggerPressed = Input::isLeftTriggerJustPressed(getPlayerIndex()); //TODO:Check Index
    bool canActivateNow = canActivate();
    bool isActiveNow = isActive();


    if (!isActiveNow && canActivateNow && (leftTriggerPressed))
    {
        onActivate();
        Debug::log("DeathTaunt activated - Cooldown: %.2f, Duration: %.2f, Range: %.2f, Half Angle: %.2f",
            m_TauntCooldownSeconds, m_TauntDurationSeconds, m_TauntRange, m_TauntHalfAngleDegrees);
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
    const Vector3 ownerForward = getHorizontalForward(ownerTransform);

    const float clampedHalfAngle = (m_TauntHalfAngleDegrees < 0.1f) ? 0.1f : ((m_TauntHalfAngleDegrees > 89.9f) ? 89.9f : m_TauntHalfAngleDegrees);
    const float halfAngleRadians = clampedHalfAngle * (3.14159265f / 180.0f);
    const float baseRadius = m_TauntRange * std::tan(halfAngleRadians);

    DebugDrawAPI::drawCone(ownerPosition, ownerForward * m_TauntRange, Vector3(1.0f, 0.0f, 0.0f), baseRadius, 0.0f, 0, false);
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
    const Vector3 ownerForward = getHorizontalForward(ownerTransform);

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

    if (ownerForward.LengthSquared() <= 0.0001f)
    {
        return false;
    }

    Vector3 flattenedForward = ownerForward;
    flattenedForward.Normalize();
    directionToEnemy.Normalize();

    const float halfAngleRadians = m_TauntHalfAngleDegrees * (3.14159265f / 180.0f);
    const float coneThreshold = std::cos(halfAngleRadians);

    // TODO: Add a line-of-sight / wall check before confirming the taunt hit.
    return flattenedForward.Dot(directionToEnemy) >= coneThreshold;
}

IMPLEMENT_SCRIPT(DeathTaunt)
