#pragma once

#include "ScriptAPI.h"

class TestingButton : public Script
{
public:
    explicit TestingButton(GameObject* owner);

    void Start() override;
    void Update() override;

    ScriptMethodList getExposedMethods() const override;

    void onFieldEdited(const ExposedFieldInfo& field) override;
    void onAfterDeserialize() override;

public:
    void ButtonHover();
    void ButtonPress();
    void ButtonRelease();
};
