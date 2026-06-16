#pragma once

#include "ScriptAPI.h"

class LocalMultiplayerSetup : public Script
{
public:
    explicit LocalMultiplayerSetup(GameObject* owner);

    void Start() override;

    ExposedFieldList getExposedFields() const override;
    ScriptMethodList getExposedMethods() const override;

    ScriptComponentRef<Transform> keyboardGamepadButton;
	ScriptComponentRef<Transform> twoGamepadButton;

    void setKeyboardGamepad();
    void setTwoGamepad();

private:
    void setMode(int mode);
    void chooseConfiguration();

public:
    int m_setupMode = 0;
};