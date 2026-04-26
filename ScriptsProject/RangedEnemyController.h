#pragma once

#include "ScriptAPI.h"
#include "EnemyController.h"

class RangedEnemyController : public EnemyController
{
    DECLARE_SCRIPT(RangedEnemyController)

public:
    explicit RangedEnemyController(GameObject* owner);

    void OnStart() override;
    void OnUpdate() override;

    ScriptFieldList getExposedFields() const override;

    bool m_debugEnabled = true;
};