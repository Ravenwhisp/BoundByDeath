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
	)
)

namespace
{
	constexpr float SeekerSigilsUIYOffset = 0.15f;
	constexpr float SeekerSigilsImpactFadeDuration = 0.15f;
}

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
}

void AelorinUI::Update()
{
	updateSeekerSigilsUI(Time::getDeltaTime());
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
	uiPosition.y += SeekerSigilsUIYOffset;

	TransformAPI::setGlobalPosition(slot->canvas, uiPosition);
	TransformAPI::setGlobalRotationEuler(slot->canvas, Vector3(90.0f, 0.0f, 0.0f));

	Transform2DAPI::setScale(slot->container, Vector2(radius, radius));	
	Transform2DAPI::setAlpha(slot->container, 0.0f);
	Transform2DAPI::setAlpha(slot->background, 1.0f);
	Transform2DAPI::setAlpha(slot->border, 1.0f);
	Transform2DAPI::setAlpha(slot->glow, 0.0f);

	Transform2DAPI::setScale(slot->background, Vector2(0.1f, 0.1f));
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
		const float impactT = std::clamp(impactTimer / SeekerSigilsImpactFadeDuration, 0.0f, 1.0f);
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

IMPLEMENT_SCRIPT(AelorinUI)