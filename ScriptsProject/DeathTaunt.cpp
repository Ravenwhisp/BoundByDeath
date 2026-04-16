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
    { "Ability Cooldown", ScriptFieldType::Float, offsetof(DeathTaunt, m_TauntCooldownSeconds), { 1.0f, 10.0f, 0.05f } },
    { "Ability Duration", ScriptFieldType::Float, offsetof(DeathTaunt, m_TauntDurationSeconds), { 1.0f, 10.0f, 0.05f } },
    { "Cone Range", ScriptFieldType::Float, offsetof(DeathTaunt, m_TauntRange), { 1.0f, 10.0f, 0.1f } },
    { "Cone Angle", ScriptFieldType::Float, offsetof(DeathTaunt, m_TauntHalfAngleDegrees), { 1.0f, 180.0f, 1.0f } }
    
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

    const bool leftTriggerPressed = Input::isLeftTriggerJustPressed(getPlayerIndex()); 
    bool canActivateNow = canActivate();
    bool isActiveNow = isActive();

    if (!isActiveNow && canActivateNow && (leftTriggerPressed))
    {
        onActivate();
        applyTauntToEnemiesInCone();
        m_debugConeTimer = 0.25f;
        onDeactivate();
    }

    if (m_debugConeTimer > 0.0f)
    {
        m_debugConeTimer -= Time::getDeltaTime();
        if (m_debugConeTimer < 0.0f)
        {
            m_debugConeTimer = 0.0f;
        }
    }
}

void DeathTaunt::drawGizmo()
{
    if (m_debugConeTimer <= 0.0f)
    {
        return;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(m_owner);
    if (ownerTransform == nullptr)
    {
        return;
    }

    const Vector3 ownerPosition = TransformAPI::getPosition(ownerTransform);
    const Vector3 ownerForward = getHorizontalForward(ownerTransform);

    const float clampedHalfAngle = (m_TauntHalfAngleDegrees < 0.1f) ? 0.1f : ((m_TauntHalfAngleDegrees > 89.9f) ? 89.9f : m_TauntHalfAngleDegrees);
    const float halfAngleRadians = clampedHalfAngle * (3.14159265f / 180.0f);

    // Draw the sector as lines from center to arc points
    const int numSteps = 16;
    const float angleStep = (2.0f * halfAngleRadians) / (numSteps - 1);
    const Vector3 color(1.0f, 0.0f, 0.0f); // Red color

    for (int i = 0; i < numSteps; ++i)
    {
        float angle = -halfAngleRadians + i * angleStep;
        Vector3 direction = ownerForward;
        direction = Vector3(
            direction.x * std::cos(angle) - direction.z * std::sin(angle),
            0.0f,
            direction.x * std::sin(angle) + direction.z * std::cos(angle)
        );
        direction.Normalize();
        Vector3 arcPoint = ownerPosition + direction * m_TauntRange;
        DebugDrawAPI::drawLine(ownerPosition, arcPoint, color, 0, false);
    }
}

void DeathTaunt::onFieldEdited(const ScriptFieldInfo& field)
{
    if (strcmp(field.name, "Cone Range") == 0 || strcmp(field.name, "Cone Angle") == 0)
    {
        m_debugConeTimer = 1.0f;
    }
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
