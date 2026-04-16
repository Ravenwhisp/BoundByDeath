#include "pch.h"
#include "LyrielBasicAttack.h"

#include "PlayerTargetController.h"
#include "ArrowPool.h"
#include "LyrielArrowProjectile.h"
#include "PlayerState.h"
#include "PlayerRotation.h"
#include "PlayerAnimationController.h"

static const ScriptFieldInfo LyrielBasicAttackFields[] =
{
    { "Player Index", ScriptFieldType::Int, offsetof(LyrielBasicAttack, m_playerIndex) },
    { "Attack Damage", ScriptFieldType::Float, offsetof(LyrielBasicAttack, m_attackDamage), { 0.0f, 100.0f, 0.5f } },
    { "Attack Cooldown", ScriptFieldType::Float, offsetof(LyrielBasicAttack, m_attackCooldown), { 0.0f, 5.0f, 0.05f } },
    { "Arrow Speed", ScriptFieldType::Float, offsetof(LyrielBasicAttack, m_arrowSpeed), { 0.0f, 100.0f, 0.5f } },
    { "Attack Lock Duration", ScriptFieldType::Float, offsetof(LyrielBasicAttack, m_attackLockDuration), { 0.0f, 2.0f, 0.01f } },
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

    Script* stateScript = GameObjectAPI::getScript(getOwner(), "PlayerState");
    m_playerState = dynamic_cast<PlayerState*>(stateScript);

    Script* rotationScript = GameObjectAPI::getScript(getOwner(), "PlayerRotation");
    m_playerRotation = dynamic_cast<PlayerRotation*>(rotationScript);

    Script* animationScript = GameObjectAPI::getScript(getOwner(), "PlayerAnimationController");
    m_playerAnimationController = dynamic_cast<PlayerAnimationController*>(animationScript);

    if (m_targetController == nullptr)
    {
        Debug::log("[LyrielBasicAttack] PlayerTargetController not found on owner.");
    }

    if (m_arrowPool == nullptr)
    {
        Debug::log("[LyrielBasicAttack] ArrowPool not found on owner.");
    }

    if (m_playerState == nullptr)
    {
        Debug::log("[LyrielBasicAttack] PlayerState not found on owner.");
    }

    if (m_playerRotation == nullptr)
    {
        Debug::log("[LyrielBasicAttack] PlayerRotation not found on owner.");
    }

    if (m_playerAnimationController == nullptr)
    {
        Debug::log("[LyrielBasicAttack] PlayerAnimationController not found on owner.");
    }
}

void LyrielBasicAttack::Update()
{
    updateCooldown();

    if (m_attackStateTimer > 0.0f)
    {
        if (m_attackFacingTarget != nullptr)
        {
            faceTarget(m_attackFacingTarget);
        }
    }

    updateAttackStateTimer();

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

    if (m_playerState != nullptr && m_playerState->isDowned())
    {
        return;
    }

    GameObject* target = m_targetController->getCurrentTarget();
    if (target == nullptr)
    {
        Debug::log("[LyrielBasicAttack] No current target.");
        return;
    }

    faceTarget(target);
    m_attackFacingTarget = target;

    if (m_playerState != nullptr)
    {
        m_playerState->setState(PlayerStateType::Attacking);
    }

    if (m_playerAnimationController != nullptr)
    {
        m_playerAnimationController->requestAttack();
    }

    m_attackStateTimer = m_attackLockDuration;

    spawnArrowToTarget(target);

    m_cooldownTimer = m_attackCooldown;

    Debug::log("[LyrielBasicAttack] Shot arrow to target '%s'.", GameObjectAPI::getName(target));
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

    arrow->launch(startPosition, direction, m_arrowSpeed, arrowLifetime, target, m_attackDamage);
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

void LyrielBasicAttack::faceTarget(GameObject* target)
{
    if (m_playerRotation == nullptr || target == nullptr)
    {
        return;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    Transform* targetTransform = GameObjectAPI::getTransform(target);

    Vector3 ownerPosition = TransformAPI::getGlobalPosition(ownerTransform);
    Vector3 targetPosition = TransformAPI::getGlobalPosition(targetTransform);

    Vector3 direction = targetPosition - ownerPosition;
    direction.y = 0.0f;
    if (direction.LengthSquared() <= 0.0001f)
    {
        return;
    }
    direction.Normalize();

    m_playerRotation->applyFacingFromDirection(getOwner(), direction, Time::getDeltaTime());
}

void LyrielBasicAttack::updateCooldown()
{
    if (m_cooldownTimer <= 0.0f)
    {
        return;
    }

    m_cooldownTimer -= Time::getDeltaTime();
    if (m_cooldownTimer < 0.0f)
    {
        m_cooldownTimer = 0.0f;
    }
}

void LyrielBasicAttack::updateAttackStateTimer()
{
    if (m_attackStateTimer <= 0.0f)
    {
        return;
    }

    m_attackStateTimer -= Time::getDeltaTime();
    if (m_attackStateTimer <= 0.0f)
    {
        m_attackStateTimer = 0.0f;
        m_attackFacingTarget = nullptr;

        if (m_playerState != nullptr && m_playerState->isAttacking())
        {
            m_playerState->setState(PlayerStateType::Normal);
        }
    }
}

IMPLEMENT_SCRIPT(LyrielBasicAttack)