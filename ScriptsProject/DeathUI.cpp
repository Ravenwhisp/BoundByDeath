#include "pch.h"
#include "DeathUI.h"

#include <cmath>

static constexpr float kChargedAttackUIReferenceRadius = 3.0f;
static constexpr float kChargedAttackUIReferenceSize = 1000.0f;

IMPLEMENT_SCRIPT_FIELDS_INHERITED(DeathUI, CharacterUI,
	FIELD_GROUP_LABEL("Taunt"),
	SERIALIZED_COMPONENT_REF(m_tauntUI, "Taunt UI", ComponentType::TRANSFORM),

	FIELD_GROUP_LABEL("Charged Attack"),
	SERIALIZED_COMPONENT_REF(m_chargedAttackUI, "Charged Attack UI", ComponentType::TRANSFORM),
	SERIALIZED_COMPONENT_REF(m_chargedAttackChargeSlider, "Charge Ring Slider", ComponentType::UISLIDER),

	FIELD_GROUP_LABEL("Slash Combo"),
	SERIALIZED_COMPONENT_REF(m_basicSlashUI, "Basic Slash UI", ComponentType::TRANSFORM),
	SERIALIZED_COMPONENT_REF(m_basicSlashSlider, "Basic Slash Slider", ComponentType::UISLIDER),

	SERIALIZED_COMPONENT_REF(m_chargedSlashUI, "Charged Slash UI", ComponentType::TRANSFORM),
	SERIALIZED_COMPONENT_REF(m_chargedSlashSlider, "Charged Slash Slider", ComponentType::UISLIDER)
)

DeathUI::DeathUI(GameObject* owner)
	: CharacterUI(owner)
{
}

void DeathUI::Start()
{
	CharacterUI::Start();

	m_tauntUITransform = m_tauntUI.getReferencedComponent();
	m_chargedAttackUITransform = m_chargedAttackUI.getReferencedComponent();
	m_chargedAttackChargeUISlider = m_chargedAttackChargeSlider.getReferencedComponent();

	m_basicSlashUITransform = m_basicSlashUI.getReferencedComponent();
	m_basicSlashUISlider = m_basicSlashSlider.getReferencedComponent();

	m_chargedSlashUITransform = m_chargedSlashUI.getReferencedComponent();
	m_chargedSlashUISlider = m_chargedSlashSlider.getReferencedComponent();

	hideTauntUI();
	hideChargedAttackUI();
	hideSlashUI();

	setupSlashUI();
}

void DeathUI::showTauntUI()
{
	if (!m_tauntUITransform)
	{
		return;
	}

	GameObject* owner = m_tauntUITransform->getOwner();

	if (!owner)
	{
		return;
	}

	GameObjectAPI::setActive(owner, true);
}

void DeathUI::updateTauntUI(const Vector3& origin, const Vector3& aimDirection)
{
	if (!m_tauntUITransform)
	{
		return;
	}

	Vector3 flatDirection = aimDirection;
	flatDirection.y = 0.0f;

	if (flatDirection.LengthSquared() <= 0.0001f)
	{
		return;
	}

	flatDirection.Normalize();

	const float yawRad = std::atan2(flatDirection.x, flatDirection.z);
	const float targetYawDeg = yawRad * (180.0f / 3.14159265f);

	TransformAPI::setGlobalPosition(m_tauntUITransform, origin);
	TransformAPI::setGlobalRotationEuler(m_tauntUITransform, Vector3(0.0f, targetYawDeg, 0.0f));
}

void DeathUI::hideTauntUI()
{
	if (!m_tauntUITransform)
	{
		return;
	}

	GameObject* owner = m_tauntUITransform->getOwner();

	if (!owner)
	{
		return;
	}

	GameObjectAPI::setActive(owner, false);
}

void DeathUI::showChargedAttackUI()
{
	if (!m_chargedAttackUITransform)
	{
		return;
	}

	GameObject* owner = m_chargedAttackUITransform->getOwner();

	if (!owner)
	{
		return;
	}

	GameObjectAPI::setActive(owner, true);
}

