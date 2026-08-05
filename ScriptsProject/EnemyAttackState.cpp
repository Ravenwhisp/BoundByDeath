#include "pch.h"
#include "EnemyAttackState.h"

#include "EnemyBaseController.h"
#include "EnemyBaseAttackConfig.h"
#include "EnemyAttackExecutor.h"
#include "EnemySound.h"
#include "PaladinAttackConfig.h"

#include "Damageable.h"
#include "PlayerState.h"
#include "PaladinVFX.h"

EnemyAttackState::EnemyAttackState(GameObject* owner)
    : StateMachineScript(owner)
{
}

void EnemyAttackState::OnStateEnter()
{
    m_controller =
        GameObjectAPI::findScript<EnemyBaseController>(getOwner());

    m_attackExecutor =
        GameObjectAPI::findScript<EnemyAttackExecutor>(getOwner());

    m_animation =
        AnimationAPI::getAnimationComponent(getOwner());

    m_paladinVFX =
        GameObjectAPI::findScript<PaladinVFX>(getOwner());

    m_stateTimer = 0.0f;
    m_hasAppliedDamage = false;
    m_committedTarget = nullptr;
    m_usePaladinAreaAttack = false;
    m_lockedAttackOrigin = Vector3::Zero;
    m_lockedAttackDirection = Vector3::Zero;
    m_lockedAttackRotation = Vector3::Zero;

    if (m_paladinVFX)
    {
        m_paladinVFX->stopBasicAttackTelegraph();
    }

    if (!m_controller)
    {
        Debug::error(
            "[EnemyAttackState] EnemyController not found."
        );
        return;
    }

    if (!m_animation)
    {
        Debug::error(
            "[EnemyAttackState] AnimationComponent not found."
        );
        return;
    }

    m_controller->clearPath();
    m_controller->resetRepathTimer();

    m_controller->updateCurrentTarget();
    m_committedTarget = m_controller->getCurrentTarget();

    if (m_paladinVFX &&
        m_attackExecutor &&
        m_committedTarget)
    {
        lockPaladinAttackArea();
    }

    m_enemySound =
        GameObjectAPI::findScript<EnemySound>(getOwner());

    if (m_enemySound)
    {
        m_enemySound->playBasicTelegraph();
    }

    Debug::log("[EnemyAttackState] ENTER");
}

void EnemyAttackState::OnStateUpdate()
{
    if (!m_controller ||
        !m_controller->getAttackConfig() ||
        !m_animation)
    {
        return;
    }

    if (m_controller->trySendDeathTrigger(m_animation))
    {
        return;
    }

    if (m_controller->trySendStunTrigger(m_animation))
    {
        return;
    }

    if (!m_usePaladinAreaAttack)
    {
        m_controller->faceCurrentTarget();
    }

    m_stateTimer += Time::getDeltaTime();

    if (!m_hasAppliedDamage &&
        m_stateTimer >=
        m_controller
        ->getAttackConfig()
        ->m_basicAttackWindupTime)
    {
        if (m_usePaladinAreaAttack && m_paladinVFX)
        {
            m_paladinVFX->stopBasicAttackTelegraph();
        }

        playBasicAttackEffect();

        if (m_usePaladinAreaAttack)
        {
            applyPaladinAreaDamage();
        }
        else
        {
            tryDamageTarget(m_committedTarget);
        }

        if (m_enemySound)
        {
            m_enemySound->playBasicImpact();
        }

        m_hasAppliedDamage = true;
    }

    if (m_stateTimer >=
        m_controller
        ->getAttackConfig()
        ->m_basicAttackTotalDuration)
    {
        m_controller->updateCurrentTarget();

        if (!m_controller->hasValidTarget())
        {
            AnimationAPI::sendTrigger(
                m_animation,
                "ToIdle"
            );

            Debug::log(
                "[EnemyAttackState] Attack finished, Idle trigger sent"
            );
        }
        else
        {
            AnimationAPI::sendTrigger(
                m_animation,
                "ToChase"
            );

            Debug::log(
                "[EnemyAttackState] Attack finished, Chase trigger sent"
            );
        }

        return;
    }
}

