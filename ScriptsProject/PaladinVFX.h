#pragma once

#include "ScriptAPI.h"

class PaladinVFX : public Script
{
    DECLARE_SCRIPT(PaladinVFX)

public:

    explicit PaladinVFX(GameObject* owner);

    void Start() override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

    void setWalkingDustActive(bool active);
    void stopWalkingDust();

private:

    Vector3 getWalkingDustPosition() const;
    Vector3 getOwnerRotation() const;

    void addWalkingDust();
    void removeWalkingDust();
    void updateWalkingDustPosition();

public:

    float walkingDustYOffset = 0.05f;
    float walkingDustForwardOffset = -0.35f;

private:

    GameObject* walkingDustEffect = nullptr;
    bool walkingDustActive = false;

};