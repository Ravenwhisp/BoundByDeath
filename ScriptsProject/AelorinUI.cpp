#include "pch.h"
#include "AelorinUI.h"

#include "Transform2D.h"

#include <algorithm>
#include <cmath>

IMPLEMENT_SCRIPT_FIELDS(AelorinUI,
	FIELD_GROUP_COLLAPSE("Seeker Sigils",
		SERIALIZED_COMPONENT_REF(m_seekerSigilsUICanvas, "Seeker Sigils UI Canvas", ComponentType::TRANSFORM),
		SERIALIZED_COMPONENT_REF(m_seekerSigilsUIContainer, "Seeker Sigils UI Container", ComponentType::TRANSFORM2D),
		SERIALIZED_COMPONENT_REF(m_seekerSigilsUIBackground, "Seeker Sigils UI Background", ComponentType::TRANSFORM2D),
		SERIALIZED_COMPONENT_REF(m_seekerSigilsUIBorder, "Seeker Sigils UI Border", ComponentType::TRANSFORM2D),
		SERIALIZED_COMPONENT_REF(m_seekerSigilsUIGlow, "Seeker Sigils UI Glow", ComponentType::TRANSFORM2D)
	),

	FIELD_GROUP_COLLAPSE("Nova",
		SERIALIZED_COMPONENT_REF(m_novaUICanvas, "Nova UI Canvas", ComponentType::TRANSFORM),
		SERIALIZED_COMPONENT_REF(m_novaUIContainer, "Nova UI Container", ComponentType::TRANSFORM2D),
		SERIALIZED_COMPONENT_REF(m_novaUIBackground, "Nova UI Background", ComponentType::TRANSFORM2D),
		SERIALIZED_COMPONENT_REF(m_novaUIBorder, "Nova UI Border", ComponentType::TRANSFORM2D),
		SERIALIZED_COMPONENT_REF(m_novaUIGlow, "Nova UI Glow", ComponentType::TRANSFORM2D)
	),

	FIELD_GROUP_COLLAPSE("Risen Spires",
		SERIALIZED_COMPONENT_REF(m_risenSpiresUICanvas, "Risen Spires UI Canvas", ComponentType::TRANSFORM),
		SERIALIZED_COMPONENT_REF(m_risenSpiresUIContainer, "Risen Spires UI Container", ComponentType::TRANSFORM2D),
		SERIALIZED_COMPONENT_REF(m_risenSpiresUIBackground, "Risen Spires UI Background", ComponentType::TRANSFORM2D),
		SERIALIZED_COMPONENT_REF(m_risenSpiresUIBorder, "Risen Spires UI Border", ComponentType::TRANSFORM2D),
		SERIALIZED_COMPONENT_REF(m_risenSpiresUIGlow, "Risen Spires UI Glow", ComponentType::TRANSFORM2D)
	)
)

AelorinUI::AelorinUI(GameObject* owner) 
	: Script(owner)
{
}

void AelorinUI::Start()
{
	// Seeker Sigils
	m_seekerSigilsUICanvasTransform = m_seekerSigilsUICanvas.getReferencedComponent();
	m_seekerSigilsUIContainerTransform2D = m_seekerSigilsUIContainer.getReferencedComponent();
	m_seekerSigilsUIBackgroundTransform2D = m_seekerSigilsUIBackground.getReferencedComponent();
	m_seekerSigilsUIBorderTransform2D = m_seekerSigilsUIBorder.getReferencedComponent();
	m_seekerSigilsUIGlowTransform2D = m_seekerSigilsUIGlow.getReferencedComponent();

	setupSeekerSigilsUI();
	hideAllSeekerSigilsUI();

	// Nova
	m_novaUICanvasTransform = m_novaUICanvas.getReferencedComponent();
	m_novaUIContainerTransform2D = m_novaUIContainer.getReferencedComponent();
	m_novaUIBackgroundTransform2D = m_novaUIBackground.getReferencedComponent();
	m_novaUIBorderTransform2D = m_novaUIBorder.getReferencedComponent();
	m_novaUIGlowTransform2D = m_novaUIGlow.getReferencedComponent();

	hideNovaUI();

	// Risen Spires
	m_risenSpiresUICanvasTransform = m_risenSpiresUICanvas.getReferencedComponent();
	m_risenSpiresUIContainerTransform2D = m_risenSpiresUIContainer.getReferencedComponent();
	m_risenSpiresUIBackgroundTransform2D = m_risenSpiresUIBackground.getReferencedComponent();
	m_risenSpiresUIBorderTransform2D = m_risenSpiresUIBorder.getReferencedComponent();
	m_risenSpiresUIGlowTransform2D = m_risenSpiresUIGlow.getReferencedComponent();

	setupRisenSpiresUI();
	hideAllRisenSpiresUI();
}

