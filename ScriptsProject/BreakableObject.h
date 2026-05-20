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

    virtual void onBreak() { breakObject(); }
    bool isBroken() const { return m_isBroken; }

public:
    ScriptComponentRef<Transform> m_normalObjectTransformComponent;
    ScriptComponentRef<Transform> m_brokenObjectTransformComponent;

protected:
	Transform* m_normalObjectTransform = nullptr;
	Transform* m_brokenObjectTransform = nullptr;
    void breakObject();

private:
    bool m_isBroken = false;

};