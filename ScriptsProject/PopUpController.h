#pragma once

#include "ScriptAPI.h"

class PopUpController : public Script
{
    DECLARE_SCRIPT(PopUpController)

public:
    explicit PopUpController(GameObject* owner);

    void Start() override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;
};