#include "pch.h"
#include "DeathBasicAttack.h"
#include "CharacterBase.h"
#include "DeathCharacter.h"
#include "PlayerTargetController.h"
#include "PlayerState.h"
#include "PlayerAnimationController.h"
#include "PlayerRotation.h"

#include <cmath>

static const ScriptFieldInfo DeathBasicAttackFields[] =
{
    { "Attack Lock Duration", ScriptFieldType::Float, offsetof(DeathBasicAttack, m_attackLockDuration), { 0.05f, 2.0f, 0.05f } },
};

IMPLEMENT_SCRIPT_FIELDS(DeathBasicAttack, DeathBasicAttackFields)

DeathBasicAttack::DeathBasicAttack(GameObject* owner)
    : AbilityBase(owner)
{
}

void DeathBasicAttack::Start()
{
    m_character = static_cast<CharacterBase*>(
        GameObjectAPI::getScript(m_owner, "DeathCharacter"));

    m_playerState = static_cast<PlayerState*>(
        GameObjectAPI::getScript(m_owner, "PlayerState"));

    m_animController = static_cast<PlayerAnimationController*>(
        GameObjectAPI::getScript(m_owner, "PlayerAnimationController"));

    m_playerRotation = static_cast<PlayerRotation*>(
        GameObjectAPI::getScript(m_owner, "PlayerRotation"));

    if (m_character == nullptr)
        Debug::warn("DeathBasicAttack: DeathCharacter not found on this GameObject.");
    if (m_playerState == nullptr)
        Debug::warn("DeathBasicAttack: PlayerState not found on this GameObject.");
    if (m_animController == nullptr)
        Debug::warn("DeathBasicAttack: PlayerAnimationController not found on this GameObject.");
    if (m_playerRotation == nullptr)
        Debug::warn("DeathBasicAttack: PlayerRotation not found on this GameObject.");
}

void DeathBasicAttack::Update()
{
    AbilityBase::Update();

    if (m_character == nullptr)
    {
        return;
    }

    DeathCharacter* deathChar = static_cast<DeathCharacter*>(m_character);
    const float dt = Time::getDeltaTime();

    // --- Attack lock tick ---
    // The ability stays active (canAct = false) for m_attackLockDuration after each hit.
    // This gives the animation time to play before the next R1 is accepted.
    if (m_attackLockTimer > 0.0f)
    {
        faceTarget(m_attackFacingTarget);

        // requestAttack() is a one-frame flag — call it every frame during the lock
        // so PlayerAnimationController stays in Attack state until the swing finishes.
        if (m_animController != nullptr)
            m_animController->requestAttack();

        m_attackLockTimer -= dt;
        if (m_attackLockTimer <= 0.0f)
        {
            m_attackLockTimer    = 0.0f;
            m_attackFacingTarget = nullptr;
            if (m_playerState != nullptr)
                m_playerState->setState(PlayerStateType::Normal);
            onDeactivate();
        }
        return;
    }

    // --- Input ---
    if (!Input::isRightShoulderJustPressed(getPlayerIndex()))
    {
        return;
    }

    if (!canActivate())
    {
        if (m_character->isDead())
            Debug::log("[R1] bloqueado — personaje muerto");
        else if (!m_character->canAct())
            Debug::log("[R1] bloqueado — otra ability activa");
        else
            Debug::log("[R1] bloqueado — cooldown o desactivado");
        return;
    }

    // --- Execute hit ---
    GameObject* target = m_character->getTargetController()
        ? m_character->getTargetController()->getCurrentTarget()
        : nullptr;

    faceTarget(target);          // snap towards target (or stay forward if no target)
    m_attackFacingTarget = target;  // keep rotating during the lock

    onActivate();  // sets isActive = true, canAct = false

    const int   comboStep = deathChar->getComboStep();
    const float damage    = deathChar->m_basicAttackDamage;

    deathChar->dealDamageInArc(damage);
    deathChar->advanceCombo(false);

    m_attackLockTimer = m_attackLockDuration;

    if (m_animController != nullptr)
        m_animController->requestAttack();

    if (m_playerState != nullptr)
        m_playerState->setState(PlayerStateType::Attacking);

    if (comboStep < 2)
        Debug::log("[COMBO] R1  step %d/3  lock=%.2fs", comboStep + 1, m_attackLockDuration);
    else
        Debug::log("[COMBO] R1  step 3/3  COMPLETO — reset");
}

