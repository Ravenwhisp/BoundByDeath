#include "pch.h"
#include "TargetIndicatorUI.h"
#include "PlayerTargetController.h"

IMPLEMENT_SCRIPT_FIELDS(TargetIndicatorUI,
    SERIALIZED_COMPONENT_REF(m_playerTransform, "Player Transform", ComponentType::TRANSFORM),
    SERIALIZED_COMPONENT_REF(m_indicatorVisualTransform, "Indicator Visual Transform", ComponentType::TRANSFORM),
    SERIALIZED_VEC3(m_positionOffset, "Position Offset"),
    SERIALIZED_FLOAT(m_followSharpness, "Follow Sharpness", 0.0f, 50.0f, 0.1f),
    FIELD_GROUP_LABEL("Animation"),
    SERIALIZED_FLOAT(m_switchPopScale, "Switch Pop Scale", 1.0f, 3.0f, 0.05f),
    SERIALIZED_FLOAT(m_switchPopDuration, "Switch Pop Duration", 0.01f, 1.0f, 0.01f)
)

TargetIndicatorUI::TargetIndicatorUI(GameObject* owner)
    : Script(owner)
{
}

void TargetIndicatorUI::Start()
{
    m_playerTargetController = getPlayerTargetController();

    Transform* visualTransform = m_indicatorVisualTransform.getReferencedComponent();
    if (visualTransform != nullptr)
    {
        m_baseScale = TransformAPI::getScale(visualTransform);
    }

    hideIndicator();
}

void TargetIndicatorUI::Update()
{
    if (m_playerTargetController == nullptr)
    {
        m_playerTargetController = getPlayerTargetController();
    }

    if (m_playerTargetController == nullptr)
    {
        hideIndicator();
        return;
    }

    Transform* visualTransform = m_indicatorVisualTransform.getReferencedComponent();
    if (visualTransform == nullptr)
    {
        hideIndicator();
        return;
    }

    GameObject* currentTarget = m_playerTargetController->getCurrentTarget();
    if (currentTarget == nullptr)
    {
        hideIndicator();
        return;
    }

    Transform* targetTransform = GameObjectAPI::getTransform(currentTarget);

    showIndicator();

    const Vector3 targetPosition = TransformAPI::getGlobalPosition(targetTransform);
    const Vector3 desiredPosition = targetPosition + m_positionOffset;

    const bool targetChanged = (currentTarget != m_previousTarget);

    if (targetChanged)
    {
        TransformAPI::setPosition(visualTransform, desiredPosition);
        startSwitchAnimation();
        m_previousTarget = currentTarget;
    }
    else if (m_followSharpness <= 0.0f)
    {
        TransformAPI::setPosition(visualTransform, desiredPosition);
    }
    else
    {
        const Vector3 currentPosition = TransformAPI::getPosition(visualTransform);
        const float dt = Time::getDeltaTime();
        const float followFraction = 1.0f - expf(-m_followSharpness * dt);
        const Vector3 smoothedPosition = currentPosition + (desiredPosition - currentPosition) * followFraction;

        TransformAPI::setPosition(visualTransform, smoothedPosition);
    }

    updateSwitchAnimation(visualTransform);
}

PlayerTargetController* TargetIndicatorUI::getPlayerTargetController() const
{
    Transform* playerTransform = m_playerTransform.getReferencedComponent();
    if (playerTransform == nullptr)
    {
        return nullptr;
    }

    GameObject* player = ComponentAPI::getOwner(playerTransform);

    return GameObjectAPI::findScript<PlayerTargetController>(player);
}

void TargetIndicatorUI::hideIndicator()
{
    Transform* visualTransform = m_indicatorVisualTransform.getReferencedComponent();
    if (visualTransform == nullptr)
    {
        return;
    }

    m_switchAnimationTimer = 0.0f;
    TransformAPI::setScale(visualTransform, m_baseScale);

    GameObject* visualObject = ComponentAPI::getOwner(visualTransform);
    GameObjectAPI::setActive(visualObject, false);
}

void TargetIndicatorUI::showIndicator()
{
    Transform* visualTransform = m_indicatorVisualTransform.getReferencedComponent();
    if (visualTransform == nullptr)
    {
        return;
    }

    GameObject* visualObject = ComponentAPI::getOwner(visualTransform);
    GameObjectAPI::setActive(visualObject, true);
}

void TargetIndicatorUI::startSwitchAnimation()
{
    m_switchAnimationTimer = m_switchPopDuration;
}

void TargetIndicatorUI::updateSwitchAnimation(Transform* visualTransform)
{
    if (visualTransform == nullptr)
    {
        return;
    }

    if (m_switchAnimationTimer <= 0.0f || m_switchPopDuration <= 0.0001f)
    {
        TransformAPI::setScale(visualTransform, m_baseScale);
        return;
    }

    m_switchAnimationTimer -= Time::getDeltaTime();

    if (m_switchAnimationTimer < 0.0f)
    {
        m_switchAnimationTimer = 0.0f;
    }

    const float normalizedTime = 1.0f - (m_switchAnimationTimer / m_switchPopDuration);

    const float popAmount = sinf(normalizedTime * 3.14159265f);
    const float scaleMultiplier = 1.0f + ((m_switchPopScale - 1.0f) * popAmount);

    TransformAPI::setScale(visualTransform, m_baseScale * scaleMultiplier);
}

IMPLEMENT_SCRIPT(TargetIndicatorUI)