void AelorinUI::Update()
{
	const float deltaTime = Time::getDeltaTime();

	updateSeekerSigilsUI(deltaTime);
	updateNovaUI(deltaTime);
	updateRisenSpiresUI(deltaTime);
}

void AelorinUI::showSeekerSigilsUI(const Vector3& impactPosition, float radius, float telegraphDuration)
{
	SeekerSigilsUISlot* slot = acquireSeekerSigilsUISlot();

	if (!slot ||
		!slot->canvas ||
		!slot->container ||
		!slot->background ||
		!slot->border ||
		!slot->glow)
	{
		return;
	}

	GameObject* canvasObject = ComponentAPI::getOwner(slot->canvas);
	if (!canvasObject)
	{
		return;
	}

	slot->active = true;
	slot->timer = 0.0f;
	slot->duration = (std::max)(telegraphDuration, 0.001f);

	GameObjectAPI::setActive(canvasObject, true);

	Vector3 uiPosition = impactPosition;
	uiPosition.y += 0.05f;

	TransformAPI::setGlobalPosition(slot->canvas, uiPosition);
	TransformAPI::setGlobalRotationEuler(slot->canvas, Vector3(90.0f, 0.0f, 0.0f));

	Transform2DAPI::setScale(slot->container, Vector2(radius, radius));	
	Transform2DAPI::setAlpha(slot->container, 0.0f);
	Transform2DAPI::setAlpha(slot->background, 1.0f);
	Transform2DAPI::setAlpha(slot->border, 1.0f);
	Transform2DAPI::setAlpha(slot->glow, 0.0f);

	Transform2DAPI::setScale(slot->background, Vector2(0.1f, 0.1f));
}

void AelorinUI::setNovaContainerRadius(float radius)
{
	if (!m_novaUIContainerTransform2D)
	{
		return;
	}

	const float baseDiameterUI = Transform2DAPI::getBaseSize(m_novaUIContainerTransform2D).x;
	if (baseDiameterUI <= 0.001f)
	{
		return;
	}

	const float desiredDiameterUI = radius * 2.0f * 100.0f;
	const float scale = desiredDiameterUI / baseDiameterUI;

	Transform2DAPI::setScale(m_novaUIContainerTransform2D, Vector2(scale, scale));
}

void AelorinUI::showNovaUI(const Vector3& center, float firstRadius, float firstChargeDuration, bool hasSecondWave, float secondRadius, float secondChargeDuration)
{
	if (!m_novaUICanvasTransform ||
		!m_novaUIContainerTransform2D ||
		!m_novaUIBackgroundTransform2D ||
		!m_novaUIBorderTransform2D ||
		!m_novaUIGlowTransform2D)
	{
		return;
	}

	GameObject* canvasObject = ComponentAPI::getOwner(m_novaUICanvasTransform);
	if (!canvasObject)
	{
		return;
	}

	m_novaUIActive = true;
	m_novaUIHasSecondWave = hasSecondWave && secondRadius > firstRadius && secondChargeDuration > 0.0f;
	m_novaUISecondWaveStarted = false;
	m_novaUITimer = 0.0f;
	m_novaUIFirstRadius = firstRadius;
	m_novaUISecondRadius = secondRadius;
	m_novaUIFirstChargeDuration = (std::max)(firstChargeDuration, 0.001f);
	m_novaUISecondChargeDuration = (std::max)(secondChargeDuration, 0.001f);

	GameObjectAPI::setActive(canvasObject, true);

	Vector3 uiPosition = center;
	uiPosition.y += 0.05f;

	TransformAPI::setGlobalPosition(m_novaUICanvasTransform, uiPosition);
	TransformAPI::setGlobalRotationEuler(m_novaUICanvasTransform, Vector3(90.0f, 0.0f, 0.0f));

	// start with wave 1 gameplay radius
	setNovaContainerRadius(firstRadius);

	// reset visuals
	Transform2DAPI::setAlpha(m_novaUIContainerTransform2D, 1.0f);
	Transform2DAPI::setAlpha(m_novaUIBorderTransform2D, 1.0f);
	Transform2DAPI::setAlpha(m_novaUIBackgroundTransform2D, 1.0f);
	Transform2DAPI::setAlpha(m_novaUIGlowTransform2D, 0.0f);

	// wave 1 starts from the center and expands until it reaches the first damage radius
	Transform2DAPI::setScale(m_novaUIBackgroundTransform2D, Vector2(0.1f, 0.1f));

	// glow uses the full current radius
	// in phase 2 resize it at wave 1 impact
	Transform2DAPI::setScale(m_novaUIGlowTransform2D, Vector2(1.0f, 1.0f));
}

