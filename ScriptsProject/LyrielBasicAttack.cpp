#include "pch.h"
#include "LyrielBasicAttack.h"

#include "PlayerTargetController.h"
#include "ArrowPool.h"
#include "LyrielArrowProjectile.h"
#include "Damageable.h"

static const ScriptFieldInfo LyrielBasicAttackFields[] =
{
    { "Player Index", ScriptFieldType::Int, offsetof(LyrielBasicAttack, m_playerIndex) },
    { "Attack Damage", ScriptFieldType::Float, offsetof(LyrielBasicAttack, m_attackDamage), { 0.0f, 100.0f, 0.5f } },
    { "Attack Cooldown", ScriptFieldType::Float, offsetof(LyrielBasicAttack, m_attackCooldown), { 0.0f, 5.0f, 0.05f } },
    { "Arrow Speed", ScriptFieldType::Float, offsetof(LyrielBasicAttack, m_arrowSpeed), { 0.0f, 100.0f, 0.5f } },
    { "Arrow Spawn Child Name", ScriptFieldType::String, offsetof(LyrielBasicAttack, m_arrowSpawnChildName) }
};

IMPLEMENT_SCRIPT_FIELDS(LyrielBasicAttack, LyrielBasicAttackFields)

LyrielBasicAttack::LyrielBasicAttack(GameObject* owner)
    : Script(owner)
{
}

void LyrielBasicAttack::Start()
{
    Script* targetControllerScript = GameObjectAPI::getScript(getOwner(), "PlayerTargetController");
    m_targetController = dynamic_cast<PlayerTargetController*>(targetControllerScript);

    Script* arrowPoolScript = GameObjectAPI::getScript(getOwner(), "ArrowPool");
    m_arrowPool = dynamic_cast<ArrowPool*>(arrowPoolScript);

    if (m_targetController == nullptr)
    {
        Debug::log("[LyrielBasicAttack] PlayerTargetController not found on owner.");
    }

    if (m_arrowPool == nullptr)
    {
        Debug::log("[LyrielBasicAttack] ArrowPool not found on owner.");
    }
}

void LyrielBasicAttack::Update()
{
    if (m_cooldownTimer > 0.0f)
    {
        m_cooldownTimer -= Time::getDeltaTime();
        if (m_cooldownTimer < 0.0f)
        {
            m_cooldownTimer = 0.0f;
        }
    }

    if (Input::isRightShoulderJustPressed(m_playerIndex))
    {
        tryAttack();
    }
}

void LyrielBasicAttack::tryAttack()
{
    if (m_cooldownTimer > 0.0f)
    {
        return;
    }

    if (m_targetController == nullptr)
    {
        return;
    }

    GameObject* target = m_targetController->getCurrentTarget();
    if (target == nullptr)
    {
        Debug::log("[LyrielBasicAttack] No current target.");
        return;
    }

    Script* script = GameObjectAPI::getScript(target, "Damageable");
    Damageable* damageable = dynamic_cast<Damageable*>(script);
    if (damageable == nullptr)
    {
        return;
    }

    damageable->takeDamage(m_attackDamage);
    spawnArrowToTarget(target);

    m_cooldownTimer = m_attackCooldown;

    Debug::log("[LyrielBasicAttack] Attacked target '%s' for %.2f damage.", GameObjectAPI::getName(target), m_attackDamage);
}

void LyrielBasicAttack::spawnArrowToTarget(GameObject* target)
{
    if (m_arrowPool == nullptr || target == nullptr)
    {
        return;
    }

    LyrielArrowProjectile* arrow = m_arrowPool->acquireArrow();
    if (arrow == nullptr)
    {
        return;
    }

    Transform* spawnTransform = findArrowSpawnTransform();
    Transform* targetTransform = GameObjectAPI::getTransform(target);

    if (spawnTransform == nullptr || targetTransform == nullptr)
    {
        return;
    }

    const Vector3 startPosition = TransformAPI::getGlobalPosition(spawnTransform);
    const Vector3 targetPosition = TransformAPI::getGlobalPosition(targetTransform);

    Vector3 direction = targetPosition - startPosition;
    Debug::log("[LyrielBasicAttack] Start Pos: (%.2f, %.2f, %.2f) | Target Pos: (%.2f, %.2f, %.2f)",
        startPosition.x, startPosition.y, startPosition.z,
        targetPosition.x, targetPosition.y, targetPosition.z);
    const float distance = direction.Length();

    if (distance <= 0.0001f)
    {
        direction = TransformAPI::getForward(spawnTransform);
    }
    else
    {
        direction.Normalize();
    }

    float arrowLifetime = distance / m_arrowSpeed;

    arrow->launch(startPosition, direction, m_arrowSpeed, arrowLifetime);
}

Transform* LyrielBasicAttack::findArrowSpawnTransform() const
{
    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());

    if (!m_arrowSpawnChildName.empty())
    {
        Transform* spawnTransform = TransformAPI::findChildByName(ownerTransform, m_arrowSpawnChildName.c_str());
        if (spawnTransform != nullptr)
        {
            return spawnTransform;
        }
    }

    return ownerTransform;
}

IMPLEMENT_SCRIPT(LyrielBasicAttack)