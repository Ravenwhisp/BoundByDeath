#include "pch.h"
#include "CameraTransitionController.h"

static const ScriptFieldInfo CameraTransitionControllerFields[] =
{
};

IMPLEMENT_SCRIPT_FIELDS(CameraTransitionController, CameraTransitionControllerFields)

CameraTransitionController::CameraTransitionController(GameObject* owner)
    : Script(owner)
{
}

void CameraTransitionController::Start()
{
}

void CameraTransitionController::Update()
{
}

IMPLEMENT_SCRIPT(CameraTransitionController)