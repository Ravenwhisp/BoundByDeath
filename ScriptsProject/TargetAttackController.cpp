#include "pch.h"
#include "TargetAttackController.h"
#include "PlayerTargetController.h"
#include "Damageable.h"

static const ScriptFieldInfo targetAttackControllerFields[] =
{
    { "Attack Damage", ScriptFieldType::Float, offsetof(TargetAttackController, m_attackDamage), { 0.0f, 999.0f, 1.0f } }
};

IMPLEMENT_SCRIPT_FIELDS(TargetAttackController, targetAttackControllerFields)

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

    int playerIndex = 0;
    if (m_targetController != nullptr)
    {
        playerIndex = m_targetController->m_playerIndex;
    }

    const bool gamepadJustPressed = Input::isFaceButtonBottomJustPressed(playerIndex);

    return keyboardJustPressed || gamepadJustPressed;
}

void TargetAttackController::attackCurrentTarget()
{
    if (m_targetController == nullptr)
    {
        return;
    }

    GameObject* currentTarget = m_targetController->getCurrentTarget();
    if (currentTarget == nullptr)
    {
        Debug::log("Attack pressed, but there is no current target.");
        return;
    }

    Script* script = GameObjectAPI::getScript(currentTarget, "Damageable");
    if (script == nullptr)
    {
        Debug::warn("TargetAttackController: target %s has no Damageable script.",
            GameObjectAPI::getName(currentTarget));
        return;
    }

    Damageable* damageable = static_cast<Damageable*>(script);
    damageable->takeDamage(m_attackDamage);

    Debug::log("Attacked %s for %.2f damage.",
        GameObjectAPI::getName(currentTarget),
        m_attackDamage);
}

IMPLEMENT_SCRIPT(TargetAttackController)