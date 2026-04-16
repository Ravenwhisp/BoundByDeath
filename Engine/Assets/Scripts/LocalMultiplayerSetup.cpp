#include "pch.h"
#include "LocalMultiplayerSetup.h"

static const char* setupModeNames[] =
{
    "Keyboard + Gamepad",
    "Two Gamepads"
};

static const ScriptFieldInfo localMultiplayerSetupFields[] =
{
    { "Setup Mode", ScriptFieldType::EnumInt, offsetof(LocalMultiplayerSetup, m_setupMode), {}, { setupModeNames, 2 } },
    { "Keyboard + Gamepad Button", ScriptFieldType::ComponentRef, offsetof(LocalMultiplayerSetup, keyboardGamepadButton), {}, {}, { ComponentType::TRANSFORM } },
	{ "Two Gamepads Button", ScriptFieldType::ComponentRef, offsetof(LocalMultiplayerSetup, twoGamepadButton), {}, {}, { ComponentType::TRANSFORM } }
};

static const ScriptMethodInfo UIControllerMethods[] =
{
    {"Set Keyboard + Gamepad", [](Script* s) { static_cast<LocalMultiplayerSetup*>(s)->setKeyboardGamepad(); } },
    { "Set Two Gamepads", [](Script* s) { static_cast<LocalMultiplayerSetup*>(s)->setTwoGamepad(); } }
};

LocalMultiplayerSetup::LocalMultiplayerSetup(GameObject* owner)
    : Script(owner)
{
}

void LocalMultiplayerSetup::Start()
{
    chooseConfiguration();
}

ScriptFieldList LocalMultiplayerSetup::getExposedFields() const
{
    return { localMultiplayerSetupFields, sizeof(localMultiplayerSetupFields) / sizeof(ScriptFieldInfo) };
}

ScriptMethodList LocalMultiplayerSetup::getExposedMethods() const
{
    return { UIControllerMethods, sizeof(UIControllerMethods) / sizeof(ScriptMethodInfo) };
}

void LocalMultiplayerSetup::setKeyboardGamepad()
{
    setMode(0);
}

void LocalMultiplayerSetup::setTwoGamepad()
{
    setMode(1);
}

void LocalMultiplayerSetup::setMode(int mode)
{
    m_setupMode = mode;
    chooseConfiguration();
}

void LocalMultiplayerSetup::chooseConfiguration()
{
    Transform* kbComp = keyboardGamepadButton.getReferencedComponent();
    Transform* gpComp = twoGamepadButton.getReferencedComponent();
    switch (m_setupMode)
    {
    case 0:
        Input::setPlayerKeyboard(0);
        Input::setPlayerGamepad(1, 0);
        if (kbComp && gpComp)
        {
            kbComp->getOwner()->SetActive(true);
            gpComp->getOwner()->SetActive(false);
        }
        break;

    case 1:
        Input::setPlayerGamepad(0, 0);
        Input::setPlayerGamepad(1, 1);
        if (kbComp && gpComp)
        {
            kbComp->getOwner()->SetActive(false);
            gpComp->getOwner()->SetActive(true);
        }
        break;

    default:
        Input::setPlayerKeyboard(0);
        Input::setPlayerGamepad(1, 0);
        if (kbComp && gpComp)
        {
            kbComp->getOwner()->SetActive(true);
            gpComp->getOwner()->SetActive(false);
        }
        break;
    }
}

IMPLEMENT_SCRIPT(LocalMultiplayerSetup)