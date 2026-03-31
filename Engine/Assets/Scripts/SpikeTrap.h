#pragma once

#include "ScriptAPI.h"

class SpikeTrap : public Script
{
    DECLARE_SCRIPT(SpikeTrap)

public:
    explicit SpikeTrap(GameObject* owner);

    void Start() override;
    void Update() override;

    void TrapLoop();

    ScriptFieldList getExposedFields() const override;

    float m_speed = 0.0;
    float a_speed = 5.0;
    float duration = 2.0;

    GameObject* owner = getOwner();
    Transform* transform = GameObjectAPI::getTransform(owner);
    Vector3 position = TransformAPI::getPosition(transform);

    float dt = Time::getDeltaTime();

private:
    
};
