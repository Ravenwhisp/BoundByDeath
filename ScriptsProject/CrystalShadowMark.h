#pragma once
#include "ScriptAPI.h"
#include "EnemyShadowMark.h"

class CrystalVisuals;

class CrystalShadowMark : public EnemyShadowMark
{
    DECLARE_SCRIPT(CrystalShadowMark)
public:
    explicit CrystalShadowMark(GameObject* owner);

    void Start()  override;
    void Update() override;

    FieldList getExposedFields() const override;

    bool processAttack(PlayerAttackType attackType) override;
    bool isActivated() const { return m_activated; }
    bool isPuzzleCompleted() const { return m_puzzleCompleted; }

    void completeCrystal();

private:
	void activeEffect();
	void deactivateEffect();

    void activateCrystal();

public:
    float m_activeTime = 5.0f;
    PrefabRef m_crystalSparks;
    PrefabRef m_crystalStars;

private:
    CrystalVisuals* m_visualsController = nullptr;

    bool m_activated = false;
    bool m_puzzleCompleted = false;
    bool m_activatedLoopStarted = false;

    float m_activationTimer = 0.0f;

    GameObject* effectObject = nullptr;
    GameObject* effectObject2 = nullptr;
};