void DeathUI::updateChargedAttackUI(const Vector3& origin, float chargeRatio, float attackRadius)
{
	if (!m_chargedAttackUITransform)
	{
		return;
	}

	TransformAPI::setGlobalPosition(m_chargedAttackUITransform, origin);

	constexpr float referenceRadius = 3.0f;

	float uiScale = 1.0f;

	if (referenceRadius > 0.0001f)
	{
		uiScale = attackRadius / referenceRadius;
	}

	TransformAPI::setScale(m_chargedAttackUITransform, Vector3(uiScale, uiScale, uiScale));

	if (!m_chargedAttackChargeUISlider)
	{
		return;
	}

	float clampedRatio = chargeRatio;

	if (clampedRatio < 0.0f)
	{
		clampedRatio = 0.0f;
	}
	else if (clampedRatio > 1.0f)
	{
		clampedRatio = 1.0f;
	}

	SliderAPI::setFillAmount(m_chargedAttackChargeUISlider, clampedRatio);
}

void DeathUI::hideChargedAttackUI()
{
	if (m_chargedAttackChargeUISlider)
	{
		SliderAPI::setFillAmount(m_chargedAttackChargeUISlider, 0.0f);
	}

	if (!m_chargedAttackUITransform)
	{
		return;
	}

	GameObject* owner = m_chargedAttackUITransform->getOwner();

	if (!owner)
	{
		return;
	}

	GameObjectAPI::setActive(owner, false);
}

void DeathUI::setupSlashUI()
{
	if (m_basicSlashUITransform)
	{
		GameObjectAPI::setActive(m_basicSlashUITransform->getOwner(), false);
	}

	if (m_basicSlashUISlider)
	{
		SliderAPI::setFillAmount(m_basicSlashUISlider, 0.0f);
	}

	if (m_chargedSlashUITransform)
	{
		GameObjectAPI::setActive(m_chargedSlashUITransform->getOwner(), false);
	}

	if (m_chargedSlashUISlider)
	{
		SliderAPI::setFillAmount(m_chargedSlashUISlider, 0.0f);
	}
}

void DeathUI::updateBasicSlashUI(float attackStateTimer, float attackLockDuration)
{
	updateSlashUI(m_basicSlashUITransform, m_basicSlashUISlider, attackStateTimer, attackLockDuration);
}

void DeathUI::updateChargedSlashUI(float attackStateTimer, float attackLockDuration)
{
	updateSlashUI(m_chargedSlashUITransform, m_chargedSlashUISlider, attackStateTimer, attackLockDuration);
}

void DeathUI::updateSlashUI(Transform* slashTransform, UISlider* slashSlider, float attackStateTimer, float attackLockDuration)
{
	if (!slashTransform || !slashSlider)
	{
		return;
	}

	const bool showUI = attackStateTimer > 0.0f;
	GameObjectAPI::setActive(slashTransform->getOwner(), showUI);

	if (!showUI)
	{
		return;
	}

	if (attackLockDuration <= 0.0001f)
	{
		SliderAPI::setFillAmount(slashSlider, 0.0f);
		return;
	}

	const float t = 1.0f - (attackStateTimer / attackLockDuration);

	SliderAPI::setFillOrigin(slashSlider, t < 0.5f ? FillOrigin::Radial180BottomCCW : FillOrigin::Radial180Bottom);

	const float fillAmount = MathAPI::pingPong(t);
	const float easedFill = MathAPI::evaluateEasing(MathAPI::EasingType::EaseOutCubic, fillAmount);

	SliderAPI::setFillAmount(slashSlider, easedFill);
}

void DeathUI::hideSlashUI()
{
	if (m_basicSlashUITransform)
	{
		GameObjectAPI::setActive(m_basicSlashUITransform->getOwner(), false);
	}

	if (m_chargedSlashUITransform)
	{
		GameObjectAPI::setActive(m_chargedSlashUITransform->getOwner(), false);
	}
}

IMPLEMENT_SCRIPT(DeathUI)