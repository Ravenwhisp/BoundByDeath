#include "pch.h"
#include "TargetAttackController.h"
#include "PlayerTargetController.h"
#include "Damageable.h"

static const ScriptFieldInfo TargetAttackControllerFields[] =
{
    { "Attack Damage", ScriptFieldType::Float, offsetof(TargetAttackController, m_attackDamage), { 0.0f, 999.0f, 1.0f } }
};

IMPLEMENT_SCRIPT_FIELDS(TargetAttackController, TargetAttackControllerFields)

TargetAttackController::TargetAttackController(GameObject* owner)
    : Script(owner)
{
}

void TargetAttackController::Start()
{
    m_targetController = getTargetController();
}

void TargetAttackController::Update()
{
    if (m_targetController == nullptr)
    {
        m_targetController = getTargetController();
    }

    if (m_targetController == nullptr)
    {
        return;
    }

    if (!isAttackJustPressed())
    {
        return;
    }

    attackCurrentTarget();
}

PlayerTargetController* TargetAttackController::getTargetController() const
{
    Script* script = GameObjectAPI::getScript(getOwner(), "PlayerTargetController");
    if (script == nullptr)
    {
        return nullptr;
    }

    return static_cast<PlayerTargetController*>(script);
}

bool TargetAttackController::isAttackJustPressed()
{
    const bool keyboardDown = Input::isKeyDown(KeyCode::Space);
    const bool keyboardJustPressed = keyboardDown && !m_attackWasDown;
    m_attackWasDown = keyboardDown;

    return keyboardJustPressed;
}

void TargetAttackController::attackCurrentTarget()
{
    GameObject* currentTarget = m_targetController->getCurrentTarget();
    if (currentTarget == nullptr)
    {
        return;
    }

    Script* script = GameObjectAPI::getScript(currentTarget, "Damageable");
    if (script == nullptr)
    {
        return;
    }

    Damageable* damageable = static_cast<Damageable*>(script);
    damageable->takeDamage(m_attackDamage);
}

IMPLEMENT_SCRIPT(TargetAttackController)