void AelorinUI::showRisenSpiresUI(Transform* patternRoot, float radius, float chargeDuration)
{
	if (!patternRoot)
	{
		return;
	}

	hideAllRisenSpiresUI();

	m_risenSpiresUIActive = true;
	m_risenSpiresUITimer = 0.0f;
	m_risenSpiresUIChargeDuration = (std::max)(chargeDuration, 0.001f);

	const int pointCount = TransformAPI::getChildCount(patternRoot);

	for (int i = 0; i < pointCount; ++i)
	{
		Transform* spirePoint = TransformAPI::getChild(patternRoot, i);
		if (!spirePoint)
		{
			continue;
		}

		RisenSpiresUISlot* slot = acquireRisenSpiresUISlot();
		if (!slot)
		{
			Debug::warn("[AelorinUI] Not enough Risen Spires UI slots for pattern");
			break;
		}

		if (!slot->canvas ||
			!slot->container ||
			!slot->background ||
			!slot->border ||
			!slot->glow)
		{
			continue;
		}

		GameObject* slotObject = ComponentAPI::getOwner(slot->canvas);
		if (!slotObject)
		{
			continue;
		}

		Vector3 uiPosition = TransformAPI::getGlobalPosition(spirePoint);
		uiPosition.y += 0.05f;
		
		GameObjectAPI::setActive(slotObject, true);
		TransformAPI::setGlobalPosition(slot->canvas, uiPosition);
		TransformAPI::setGlobalRotationEuler(slot->canvas, Vector3(90.0f, 0.0f, 0.0f));

		setRisenSpiresSlotRadius(*slot, radius);

		// reset visuals
		Transform2DAPI::setAlpha(slot->container, 1.0f);
		Transform2DAPI::setAlpha(slot->background, 1.0f);
		Transform2DAPI::setAlpha(slot->border, 1.0f);
		Transform2DAPI::setAlpha(slot->glow, 0.0f);

		// telegraph starts from the center
		Transform2DAPI::setScale(slot->background, Vector2(0.1f, 0.1f));
		Transform2DAPI::setScale(slot->glow, Vector2(1.0f, 1.0f));

		slot->active = true;
	}
}

