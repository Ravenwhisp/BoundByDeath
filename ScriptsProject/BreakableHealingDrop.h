#pragma once

#include "ScriptAPI.h"

#include "BreakableObject.h"

class BreakableHealingDrop : public BreakableObject
{
    DECLARE_SCRIPT(BreakableHealingDrop)

public:
    explicit BreakableHealingDrop(GameObject* owner);

    void Start() override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

public:
    std::string m_healthPickupPrefabPath = "";

private:
    void onBreak() override;

};