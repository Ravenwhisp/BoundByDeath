#include "pch.h"
#include "SoulConsumeVFX.h"

#include "ParticleLifecycle.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

IMPLEMENT_SCRIPT_FIELDS(SoulConsumeVFX,
    SERIALIZED_FLOAT(m_flightDuration, "Flight Duration", 0.05f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_initialVerticalLift, "Initial Vertical Lift", 0.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_verticalLiftRandomness, "Vertical Lift Randomness", 0.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_lateralCurveRange, "Lateral Curve Range", 0.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_terminalCurveInfluence, "Terminal Curve Influence", 0.0f, 1.0f, 0.05f),
    SERIALIZED_VEC3(m_targetOffset, "Target Offset"),
    SERIALIZED_FLOAT(m_trailFadeDelay, "Trail Fade Delay", 0.0f, 5.0f, 0.05f),
    SERIALIZED_BOOL(m_drawDebugCurve, "Draw Debug Curve")
)

namespace
{
    float randomSigned()
    {
        return (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
    }
}

SoulConsumeVFX::SoulConsumeVFX(GameObject* owner)
    : Script(owner)
{
}

void SoulConsumeVFX::Start()
{
    m_ownerTransform = GameObjectAPI::getTransform(getOwner());
    ParticleLifecycle::disableSelfDestruct(getOwner());
}

void SoulConsumeVFX::beginFlight(Transform* target)
{
    if (m_ownerTransform == nullptr)
    {
        m_ownerTransform = GameObjectAPI::getTransform(getOwner());
    }

    if (m_ownerTransform == nullptr || target == nullptr)
    {
        Debug::warn("[SoulConsumeVFX] Cannot begin flight without an owner and target transform.");
        GameObjectAPI::removeGameObject(getOwner());
        return;
    }

    m_targetTransform = target;
    m_startPosition = TransformAPI::getGlobalPosition(m_ownerTransform);
    m_previousPosition = m_startPosition;

    const Vector3 initialTarget = getTargetPosition();
    Vector3 horizontalDirection = initialTarget - m_startPosition;
    horizontalDirection.y = 0.0f;

    const float horizontalLengthSq = horizontalDirection.x * horizontalDirection.x + horizontalDirection.z * horizontalDirection.z;
    if (horizontalLengthSq > 0.0001f)
    {
        const float inverseLength = 1.0f / sqrtf(horizontalLengthSq);
        horizontalDirection.x *= inverseLength;
        horizontalDirection.z *= inverseLength;
        m_lateralDirection = Vector3(-horizontalDirection.z, 0.0f, horizontalDirection.x);
    }
    else
    {
        // A vertical/overlapping target still needs a visually varied direction.
        m_lateralDirection = randomSigned() >= 0.0f ? Vector3(1.0f, 0.0f, 0.0f) : Vector3(-1.0f, 0.0f, 0.0f);
    }

    m_lateralOffset = randomSigned() * m_lateralCurveRange;
    const float lift = m_initialVerticalLift + randomSigned() * m_verticalLiftRandomness;
    m_firstControlPoint = m_startPosition + Vector3(0.0f, (std::max)(0.0f, lift), 0.0f) + m_lateralDirection * (m_lateralOffset * 0.35f);

    m_flightTimer = 0.0f;
    m_trailFadeTimer = 0.0f;
    m_isFadingTrail = false;
    m_isFlying = true;
    setTrailGenerating(getOwner(), true);
}

void SoulConsumeVFX::Update()
{
    if (m_isFlying)
    {
        if (m_targetTransform == nullptr || m_ownerTransform == nullptr)
        {
            beginTrailFade();
            return;
        }

        m_flightTimer += Time::getDeltaTime();
        const float rawProgress = m_flightDuration > 0.0001f ? m_flightTimer / m_flightDuration : 1.0f;
        const float progress = std::clamp(rawProgress, 0.0f, 1.0f);
        const float easedProgress = MathAPI::evaluateEasing(MathAPI::EasingType::EaseInOutSine, progress);

        const Vector3 newPosition = evaluateBezier(easedProgress);
        TransformAPI::setGlobalPosition(m_ownerTransform, newPosition);
        updateFacingDirection(m_previousPosition, newPosition);
        m_previousPosition = newPosition;

        if (progress >= 1.0f)
        {
            TransformAPI::setGlobalPosition(m_ownerTransform, getTargetPosition());
            beginTrailFade();
        }
        return;
    }

    if (m_isFadingTrail)
    {
        m_trailFadeTimer += Time::getDeltaTime();
        if (m_trailFadeTimer >= m_trailFadeDelay)
        {
            GameObjectAPI::removeGameObject(getOwner());
        }
    }
}

Vector3 SoulConsumeVFX::getTargetPosition() const
{
    if (m_targetTransform == nullptr)
    {
        return m_previousPosition;
    }

    return TransformAPI::getGlobalPosition(m_targetTransform) + m_targetOffset;
}

Vector3 SoulConsumeVFX::evaluateBezier(float t) const
{
    const Vector3 endPosition = getTargetPosition();
    const Vector3 secondControlPoint = endPosition + m_lateralDirection * (m_lateralOffset * m_terminalCurveInfluence);

    const float inverseT = 1.0f - t;
    return m_startPosition * (inverseT * inverseT * inverseT)
        + m_firstControlPoint * (3.0f * inverseT * inverseT * t)
        + secondControlPoint * (3.0f * inverseT * t * t)
        + endPosition * (t * t * t);
}

void SoulConsumeVFX::updateFacingDirection(const Vector3& previousPosition, const Vector3& newPosition)
{
    const Vector3 direction = newPosition - previousPosition;
    const float directionLengthSq = direction.x * direction.x + direction.y * direction.y + direction.z * direction.z;
    if (directionLengthSq > 0.0001f)
    {
        TransformAPI::lookAt(m_ownerTransform, newPosition + direction);
    }
}

void SoulConsumeVFX::setTrailGenerating(GameObject* gameObject, bool generate) const
{
    if (gameObject == nullptr)
    {
        return;
    }

    if (TrailComponent* trail = TrailAPI::getTrailComponent(gameObject))
    {
        TrailAPI::generateTrail(trail, generate);
    }

    Transform* transform = GameObjectAPI::getTransform(gameObject);
    if (transform == nullptr)
    {
        return;
    }

    const int childCount = TransformAPI::getChildCount(transform);
    for (int i = 0; i < childCount; ++i)
    {
        Transform* child = TransformAPI::getChild(transform, i);
        if (child != nullptr)
        {
            setTrailGenerating(ComponentAPI::getOwner(child), generate);
        }
    }
}

void SoulConsumeVFX::beginTrailFade()
{
    if (!m_isFlying)
    {
        return;
    }

    m_isFlying = false;
    m_isFadingTrail = true;
    m_trailFadeTimer = 0.0f;
    setTrailGenerating(getOwner(), false);
    ParticleLifecycle::stop(getOwner());
}

void SoulConsumeVFX::drawGizmo()
{
    if (!m_drawDebugCurve || !m_isFlying)
    {
        return;
    }

    constexpr int segmentCount = 16;
    Vector3 previousPoint = m_startPosition;
    for (int i = 1; i <= segmentCount; ++i)
    {
        const float t = static_cast<float>(i) / static_cast<float>(segmentCount);
        const Vector3 currentPoint = evaluateBezier(t);
        DebugDrawAPI::drawLine(previousPoint, currentPoint, Vector3(0.45f, 0.8f, 1.0f), 0, true);
        previousPoint = currentPoint;
    }
}

IMPLEMENT_SCRIPT(SoulConsumeVFX)