void DeathBasicAttack::drawGizmo()
{
    if (m_character == nullptr)
    {
        return;
    }

    const Transform* t = GameObjectAPI::getTransform(m_owner);
    if (t == nullptr)
    {
        return;
    }

    DeathCharacter* deathChar = static_cast<DeathCharacter*>(m_character);

    const Vector3 pos   = TransformAPI::getPosition(t);
    const Vector3 fwd   = TransformAPI::getForward(t);
    const float   range = deathChar->m_arcRange;
    const float   angle = deathChar->m_arcAngle;

    // fill = 1.0 right after a hit, drains over DeathCharacter::m_comboWindow.
    // Resets to 1.0 if the next hit lands before it empties.
    // Snaps to 0 after the 3rd hit or when the window expires.
    const float fill = deathChar->getComboFillRatio();

    constexpr float k_degToRad = 3.14159265f / 180.0f;
    const float halfRad = angle * 0.5f * k_degToRad;
    const float cosA    = cosf(halfRad);
    const float sinA    = sinf(halfRad);

    const Vector3 posFlat  = { pos.x, pos.y, pos.z };
    const Vector3 leftRay  = { fwd.x * cosA + fwd.z * sinA,  0.0f, -fwd.x * sinA + fwd.z * cosA };
    const Vector3 rightRay = { fwd.x * cosA - fwd.z * sinA,  0.0f,  fwd.x * sinA + fwd.z * cosA };

    const Vector3 colGrey   = { 0.35f, 0.35f, 0.35f };
    const Vector3 colPurple = { 0.9f,  0.0f,  0.9f  };

    // Helper: direction vector rotated 'a' radians around Y from forward.
    auto radialDir = [&](float a) -> Vector3
    {
        return Vector3(
            fwd.x * cosf(a) + fwd.z * sinf(a),
            0.0f,
            -fwd.x * sinf(a) + fwd.z * cosf(a));
    };

    // Boundary rays — always grey outline.
    DebugDrawAPI::drawLine(posFlat, posFlat + leftRay  * range, colGrey);
    DebugDrawAPI::drawLine(posFlat, posFlat + rightRay * range, colGrey);

    // Arc edge — 16 grey segments.
    const int   edgeSegs   = 16;
    const float arcStep    = (angle * k_degToRad) / static_cast<float>(edgeSegs);
    for (int i = 0; i < edgeSegs; ++i)
    {
        const float a0 = -halfRad + arcStep * static_cast<float>(i);
        const float a1 = a0 + arcStep;
        DebugDrawAPI::drawLine(posFlat + radialDir(a0) * range,
                               posFlat + radialDir(a1) * range, colGrey);
    }

    // Interior fill — radial lines sweeping left→right.
    // At fill=1.0 the whole cone is covered; at fill=0.0 nothing is drawn.
    // As the combo window drains the filled region shrinks from the right edge.
    if (fill > 0.0f)
    {
        const int   fillLines    = 24;  // density of radial lines
        const float filledAngle  = fill * 2.0f * halfRad;  // angle range to cover

        for (int i = 0; i <= fillLines; ++i)
        {
            const float t = static_cast<float>(i) / static_cast<float>(fillLines);
            const float a = -halfRad + t * filledAngle;
            DebugDrawAPI::drawLine(posFlat, posFlat + radialDir(a) * range, colPurple);
        }

        // Overdraw boundary rays in purple only for the filled side.
        DebugDrawAPI::drawLine(posFlat, posFlat + leftRay * range, colPurple);
    }
}

void DeathBasicAttack::faceTarget(GameObject* target)
{
    if (m_playerRotation == nullptr || target == nullptr)
    {
        return;
    }

    Transform* myTransform     = GameObjectAPI::getTransform(m_owner);
    Transform* targetTransform = GameObjectAPI::getTransform(target);

    Vector3 myPos     = TransformAPI::getGlobalPosition(myTransform);
    Vector3 targetPos = TransformAPI::getGlobalPosition(targetTransform);

    Vector3 dir = targetPos - myPos;
    dir.y = 0.0f;
    if (dir.LengthSquared() <= 0.0001f)
    {
        return;
    }
    dir.Normalize();

    m_playerRotation->applyFacingFromDirection(m_owner, dir, Time::getDeltaTime());
}

IMPLEMENT_SCRIPT(DeathBasicAttack)
