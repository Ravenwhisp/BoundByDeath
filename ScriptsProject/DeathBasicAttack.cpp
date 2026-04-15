#include "pch.h"
#include "DeathBasicAttack.h"
#include "CharacterBase.h"
#include "DeathCharacter.h"

#include <cmath>

ScriptFieldList DeathBasicAttack::getExposedFields() const
{
    return {};  // no inspector fields — all parameters live on DeathCharacter
}

DeathBasicAttack::DeathBasicAttack(GameObject* owner)
    : AbilityBase(owner)
{
}

void DeathBasicAttack::Start()
{
    m_character = static_cast<CharacterBase*>(
        GameObjectAPI::getScript(m_owner, "DeathCharacter"));

    if (m_character == nullptr)
    {
        Debug::warn("DeathBasicAttack: DeathCharacter not found on this GameObject.");
    }
}

void DeathBasicAttack::Update()
{
    AbilityBase::Update();

    if (m_character == nullptr)
    {
        return;
    }

    const int playerIdx = getPlayerIndex();

    if (!Input::isRightShoulderJustPressed(playerIdx))
    {
        return;
    }

    if (!canActivate())
    {
        DeathCharacter* deathChar = static_cast<DeathCharacter*>(m_character);
        if (m_character->isDead())
            Debug::log("[R1] bloqueado — personaje muerto");
        else if (!m_character->canAct())
            Debug::log("[R1] bloqueado — otra ability activa");
        else
            Debug::log("[R1] bloqueado — cooldown o desactivado");
        return;
    }

    onActivate();

    DeathCharacter* deathChar = static_cast<DeathCharacter*>(m_character);
    const int   comboStep = deathChar->getComboStep();
    const float damage    = deathChar->m_basicAttackDamage;

    deathChar->dealDamageInArc(damage);
    deathChar->advanceCombo(false);

    // comboStep antes de avanzar: 0=hit1, 1=hit2, 2=hit3(reset)
    if (comboStep < 2)
        Debug::log("[COMBO] R1  step %d/3  ventana=%.1fs", comboStep + 1, deathChar->m_comboWindow);
    else
        Debug::log("[COMBO] R1  step 3/3  COMPLETO — reset");

    onDeactivate();
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

    // fill = 1.0 right after a hit, depletes to 0.0 as the combo window expires.
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

IMPLEMENT_SCRIPT(DeathBasicAttack)
