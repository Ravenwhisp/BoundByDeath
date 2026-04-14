#include "pch.h"
#include "LyrielChargedAttack.h"
#include "CharacterBase.h"
#include "LyrielCharacter.h"

// ============================================================
// PROPOSAL — This script shows how a hold-to-aim ability
// should communicate with AbilityBase. All game logic is marked
// TODO and must be properly implemented.
// ============================================================

static const ScriptFieldInfo LyrielChargedAttackFields[] =
{
    { "Charge Time", ScriptFieldType::Float, offsetof(LyrielChargedAttack, m_chargeTime), { 0.0f, 150.f, 0.1f } },
    {"Prefab to instantiate", ScriptFieldType::String, offsetof(LyrielChargedAttack, m_projectileToInstantiate)  },
    { "Spawning relative point", ScriptFieldType::Vec3, offsetof(LyrielChargedAttack, m_spawnRelativePoint) }
};

IMPLEMENT_SCRIPT_FIELDS(LyrielChargedAttack, LyrielChargedAttackFields)

LyrielChargedAttack::LyrielChargedAttack(GameObject* owner)
    : AbilityBase(owner)
{
    m_objectTransform = GameObjectAPI::getTransform(owner);
}

void LyrielChargedAttack::Start()
{
    m_character = static_cast<CharacterBase*>(
        GameObjectAPI::getScript(m_owner, "LyrielCharacter"));

    if (m_character == nullptr)
    {
        Debug::warn("LyrielChargedAttack: LyrielCharacter not found on this GameObject.");
    }

    m_isCharging = m_toReleaseCharge = false;
    m_currentChargeTime = 0;
}

void LyrielChargedAttack::Update()
{
    // MUST be called first — handles dead guard, force-cancel on death, cooldown tick.
    AbilityBase::Update();

    if (m_character == nullptr)
    {
        return;
    }

    // --- Enter aiming mode (button pressed) ---
    // onActivate blocks all other abilities for the full aiming duration.
    if (!isActive() && canActivate() && Input::isRightTriggerJustPressed(getPlayerIndex()))
    {
        m_aimDirection = { 0.0f, 0.0f };
        onActivate();  // sets isActive = true, blocks other abilities via canAct
        // TODO: play draw-bow animation / show aiming reticle
        // TODO: optionally slow down PlayerController movement speed while aiming
        return;
    }

    // --- While aiming (button held) ---
    if (isActive())
    {
        // Update aim direction from the right stick each frame.
        m_aimDirection = Input::getLookAxis(getPlayerIndex());
        // TODO: rotate Lyriel or update a directional indicator based on m_aimDirection

        if (m_currentChargeTime != m_chargeTime)
        {

            m_currentChargeTime += Time::getDeltaTime();
            if (m_currentChargeTime > m_chargeTime) m_currentChargeTime = m_chargeTime;
        }

        // --- Fire on release ---
        if (Input::isRightTriggerReleased(getPlayerIndex()))
        {
            const float damage = static_cast<LyrielCharacter*>(m_character)->m_chargedArrowDamage;

            // Spawn charged arrow projectile in m_aimDirection (world space)
            // We calculate the new axis respect the facing position, assuming that it represents the x axis (this is so that m_spawnRelativePoint is relative to it)
            Vector3 xDirection = TransformAPI::getForward(m_objectTransform);
            xDirection.Normalize();
            Vector3 zDirection = xDirection.Cross(Vector3(0.f, 1.f, 0.f)); // assuming up = (0.f, 1.f, 0.f)
            zDirection.Normalize();
            Vector3 yDirection = zDirection.Cross(xDirection);
            yDirection.Normalize();

            Vector3 spawningPosition = TransformAPI::getPosition(m_objectTransform) +
                m_spawnRelativePoint.x * xDirection + m_spawnRelativePoint.y * yDirection + m_spawnRelativePoint.z * zDirection;


            GameObject* projectile = GameObjectAPI::instantiatePrefab(m_projectileToInstantiate.c_str(), spawningPosition, TransformAPI::getEulerDegrees(m_objectTransform));
            ProjectileController* projectileScript = static_cast<ProjectileController*>(GameObjectAPI::getScript(projectile, "ProjectileController"));
            projectileScript->m_scale = m_currentChargeTime / m_chargeTime;

            m_isCharging = m_toReleaseCharge = false;
            m_currentChargeTime = 0.f;

            // Placeholder: deal damage directly to the current target.
            dealDamageToCurrentTarget(damage);

            // TODO: play release animation / impact VFX / sound
            // TODO: restore PlayerController speed if it was reduced during aiming

            onDeactivate();  // sets isActive = false, starts cooldown, unblocks canAct
        }
    }
}

// MAYBE ADD IFs SO THAT THESE DO NOT DO ANYTHING BEYOND WHEN IT IS REQUIRED?
inline void LyrielChargedAttack::startCharge()
{
    m_isCharging = true;
}

inline void LyrielChargedAttack::releaseCharge()
{
    m_toReleaseCharge = true;
}

inline void LyrielChargedAttack::releaseImmediately()
{
    m_isCharging = true;
    m_toReleaseCharge = true;
}

IMPLEMENT_SCRIPT(LyrielChargedAttack)