void AelorinUI::setupSeekerSigilsUI()
{
	m_seekerSigilsUISlots.clear();

	// first UI is assigned in the editor and acts as a template for the remaining slots
	if (!m_seekerSigilsUICanvasTransform ||
		!m_seekerSigilsUIContainerTransform2D ||
		!m_seekerSigilsUIBackgroundTransform2D ||
		!m_seekerSigilsUIBorderTransform2D ||
		!m_seekerSigilsUIGlowTransform2D)
	{
		Debug::warn("[AelorinUI] Seeker Sigils template UI is incomplete");
		return;
	}

	// add the first slot
	SeekerSigilsUISlot firstSlot;
	firstSlot.canvas = m_seekerSigilsUICanvasTransform;
	firstSlot.container = m_seekerSigilsUIContainerTransform2D;
	firstSlot.background = m_seekerSigilsUIBackgroundTransform2D;
	firstSlot.border = m_seekerSigilsUIBorderTransform2D;
	firstSlot.glow = m_seekerSigilsUIGlowTransform2D;

	m_seekerSigilsUISlots.push_back(firstSlot);

	// the rest are siblings to the first canvas
	Transform* uiRoot = TransformAPI::getParent(m_seekerSigilsUICanvasTransform);
	if (!uiRoot)
	{
		Debug::warn("[AelorinUI] Seeker Sigils UI root not found");
		return;
	}

	const int childCount = TransformAPI::getChildCount(uiRoot);

	for (int i = 0; i < childCount; ++i)
	{
		Transform* canvas = TransformAPI::getChild(uiRoot, i);
		if (!canvas)
		{
			continue;
		}

		// first one was already added
		if (canvas == m_seekerSigilsUICanvasTransform)
		{
			continue;
		}

		Transform* containerTransform = TransformAPI::findChildByName(canvas, "Container");
		if (!containerTransform)
		{
			continue;
		}

		Transform* backgroundTransform = TransformAPI::findChildByName(containerTransform, "Background");
		Transform* borderTransform = TransformAPI::findChildByName(containerTransform, "Border");
		Transform* glowTransform = TransformAPI::findChildByName(containerTransform, "Glow");

		if (!backgroundTransform || !borderTransform || !glowTransform)
		{
			continue;
		}

		GameObject* containerObject = ComponentAPI::getOwner(containerTransform);
		GameObject* backgroundObject = ComponentAPI::getOwner(backgroundTransform);
		GameObject* borderObject = ComponentAPI::getOwner(borderTransform);
		GameObject* glowObject = ComponentAPI::getOwner(glowTransform);

		if (!containerObject || !backgroundObject || !borderObject || !glowObject)
		{
			continue;
		}

		Transform2D* container = static_cast<Transform2D*>(GameObjectAPI::getComponent(containerObject, ComponentType::TRANSFORM2D));
		Transform2D* background = static_cast<Transform2D*>(GameObjectAPI::getComponent(backgroundObject, ComponentType::TRANSFORM2D));
		Transform2D* border = static_cast<Transform2D*>(GameObjectAPI::getComponent(borderObject, ComponentType::TRANSFORM2D));
		Transform2D* glow =	static_cast<Transform2D*>(GameObjectAPI::getComponent(glowObject, ComponentType::TRANSFORM2D));

		if (!container || !background || !border || !glow)
		{
			continue;
		}

		SeekerSigilsUISlot slot;
		slot.canvas = canvas;
		slot.container = container;
		slot.background = background;
		slot.border = border;
		slot.glow = glow;

		m_seekerSigilsUISlots.push_back(slot);
	}

	Debug::log("[AelorinUI] Cached %d Seeker Sigils UI slots", static_cast<int>(m_seekerSigilsUISlots.size()));
}

void AelorinUI::hideAllSeekerSigilsUI()
{
	for (SeekerSigilsUISlot& slot : m_seekerSigilsUISlots)
	{
		hideSeekerSigilsUISlot(slot);
	}
}

void AelorinUI::updateSeekerSigilsUI(float deltaTime)
{
	for (SeekerSigilsUISlot& slot : m_seekerSigilsUISlots)
	{
		if (!slot.active)
		{
			continue;
		}

		if (!slot.container ||
			!slot.background ||
			!slot.border ||
			!slot.glow)
		{
			hideSeekerSigilsUISlot(slot);
			continue;
		}

		slot.timer += deltaTime;

		if (slot.timer < slot.duration)
		{
			const float t = std::clamp(slot.timer / slot.duration, 0.0f, 1.0f);
			const float containerAlpha = MathAPI::evaluateEasing(MathAPI::EasingType::EaseOutQuad, t);
			Transform2DAPI::setAlpha(slot.container, containerAlpha);

			const float backgroundScale = 0.1f + (t * 0.9f);
			Transform2DAPI::setScale(slot.background, Vector2(backgroundScale, backgroundScale));

			continue;
		}

		const float impactTimer = slot.timer - slot.duration;
		const float impactT = std::clamp(impactTimer / m_seekerSigilsImpactFadeDuration, 0.0f, 1.0f);
		Transform2DAPI::setScale(slot.background, Vector2(1.0f, 1.0f));

		const float fadeAlpha = 1.0f - MathAPI::evaluateEasing(MathAPI::EasingType::EaseOutQuad, impactT);
		Transform2DAPI::setAlpha(slot.container, fadeAlpha);

		Transform2DAPI::setAlpha(slot.glow, fadeAlpha);

		if (impactT >= 1.0f)
		{
			hideSeekerSigilsUISlot(slot);
		}
	}
}

