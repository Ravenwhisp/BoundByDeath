#pragma once

#include "ScriptAPI.h"

#include <vector>

class Transform2D;

class AelorinUI : public Script
{
	DECLARE_SCRIPT(AelorinUI)

public:
	explicit AelorinUI(GameObject* owner);

	void Start() override;
	void Update() override;

	FieldList getExposedFields() const override;

	// Seeker Sigils
	void showSeekerSigilsUI(const Vector3& impactPosition, float radius, float telegraphDuration);

private:

	// Seeker Sigils
	struct SeekerSigilsUISlot
	{
		Transform* canvas = nullptr;
		Transform2D* container = nullptr;
		Transform2D* background = nullptr;
		Transform2D* border = nullptr;
		Transform2D* glow = nullptr;

		bool active = false;

		float timer = 0.0f;
		float duration = 0.0f;
	};

	void setupSeekerSigilsUI();
	void hideAllSeekerSigilsUI();
	void updateSeekerSigilsUI(float deltaTime);
	void hideSeekerSigilsUISlot(SeekerSigilsUISlot& slot);
	SeekerSigilsUISlot* acquireSeekerSigilsUISlot();


private:

	// Seeker Sigils
	ComponentRef<Transform> m_seekerSigilsUICanvas;
	ComponentRef<Transform2D> m_seekerSigilsUIContainer;
	ComponentRef<Transform2D> m_seekerSigilsUIBackground;
	ComponentRef<Transform2D> m_seekerSigilsUIBorder;
	ComponentRef<Transform2D> m_seekerSigilsUIGlow;

	Transform* m_seekerSigilsUICanvasTransform = nullptr;
	Transform2D* m_seekerSigilsUIContainerTransform2D = nullptr;
	Transform2D* m_seekerSigilsUIBackgroundTransform2D = nullptr;
	Transform2D* m_seekerSigilsUIBorderTransform2D = nullptr;
	Transform2D* m_seekerSigilsUIGlowTransform2D = nullptr;

	std::vector<SeekerSigilsUISlot> m_seekerSigilsUISlots;
};