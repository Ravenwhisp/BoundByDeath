#include "pch.h"
#include "UIController.h"


static void ChangeScene(Script* s)
{
	static_cast<UIController*>(s)->ChangeScene("MainMenu");
}
static void SetActive(Script* s)
{
	static_cast<UIController*>(s)->setGameObjectActive(false);
}


UIController::UIController(GameObject* owner): Script(owner) {}

void UIController::Start()
{
}

void UIController::Update()
{
}

static const ScriptMethodInfo TestingButtonMethods[] =
{
	{ "ChangeScene", &ChangeScene, ScriptMethodParamType::String, "sceneName" },
	{ "SetActive", &SetActive, 
};

ScriptMethodList UIController::getExposedMethods() const
{
	return { TestingButtonMethods, sizeof(TestingButtonMethods) / sizeof(ScriptMethodInfo) };
}

void UIController::ChangeScene(const std::string& sceneName)
{
	SceneAPI::requestSceneChange(sceneName.c_str());
}

void UIController::setGameObjectActive(bool active)
{
	getOwner()->SetActive(active);
}

IMPLEMENT_SCRIPT(UIController)