void EnemyAttackState::OnStateExit()
{
    if (m_paladinVFX)
    {
        m_paladinVFX->stopBasicAttackTelegraph();
    }

    m_usePaladinAreaAttack = false;

    Debug::log("[EnemyAttackState] EXIT");
}

void EnemyAttackState::lockPaladinAttackArea()
{
    if (!m_controller ||
        !m_controller->getAttackConfig() ||
        !m_committedTarget ||
        !m_paladinVFX)
    {
        return;
    }

    Transform* ownerTransform =
        GameObjectAPI::getTransform(getOwner());

    if (!ownerTransform)
    {
        return;
    }

    m_controller->faceCurrentTarget();

    Vector3 direction =
        TransformAPI::getForward(ownerTransform);

    direction.y = 0.0f;

    if (direction.LengthSquared() < 0.0001f)
    {
        return;
    }

    direction.Normalize();

    const PaladinAttackConfig* paladinConfig =
        static_cast<const PaladinAttackConfig*>(
            m_controller->getAttackConfig()
            );

    const Vector3 ownerPosition =
        TransformAPI::getGlobalPosition(ownerTransform);

    m_lockedAttackDirection = direction;

    m_lockedAttackOrigin =
        ownerPosition +
        direction *
        paladinConfig->m_basicAttackForwardOffset;

    m_lockedAttackRotation =
        TransformAPI::getGlobalEulerDegrees(ownerTransform);

    const Vector3 telegraphPosition =
        m_lockedAttackOrigin +
        direction *
        (paladinConfig->m_basicAttackRange * 0.5f);

    m_paladinVFX->startBasicAttackTelegraph(
        telegraphPosition,
        m_lockedAttackRotation
    );

    m_usePaladinAreaAttack = true;
}

void EnemyAttackState::applyPaladinAreaDamage()
{
    if (!m_controller ||
        !m_controller->getAttackConfig() ||
        !m_attackExecutor)
    {
        return;
    }

    const PaladinAttackConfig* paladinConfig =
        static_cast<const PaladinAttackConfig*>(
            m_controller->getAttackConfig()
            );

    m_attackExecutor->applyDamageInRectangle(
        m_lockedAttackOrigin,
        m_lockedAttackDirection,
        paladinConfig->m_basicAttackRange,
        paladinConfig->m_basicAttackWidth,
        paladinConfig->m_basicAttackDamage,
        "PaladinBasicAttack"
    );
}

void EnemyAttackState::tryDamageTarget(
    Transform* targetTransform
)
{
    if (!m_controller->getAttackConfig())
    {
        return;
    }

    if (!targetTransform)
    {
        return;
    }

    GameObject* targetObject =
        ComponentAPI::getOwner(targetTransform);

    if (!targetObject)
    {
        return;
    }

    PlayerState* playerState =
        GameObjectAPI::findScript<PlayerState>(targetObject);

    if (playerState && playerState->isDowned())
    {
        return;
    }

    Damageable* damageable =
        GameObjectAPI::findScript<Damageable>(targetObject);

    if (!damageable)
    {
        return;
    }

    damageable->takeDamage(
        m_controller
        ->getAttackConfig()
        ->m_basicAttackDamage
    );

    Debug::log(
        "[EnemyAttackState] Damaged '%s' for %.2f.",
        GameObjectAPI::getName(targetObject),
        m_controller
        ->getAttackConfig()
        ->m_basicAttackDamage
    );
}

void EnemyAttackState::playBasicAttackEffect()
{
    if (m_paladinVFX)
    {
        m_paladinVFX->playBasicAttackEffect();
    }
}

IMPLEMENT_SCRIPT(EnemyAttackState)