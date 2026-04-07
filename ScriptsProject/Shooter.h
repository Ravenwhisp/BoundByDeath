#pragma once

#include "ScriptAPI.h"

class Shooter : public Script
{
    DECLARE_SCRIPT(Shooter)

public:
    explicit Shooter(GameObject* owner);

    void Start() override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

    // To control behaviour INDEPENDENTLY of the input (and maybe use with AI)
    void startCharge();
    void releaseCharge();
    void releaseImmediately(); // to allow just a basic shot

    bool isCharging() const { return m_isCharging; }

    float m_chargeTime = 2; // in seconds!

    ScriptComponentRef<Transform> m_targetTransform; // the object to hit

    std::string m_projectileToInstantiate = "";
    Vector3 m_spawnRelativePoint = Vector3(0.f, 0.f, 0.f); // respect to target direction


private:

    Transform* m_objectTransform; // the current object

    bool m_isCharging;
    float m_currentChargeTime;

    bool m_toReleaseCharge;
};

