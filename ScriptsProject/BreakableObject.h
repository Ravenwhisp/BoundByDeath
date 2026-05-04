#pragma once

#include "ScriptAPI.h"

class Transform;

class BreakableObject : public Script
{
    DECLARE_SCRIPT(BreakableObject)

public:
    explicit BreakableObject(GameObject* owner);

    void Start() override;
    ScriptFieldList getExposedFields() const override;

    void breakObject();
    bool isBroken() const { return m_isBroken; }

public:
    ScriptComponentRef<Transform> m_normalObjectTransform;
    ScriptComponentRef<Transform> m_brokenObjectTransform;

private:
    bool m_isBroken = false;
};