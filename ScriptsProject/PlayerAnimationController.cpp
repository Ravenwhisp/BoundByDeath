#include "pch.h"
#include "PlayerAnimationController.h"

IMPLEMENT_SCRIPT_FIELDS(PlayerAnimationController,
    SERIALIZED_STRING(m_idleStateName, "Idle state name"),
    SERIALIZED_STRING(m_moveStateName, "Move state name"),
    SERIALIZED_STRING(m_dashStateName, "Dash state name"),
    SERIALIZED_STRING(m_attackStateName, "Attack state name"),
    SERIALIZED_STRING(m_attack2StateName, "Attack 2 state name"),
    SERIALIZED_STRING(m_chargeAttackStateName, "Charge attack state name"),
    SERIALIZED_STRING(m_damagedStateName, "Damaged state name"),
    SERIALIZED_STRING(m_harm1StateName, "Harm 1 state name"),
    SERIALIZED_STRING(m_harm2StateName, "Harm 2 state name"),
    SERIALIZED_STRING(m_harm3StateName, "Harm 3 state name"),
    SERIALIZED_STRING(m_downedStateName, "Downed state name"),
    SERIALIZED_STRING(m_deathStateName, "Death state name"),
    SERIALIZED_FLOAT(m_defaultBlendTime, "Default blend time", 0.0f, 2.0f, 0.01f),
    SERIALIZED_FLOAT(m_attackBlendTime, "Attack blend time", 0.0f, 2.0f, 0.01f),
    SERIALIZED_FLOAT(m_damagedBlendTime, "Damaged blend time", 0.0f, 2.0f, 0.01f),
    SERIALIZED_FLOAT(m_downedBlendTime, "Downed blend time", 0.0f, 2.0f, 0.01f),
    SERIALIZED_FLOAT(m_deathBlendTime, "Death blend time", 0.0f, 2.0f, 0.01f),
    SERIALIZED_FLOAT(m_attackLockDuration, "Attack lock duration", 0.0f, 3.0f, 0.01f)
)

PlayerAnimationController::PlayerAnimationController(GameObject* owner)
    : Script(owner)
{
}

void PlayerAnimationController::Start()
{
	m_animationComponent = findAnimationComponent();
}

void PlayerAnimationController::Update()
{
    if (!m_animationComponent)
    {
        return;
    }

    const float dt = Time::getDeltaTime();

    if (m_attackLockTimer > 0.0f)
    {
        m_attackLockTimer -= dt;
    }

    AnimState desiredState = AnimState::Idle;
    float blendTime = m_defaultBlendTime;

    if (m_isDead)
    {
        desiredState = AnimState::Death;
        blendTime = m_deathBlendTime;
    }
    else if (m_isDowned)
    {
        desiredState = AnimState::Downed;
        blendTime = m_downedBlendTime;
    }
    else if (m_damagedRequested)
    {
        desiredState = AnimState::Damaged;
        blendTime = m_damagedBlendTime;
    }
    else if (!m_pendingAttackStateName.empty())
    {
        desiredState = AnimState::Attack;
        blendTime = m_attackBlendTime;
    }
    else if (m_currentState == AnimState::Attack && m_attackLockTimer > 0.0f)
    {
        m_damagedRequested = false;
        return;
    }
    else if (m_isDashing)
    {
        desiredState = AnimState::Dash;
    }
    else if (m_isMoving)
    {
        desiredState = AnimState::Move;
    }
    else
    {
        desiredState = AnimState::Idle;
    }

    if (desiredState != m_currentState)
    {
        if (playAnimState(desiredState, blendTime))
        {
            m_currentState = desiredState;
        }
    }

    m_pendingAttackStateName.clear();
    m_damagedRequested = false;
}

void PlayerAnimationController::setMoving(bool moving)
{
    m_isMoving = moving;
}

void PlayerAnimationController::setDashing(bool dashing)
{
    m_isDashing = dashing;
}

void PlayerAnimationController::setDowned(bool downed)
{
    m_isDowned = downed;
}

void PlayerAnimationController::setDead(bool dead)
{
    m_isDead = dead;
}

void PlayerAnimationController::requestAttack()
{
    Debug::log("attack");
    m_pendingAttackStateName = m_attackStateName;
}

void PlayerAnimationController::requestBasicAttack(int comboStep)
{
    m_pendingAttackStateName = (comboStep >= 2) ? m_attack2StateName : m_attackStateName;
    m_attackLockTimer = m_attackLockDuration;
}

void PlayerAnimationController::requestChargedAttack()
{
    m_pendingAttackStateName = m_chargeAttackStateName;
    m_attackLockTimer = m_attackLockDuration;
}

void PlayerAnimationController::requestDamaged()
{
    std::string harmNames[3] = { m_harm1StateName, m_harm2StateName, m_harm3StateName };
    int validCount = 0;
    for (int i = 0; i < 3; ++i)
    {
        if (!harmNames[i].empty())
        {
            harmNames[validCount++] = harmNames[i];
        }
    }

    if (validCount > 0)
    {
        m_pendingDamageStateName = harmNames[rand() % validCount];
    }
    else
    {
        m_pendingDamageStateName = m_damagedStateName;
    }

    m_damagedRequested = true;
}

AnimationComponent* PlayerAnimationController::findAnimationComponent()
{
	m_animationComponent = AnimationAPI::getAnimationComponent(m_owner);
	if (m_animationComponent)
	{
		return m_animationComponent;
	}
	Debug::warn("CharacterAnimation on '%s' could not find an AnimationComponent on the same GameObject.", GameObjectAPI::getName(m_owner));
	return nullptr;
}

bool PlayerAnimationController::playAnimState(AnimState state, float blendTime)
{
    const char* stateName = nullptr;

    switch (state)
    {
    case AnimState::Idle:    stateName = m_idleStateName.c_str(); break;
    case AnimState::Move:    stateName = m_moveStateName.c_str(); break;
    case AnimState::Dash:    stateName = m_dashStateName.c_str(); break;
    case AnimState::Attack:  stateName = m_pendingAttackStateName.empty() ? m_attackStateName.c_str() : m_pendingAttackStateName.c_str(); break;
    case AnimState::Damaged: stateName = m_pendingDamageStateName.empty() ? m_damagedStateName.c_str() : m_pendingDamageStateName.c_str(); break;
    case AnimState::Downed:  stateName = m_downedStateName.c_str(); break;
    case AnimState::Death:   stateName = m_deathStateName.c_str(); break;
    default: return false;
    }

    if (!stateName || stateName[0] == '\0')
    {
        Debug::warn("PlayerAnimationController on '%s' has empty animation state name.", GameObjectAPI::getName(m_owner));
        return false;
    }

    const bool played = AnimationAPI::playState(m_animationComponent, stateName, blendTime);

    if (!played)
    {
        Debug::warn("PlayerAnimationController on '%s' could not play state '%s'.", GameObjectAPI::getName(m_owner), stateName);
    }

    return played;
}

IMPLEMENT_SCRIPT(PlayerAnimationController)