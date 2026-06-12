#include "pch.h"
#include "PopUpController.h"

static const ScriptFieldInfo PopUpControllerFields[] =
{
};

IMPLEMENT_SCRIPT_FIELDS(PopUpController, PopUpControllerFields)

PopUpController::PopUpController(GameObject* owner)
    : Script(owner)
{
}

void PopUpController::Start()
{
}

void PopUpController::Update()
{
}

IMPLEMENT_SCRIPT(PopUpController)