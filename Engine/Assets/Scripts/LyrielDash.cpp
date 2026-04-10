#include "pch.h"
#include "LyrielDash.h"

#include "PlayerMovement.h"

#define PI 3.1415926535897931f
#define CHARGE_RECHARGE_TIME 3.0f

static const ScriptFieldInfo LyrielDashFields[] =
{ 
    {"Dash duration",   ScriptFieldType::Float,   offsetof(LyrielDash, m_dashDuration), {0.0f, 1.0f, 0.01f} },
	{"Dash distance",   ScriptFieldType::Float,   offsetof(LyrielDash, m_dashDistance), {0.0f, 10.0f, 0.1f} },
    {"Enable debug",    ScriptFieldType::Bool,    offsetof(LyrielDash, m_debugEnabled)                      }
};

IMPLEMENT_SCRIPT_FIELDS(LyrielDash, LyrielDashFields)

LyrielDash::LyrielDash(GameObject* owner)
    : IDash(owner)
{
}

void LyrielDash::Start()
{
    GameObject* owner = getOwner();

	m_movement = findMovementScript(owner);
}

void LyrielDash::Update()
{
    const float dt = Time::getDeltaTime();
    //Recarga del dash por cooldown
    if (m_dashCharges < MAX_CHARGES)
    {
		m_cooldownTimer += dt;
        if(m_cooldownTimer >= CHARGE_RECHARGE_TIME)
        {
            addCharge();
            if (m_dashCharges < MAX_CHARGES)
            {
                m_cooldownTimer -= CHARGE_RECHARGE_TIME;
            }
		}
    }
    //Logica de la accion del dash.
    if(!m_isDashing)
    {
        return;
	}

    m_dashTimer += dt;
    calculateDashMovement(dt);

    if (m_dashTimer >= m_dashDuration)
    {
        endDash();
    }
}

void LyrielDash::drawGizmo()
{
    if (!m_debugEnabled)
    {
        return;
    }

    const Vector3 white = { 1.0f, 1.0f, 1.0f };

    //dash distance circle
	DebugDrawAPI::drawCircle(TransformAPI::getPosition(GameObjectAPI::getTransform(getOwner())), 
        Vector3(0.0f, 1.0f, 0.0f),
        white,
        m_dashDistance);
	DebugDrawAPI::drawScreenText(("Dash Charges: " + std::to_string(m_dashCharges)).c_str(),
        Vector3(10.0f, 10.0f, 0.0f),
        white);
}

bool LyrielDash::canDash() const
{
	return !m_isDashing && m_dashCharges > 0;
}

void LyrielDash::startDash(const Vector3& direction)
{
    m_isDashing = true;
	m_dashTimer = 0.0f;

	--m_dashCharges;
    
    if (direction.x == 0.0f && direction.y == 0.0f && direction.z == 0.0f)
    {
		m_dashDirection = -TransformAPI::getForward(GameObjectAPI::getTransform(getOwner()));
    }
    else
    {
        m_dashDirection = direction;
	}
}

void LyrielDash::recoverChargeDash()
{
    if (m_dashCharges < MAX_CHARGES)
    {
        addCharge();
    }
}

void LyrielDash::endDash()
{
    m_isDashing = false;
}

void LyrielDash::calculateDashMovement(const float dt)
{
	float t = m_dashTimer / m_dashDuration;

    t = (t < 0.0f) ? 0.0f : (t > 1.0f ? 1.0f : t); //clamp

    float curveSpeed = 0.5f * PI * cos(t * PI * 0.5f);

    float currentSpeed = (m_dashDistance / m_dashDuration) * curveSpeed;


    const Vector3 velocity = m_dashDirection * currentSpeed;

    if (m_movement)
    {
		m_movement->dashMovement(getOwner(), velocity * dt);
    }
}

PlayerMovement* LyrielDash::findMovementScript(GameObject* owner)
{
    const char* ownerName = GameObjectAPI::getName(m_owner);

    Script* movementScript = owner ? GameObjectAPI::getScript(owner, "PlayerMovement") : nullptr;
    if (movementScript)
    {
        return static_cast<PlayerMovement*>(movementScript);
    }
    else
    {
        Debug::warn("LyrielDash on '%s' could not find PlayerMovement on the same GameObject.",
            ownerName);
    }

    return nullptr;
}

IMPLEMENT_SCRIPT(LyrielDash)