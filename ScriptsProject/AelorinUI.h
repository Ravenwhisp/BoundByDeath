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

	// Nova
	void showNovaUI(const Vector3& center, float firstRadius, float firstChargeDuration, bool hasSecondWave, float secondRadius = 0.0f, float secondChargeDuration = 0.0f);

	// Risen Spires
	void showRisenSpiresUI(Transform* patternRoot, float radius, float chargeDuration);

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

	// Nova
	void hideNovaUI();
	void setNovaContainerRadius(float radius);
	void updateNovaUI(float deltaTime);

	// Risen Spires
	struct RisenSpiresUISlot
	{
		Transform* canvas = nullptr;
		Transform2D* container = nullptr;
		Transform2D* background = nullptr;
		Transform2D* border = nullptr;
		Transform2D* glow = nullptr;

		bool active = false;
	};

	void hideAllRisenSpiresUI();
	void setupRisenSpiresUI();
	void setRisenSpiresSlotRadius(RisenSpiresUISlot& slot, float radius);
	void updateRisenSpiresUI(float deltaTime);
	void hideRisenSpiresUISlot(RisenSpiresUISlot& slot);
	RisenSpiresUISlot* acquireRisenSpiresUISlot();

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

	static constexpr float m_seekerSigilsImpactFadeDuration = 0.15f;


	// Nova
	ComponentRef<Transform> m_novaUICanvas;
	ComponentRef<Transform2D> m_novaUIContainer;
	ComponentRef<Transform2D> m_novaUIBackground;
	ComponentRef<Transform2D> m_novaUIBorder;
	ComponentRef<Transform2D> m_novaUIGlow;

	Transform* m_novaUICanvasTransform = nullptr;
	Transform2D* m_novaUIContainerTransform2D = nullptr;
	Transform2D* m_novaUIBackgroundTransform2D = nullptr;
	Transform2D* m_novaUIBorderTransform2D = nullptr;
	Transform2D* m_novaUIGlowTransform2D = nullptr;

	bool m_novaUIActive = false;
	bool m_novaUIHasSecondWave = false;
	bool m_novaUISecondWaveStarted = false;

	float m_novaUITimer = 0.0f;

	float m_novaUIFirstChargeDuration = 0.0f;
	float m_novaUISecondChargeDuration = 0.0f;

	float m_novaUIFirstRadius = 0.0f;
	float m_novaUISecondRadius = 0.0f;

	static constexpr float m_novaUIImpactFadeDuration = 0.15f;

	// Risen Spires UI
	ComponentRef<Transform> m_risenSpiresUICanvas;
	ComponentRef<Transform2D> m_risenSpiresUIContainer;
	ComponentRef<Transform2D> m_risenSpiresUIBackground;
	ComponentRef<Transform2D> m_risenSpiresUIBorder;
	ComponentRef<Transform2D> m_risenSpiresUIGlow;

	Transform* m_risenSpiresUICanvasTransform = nullptr;
	Transform2D* m_risenSpiresUIContainerTransform2D = nullptr;
	Transform2D* m_risenSpiresUIBackgroundTransform2D = nullptr;
	Transform2D* m_risenSpiresUIBorderTransform2D = nullptr;
	Transform2D* m_risenSpiresUIGlowTransform2D = nullptr;

	std::vector<RisenSpiresUISlot> m_risenSpiresUISlots;

	bool m_risenSpiresUIActive = false;
	float m_risenSpiresUITimer = 0.0f;
	float m_risenSpiresUIChargeDuration = 0.0f;
	int m_risenSpiresUIActiveSlotCount = 0;

	static constexpr float m_risenSpiresUIImpactFadeDuration = 0.15f;

};