void AelorinUI::hideSeekerSigilsUISlot(SeekerSigilsUISlot& slot)
{
	if (slot.canvas)
	{
		GameObject* canvasObject = ComponentAPI::getOwner(slot.canvas);
		if (canvasObject)
		{
			GameObjectAPI::setActive(canvasObject, false);
		}
	}

	slot.active = false;
	slot.timer = 0.0f;
	slot.duration = 0.0f;
}

AelorinUI::SeekerSigilsUISlot* AelorinUI::acquireSeekerSigilsUISlot()
{
	for (SeekerSigilsUISlot& slot : m_seekerSigilsUISlots)
	{
		if (!slot.active)
		{
			return &slot;
		}
	}

	return nullptr;
}

void AelorinUI::hideNovaUI()
{
	if (m_novaUICanvasTransform)
	{
		GameObject* canvasObject = ComponentAPI::getOwner(m_novaUICanvasTransform);
		if (canvasObject)
		{
			GameObjectAPI::setActive(canvasObject, false);
		}
	}

	m_novaUIActive = false;
	m_novaUIHasSecondWave = false;
	m_novaUISecondWaveStarted = false;
	m_novaUITimer = 0.0f;
	m_novaUIFirstChargeDuration = 0.0f;
	m_novaUISecondChargeDuration = 0.0f;
	m_novaUIFirstRadius = 0.0f;
	m_novaUISecondRadius = 0.0f;
}

void AelorinUI::updateNovaUI(float deltaTime)
{
	if (!m_novaUIActive)
	{
		return;
	}

	if (!m_novaUIContainerTransform2D ||
		!m_novaUIBackgroundTransform2D ||
		!m_novaUIGlowTransform2D)
	{
		hideNovaUI();
		return;
	}

	m_novaUITimer += deltaTime;

	// wave 1 charge
	if (!m_novaUISecondWaveStarted && m_novaUITimer < m_novaUIFirstChargeDuration)
	{
		const float t = std::clamp(m_novaUITimer / m_novaUIFirstChargeDuration, 0.0f, 1.0f);
		const float easedT = MathAPI::evaluateEasing(MathAPI::EasingType::EaseInQuad, t);
		const float fillScale = 0.1f + 0.9f * easedT;

		Transform2DAPI::setScale(m_novaUIBackgroundTransform2D, Vector2(fillScale, fillScale));

		return;
	}

	// phase 1 - wave 1 is the final impact
	if (!m_novaUIHasSecondWave)
	{
		const float impactTimer = m_novaUITimer - m_novaUIFirstChargeDuration;
		const float impactT = std::clamp(impactTimer / m_novaUIImpactFadeDuration, 0.0f, 1.0f);

		Transform2DAPI::setScale(m_novaUIBackgroundTransform2D, Vector2(1.0f, 1.0f));
		Transform2DAPI::setAlpha(m_novaUIGlowTransform2D, 1.0f - impactT);
		Transform2DAPI::setAlpha(m_novaUIContainerTransform2D, 1.0f - impactT);

		if (impactT >= 1.0f)
		{
			hideNovaUI();
		}

		return;
	}
	
	// phase 2 - wave 1 impact -> continue outward immediately
	if (!m_novaUISecondWaveStarted)
	{
		m_novaUISecondWaveStarted = true;

		// background visually at wave 1 radius after container becomes wave 2 size
		const float firstToSecondRatio = m_novaUISecondRadius > 0.001f ? m_novaUIFirstRadius / m_novaUISecondRadius : 1.0f;

		// border instantly becomes wave 2 radius
		setNovaContainerRadius(m_novaUISecondRadius);

		// continue charge from wave 1 radius
		Transform2DAPI::setScale(m_novaUIBackgroundTransform2D, Vector2(firstToSecondRatio, firstToSecondRatio));
		Transform2DAPI::setScale(m_novaUIGlowTransform2D, Vector2(firstToSecondRatio, firstToSecondRatio));
		Transform2DAPI::setAlpha(m_novaUIGlowTransform2D, 1.0f);

		return;
	}

	// wave 2 charge
	const float secondWaveTimer = m_novaUITimer - m_novaUIFirstChargeDuration;
	if (secondWaveTimer < m_novaUISecondChargeDuration)
	{
		const float t = std::clamp(secondWaveTimer / m_novaUISecondChargeDuration, 0.0f, 1.0f);
		const float easedT = MathAPI::evaluateEasing(MathAPI::EasingType::EaseInQuad, t);
		const float startScale = m_novaUISecondRadius > 0.001f ? m_novaUIFirstRadius / m_novaUISecondRadius : 1.0f;
		const float fillScale = startScale + (1.0f - startScale) * easedT;

		Transform2DAPI::setScale(m_novaUIBackgroundTransform2D, Vector2(fillScale, fillScale));

		// fade wave 1 flash quickly
		const float glowT = std::clamp(secondWaveTimer / m_seekerSigilsImpactFadeDuration, 0.0f, 1.0f);
		
		Transform2DAPI::setAlpha(m_novaUIGlowTransform2D, 1.0f - glowT);

		return;
	}

	// wave 2 impact
	Transform2DAPI::setScale(m_novaUIBackgroundTransform2D, Vector2(1.0f, 1.0f));
	Transform2DAPI::setScale(m_novaUIGlowTransform2D, Vector2(1.0f, 1.0f));

	const float finalImpactTimer = secondWaveTimer - m_novaUISecondChargeDuration;
	const float finalImpactT = std::clamp(finalImpactTimer / m_novaUIImpactFadeDuration, 0.0f, 1.0f);

	Transform2DAPI::setAlpha(m_novaUIGlowTransform2D, 1.0f - finalImpactT);
	Transform2DAPI::setAlpha(m_novaUIContainerTransform2D, 1.0f - finalImpactT);

	if (finalImpactT >= 1.0f)
	{
		hideNovaUI();
	}
}

