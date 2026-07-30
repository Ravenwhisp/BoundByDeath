#include "pch.h"
#include "AelorinDamageable.h"

#include "AelorinBossController.h"

AelorinDamageable::AelorinDamageable(GameObject* owner)
    : EnemyDamageable(owner)
{
}

void AelorinDamageable::Start()
{
    EnemyDamageable::Start();

    m_controller = GameObjectAPI::findScript<AelorinBossController>(getOwner());

    if (!m_controller)
    {
        Debug::warn("[AelorinDamageable] AelorinBossController not found.");
    }

    Debug::log("[AelorinDamageable] Started.");
}

void AelorinDamageable::takeDamage(const HitContext& ctx)
{
    EnemyDamageable::takeDamage(ctx);
}

void AelorinDamageable::onHpDepleted()
{
    Debug::log("HP depleted");
    
    changePhase();
}

void AelorinDamageable::changePhase()
{
    if (!m_controller)
    {
        return;
    }

    if (m_controller->getPhase() == Phase::Phase1)
    {
        m_controller->updateBossPhase();
        m_currentHp = getMaxHp();
        return;
    }
}

bool AelorinDamageable::canChangePhases() const
{
    if (!m_controller)
    {
        return false;
    }

    if (m_currentHp <= 0.0f && m_controller->getPhase() == Phase::Phase1)
    {
        return true;
    }

    return false;
}

IMPLEMENT_SCRIPT(AelorinDamageable)