#pragma once

#include "ScriptAPI.h"

//NOTE :  Walking dust related stuff has been eliminated as now we use SharedEnemyParticle for the moving effects

class PaladinVFX : public Script
{
    DECLARE_SCRIPT(PaladinVFX)

public:

    explicit PaladinVFX(GameObject* owner);

    void Start() override;
    void Update() override;

    FieldList getExposedFields() const override;

    void startChargeAttackEffect();
    void stopChargeAttackEffect();

    void playBasicAttackEffect();

private:

    Vector3 getOwnerRotation() const;
    Vector3 getChargeAttackEffectPosition() const;
    Vector3 getBasicAttackEffectPosition() const;

    void addChargeAttackEffect();
    void removeChargeAttackEffect();
    void updateChargeAttackEffectPosition();

    void addBasicAttackEffect();
    void removeBasicAttackEffect();
    void updateBasicAttackEffectLifetime(float deltaTime);

public:

    PrefabRef m_chargeAttackEffectPrefab;
    PrefabRef m_basicAttackEffectPrefab;

private:

    GameObject* chargeAttackEffect = nullptr;
    bool chargeAttackEffectActive = false;

    float chargeAttackYOffset = 0.5f;
    float chargeAttackForwardOffset = 0.0f;

    GameObject* basicAttackEffect = nullptr;
    float basicAttackYOffset = 0.05f;
    float basicAttackForwardOffset = 0.75f;
    float basicAttackEffectLifetime = 1.0f;
    float basicAttackEffectTimer = 0.0f;

};