void AelorinUI::hideAllRisenSpiresUI()
{
	for (RisenSpiresUISlot& slot : m_risenSpiresUISlots)
	{
		hideRisenSpiresUISlot(slot);
	}

	m_risenSpiresUIActive = false;
	m_risenSpiresUITimer = 0.0f;
	m_risenSpiresUIChargeDuration = 0.0f;
}

void AelorinUI::setupRisenSpiresUI()
{
	m_risenSpiresUISlots.clear();

	// first UI is assigned in the editor and acts as a template for the remaining slots
	if (!m_risenSpiresUICanvasTransform ||
		!m_risenSpiresUIContainerTransform2D ||
		!m_risenSpiresUIBackgroundTransform2D ||
		!m_risenSpiresUIBorderTransform2D ||
		!m_risenSpiresUIGlowTransform2D)
	{
		Debug::warn("[AelorinUI] Risen Spires template UI is incomplete");
		return;
	}

	// add the first slot
	RisenSpiresUISlot firstSlot;
	firstSlot.canvas = m_risenSpiresUICanvasTransform;
	firstSlot.container = m_risenSpiresUIContainerTransform2D;
	firstSlot.background = m_risenSpiresUIBackgroundTransform2D;
	firstSlot.border = m_risenSpiresUIBorderTransform2D;
	firstSlot.glow = m_risenSpiresUIGlowTransform2D;
	firstSlot.active = false;

	m_risenSpiresUISlots.push_back(firstSlot);

	// the rest are siblings to the first canvas
	Transform* uiRoot = TransformAPI::getParent(m_risenSpiresUICanvasTransform);
	if (!uiRoot)
	{
		Debug::warn("[AelorinUI] Risen Spires UI root not found");
		return;
	}

	const int childCount = TransformAPI::getChildCount(uiRoot);

	for (int i = 0; i < childCount; ++i)
	{
		Transform* canvas = TransformAPI::getChild(uiRoot, i);
		if (!canvas)
		{
			continue;
		}

		// first one was already added
		if (canvas == m_risenSpiresUICanvasTransform)
		{
			continue;
		}

		Transform* containerTransform = TransformAPI::findChildByName(canvas, "Container");
		if (!containerTransform)
		{
			continue;
		}

		Transform* backgroundTransform = TransformAPI::findChildByName(containerTransform, "Background");
		Transform* borderTransform = TransformAPI::findChildByName(containerTransform, "Border");
		Transform* glowTransform = TransformAPI::findChildByName(containerTransform, "Glow");

		if (!backgroundTransform || !borderTransform || !glowTransform)
		{
			continue;
		}

		GameObject* containerObject = ComponentAPI::getOwner(containerTransform);
		GameObject* backgroundObject = ComponentAPI::getOwner(backgroundTransform);
		GameObject* borderObject = ComponentAPI::getOwner(borderTransform);
		GameObject* glowObject = ComponentAPI::getOwner(glowTransform);

		if (!containerObject || !backgroundObject || !borderObject || !glowObject)
		{
			continue;
		}

		Transform2D* container = static_cast<Transform2D*>(GameObjectAPI::getComponent(containerObject, ComponentType::TRANSFORM2D));
		Transform2D* background = static_cast<Transform2D*>(GameObjectAPI::getComponent(backgroundObject, ComponentType::TRANSFORM2D));
		Transform2D* border = static_cast<Transform2D*>(GameObjectAPI::getComponent(borderObject, ComponentType::TRANSFORM2D));
		Transform2D* glow = static_cast<Transform2D*>(GameObjectAPI::getComponent(glowObject, ComponentType::TRANSFORM2D));

		if (!container || !background || !border || !glow)
		{
			continue;
		}

		RisenSpiresUISlot slot;
		slot.canvas = canvas;
		slot.container = container;
		slot.background = background;
		slot.border = border;
		slot.glow = glow;
		slot.active = false;

		m_risenSpiresUISlots.push_back(slot);
	}

	Debug::log("[AelorinUI] Cached %d Risen Spires UI slots", static_cast<int>(m_risenSpiresUISlots.size()));
}

