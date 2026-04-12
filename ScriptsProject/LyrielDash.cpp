#include "pch.h"
#include "LyrielDash.h"

#include "CharacterBase.h"
#include "LyrielCharacter.h"
#include "PlayerController.h"
#include "PlayerMovement.h"

#define PI 3.1415926535897931f
#define CHARGE_RECHARGE_TIME 3.0f

// ============================================================
// PROPOSAL — This script shows how a timed-movement ability
// should communicate with AbilityBase. All game logic is marked
// TODO and must be properly implemented.
// ============================================================

static const ScriptFieldInfo LyrielDashFields[] =
{
    {"Dash duration",   ScriptFieldType::Float,   offsetof(LyrielDash, m_dashDuration), {0.0f, 1.0f, 0.01f} },
    {"Dash distance",   ScriptFieldType::Float,   offsetof(LyrielDash, m_dashDistance), {0.0f, 10.0f, 0.1f} },
    {"Enable debug",    ScriptFieldType::Bool,    offsetof(LyrielDash, m_debugEnabled)                      }
};

IMPLEMENT_SCRIPT_FIELDS(LyrielDash, LyrielDashFields)

LyrielDash::LyrielDash(GameObject* owner)
    : AbilityBase(owner)
{
    m_cooldown = 0.5f;
}

void LyrielDash::Start()
{
	m_playerController = findControllerScript(m_owner);
	m_playerMovement = findMovementScript(m_owner);

    m_character = static_cast<CharacterBase*>(
        GameObjectAPI::getScript(m_owner, "LyrielCharacter"));

    if (m_character == nullptr)
    {
        Debug::warn("LyrielDash: LyrielCharacter not found on this GameObject.");
    }
}

void LyrielDash::Update()
{
    // MUST be called first — handles dead guard, force-cancel on death, cooldown tick.
    AbilityBase::Update();

    if (m_character == nullptr)
    {
        return;
    }

	const float dt = Time::getDeltaTime();

    if (m_charges < MAX_CHARGES)
    {
        m_cooldownTimer += dt;
        if (m_cooldownTimer >= CHARGE_RECHARGE_TIME)
        {
            addCharge();
            if (m_charges < MAX_CHARGES)
            {
                m_cooldownTimer -= CHARGE_RECHARGE_TIME;
            }
        }
    }

    // --- Activate dash ---
    //if (!isActive() && canActivate() && Input::isLeftShoulderJustPressed(getPlayerIndex()))
	if (Input::isLeftShoulderPressed(getPlayerIndex()))
    {
        if (!isActive() && canActivate())
        {
            onActivate();
        }
    }

    // --- Tick dash movement ---
    //if (m_isDashing)
	if (isActive())
    {
        m_dashTimer += dt;
		calculateDashMovement(dt);

        if (m_dashTimer >= m_dashDuration)
        {
            onDeactivate();
            // TODO: stop evasion VFX
        }
    }
}

void LyrielDash::drawGizmo()
{
}

void LyrielDash::recoverCharge()
{
    if (m_charges < MAX_CHARGES)
    {
        addCharge();
    }
}

bool LyrielDash::canActivate() const
{
    return m_charges > 0
        //&& !m_isDashing
		&& !isActive()
        && !m_character->isDead();
}

void LyrielDash::onActivate()
{
    m_dashTimer = 0.0f;
    //m_isDashing = true;
    setActive(true);

    --m_charges;

	Vector2 moveAxis = m_playerController->getMoveAxis();
    Vector3 moveDirection = m_playerController->readMoveDirection(moveAxis);

    if (moveDirection.x == 0.0f && moveDirection.y == 0.0f && moveDirection.z == 0.0f)
    {
        m_dashDirection = -TransformAPI::getForward(GameObjectAPI::getTransform(m_owner));
    }
    else
    {
        m_dashDirection = moveDirection;
	}
}

void LyrielDash::onDeactivate()
{
    //m_isDashing = false;
	setActive(false);
}

void LyrielDash::calculateDashMovement(const float dt)
{
    float t = m_dashTimer / m_dashDuration;

    t = (t < 0.0f) ? 0.0f : (t > 1.0f ? 1.0f : t); //clamp

    float curveSpeed = 0.5f * PI * cos(t * PI * 0.5f);

    float currentSpeed = (m_dashDistance / m_dashDuration) * curveSpeed;


    const Vector3 velocity = m_dashDirection * currentSpeed;

    if (m_playerMovement)
    {
        m_playerMovement->playerDashMovement(getOwner(), velocity * dt);
    }
}

PlayerController* LyrielDash::findControllerScript(GameObject* owner) const
{
    Script* script = GameObjectAPI::getScript(owner, "PlayerController");
    if (script == nullptr)
    {
        Debug::warn("LyrielDash: PlayerController not found on this GameObject.");
        return nullptr;
    }
	return static_cast<PlayerController*>(script);
}

PlayerMovement* LyrielDash::findMovementScript(GameObject* owner) const
{
	Script* script = GameObjectAPI::getScript(owner, "PlayerMovement");
    if (script == nullptr)
    {
        Debug::warn("LyrielDash: PlayerMovement not found on this GameObject.");
        return nullptr;
	}
	return static_cast<PlayerMovement*>(script);
}

IMPLEMENT_SCRIPT(LyrielDash)
