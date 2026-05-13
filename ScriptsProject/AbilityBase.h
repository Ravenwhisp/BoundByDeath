#pragma once

#include "ScriptAPI.h"
#include "UISlider.h"

class CharacterBase;

class AbilityBase : public Script
{
public:
    explicit AbilityBase(GameObject* owner);

    void Start() override;
    void Update() override;
    ScriptFieldList getExposedFields() const override;

    bool isEnabled() const { return m_isEnabled; }
    void setEnabled(bool enabled) { m_isEnabled = enabled; }

protected:
    virtual bool canStartAbility() const;

    void updateCooldown();
	void startCooldown();
    bool isCooldownReady() const { return m_cooldownTimer <= 0.0f; }

    void setAbilityLocked(bool locked);
    int getPlayerIndex() const;

    CharacterBase* findCharacterScript(GameObject* owner) const;

    Vector3 computeCameraRelativeAimDirection(float deadzoneSq = 0.0001f) const;
	Vector3 getFallbackFacingDirection() const;

protected:
    CharacterBase* m_character = nullptr;

    float m_cooldown = 0.0f;
    float m_cooldownTimer = 0.0f;
    ScriptComponentRef<Transform> m_cdUI;
    ScriptComponentRef<UISlider> m_cdBar;

    bool m_isEnabled = true;
};