void AelorinUI::setRisenSpiresSlotRadius(RisenSpiresUISlot& slot, float radius)
{
	if (!slot.container)
	{
		return;
	}

	const float baseDiameterUI = Transform2DAPI::getBaseSize(slot.container).x;
	if (baseDiameterUI <= 0.001f)
	{
		return;
	}

	const float desiredDiameterUI = radius * 2.0f * 100.0f;
	const float radiusScale = desiredDiameterUI / baseDiameterUI;

	Transform2DAPI::setScale(slot.container, Vector2(radiusScale, radiusScale));
}

void AelorinUI::updateRisenSpiresUI(float deltaTime)
{
	if (!m_risenSpiresUIActive)
	{
		return;
	}

	m_risenSpiresUITimer += deltaTime;

	// charge
	if (m_risenSpiresUITimer < m_risenSpiresUIChargeDuration)
	{
		const float t = std::clamp(m_risenSpiresUITimer / m_risenSpiresUIChargeDuration, 0.0f, 1.0f);
		const float easedT = MathAPI::evaluateEasing(MathAPI::EasingType::EaseInQuad, t);
		const float chargeScale = 0.1f + 0.9 * easedT;

		for (RisenSpiresUISlot& slot : m_risenSpiresUISlots)
		{
			if (!slot.active || !slot.background)
			{
				continue;
			}

			Transform2DAPI::setScale(slot.background, Vector2(chargeScale, chargeScale));
		}

		return;
	}

	// impact
	const float impactTimer = m_risenSpiresUITimer - m_risenSpiresUIChargeDuration;
	const float impactT = std::clamp(impactTimer / m_risenSpiresUIImpactFadeDuration, 0.0f, 1.0f);
	const float fadeAlpha = 1.0f - impactT;

	for (RisenSpiresUISlot& slot : m_risenSpiresUISlots)
	{
		if (!slot.active)
		{
			continue;
		}

		if (slot.background)
		{
			Transform2DAPI::setScale(slot.background, Vector2(1.0f, 1.0f));
		}

		if (slot.glow)
		{
			Transform2DAPI::setAlpha(slot.glow, fadeAlpha);
		}

		if (slot.container)
		{
			Transform2DAPI::setAlpha(slot.container, fadeAlpha);
		}
	}

	if (impactT >= 1.0f)
	{
		hideAllRisenSpiresUI();
	}
}

void AelorinUI::hideRisenSpiresUISlot(RisenSpiresUISlot& slot)
{
	if (slot.canvas)
	{
		GameObject* canvasObject = ComponentAPI::getOwner(slot.canvas);
		if (canvasObject)
		{
			GameObjectAPI::setActive(canvasObject, false);
		}
	}

	slot.active = false;
}

AelorinUI::RisenSpiresUISlot* AelorinUI::acquireRisenSpiresUISlot()
{
	for (RisenSpiresUISlot& slot : m_risenSpiresUISlots)
	{
		if (!slot.active)
		{
			return &slot;
		}
	}

	return nullptr;
}

IMPLEMENT_SCRIPT(AelorinUI)