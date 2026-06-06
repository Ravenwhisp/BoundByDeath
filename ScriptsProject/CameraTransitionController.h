#pragma once

#include "ScriptAPI.h"

class CameraTransitionController : public Script
{
    DECLARE_SCRIPT(CameraTransitionController)

public:
    explicit CameraTransitionController(GameObject* owner);

    void Start() override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;
};