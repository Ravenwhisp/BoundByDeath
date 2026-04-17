#include "pch.h"
#include "LevelCheats.h"
#include "KeyCode.h"
#include "PlayerState.h"
#include "PlayerDamageable.h"

//static const ScriptFieldInfo myScriptFields[] =
//{
//    { "Level Name", ScriptFieldType::String, offsetof(LevelCheats, levelName) },
//    { "Win Scene Name", ScriptFieldType::String, offsetof(LevelCheats, winSceneName) },
//    { "Lose Scene Name", ScriptFieldType::String, offsetof(LevelCheats, loseSceneName)}
//};
//
//IMPLEMENT_SCRIPT_FIELDS(LevelCheats, myScriptFields)

LevelCheats::LevelCheats(GameObject* owner)
    : Script(owner)
{
}

void LevelCheats::Start()
{
}

void LevelCheats::Update()
{
    if(Input::isKeyDown(KeyCode::K) && Input::isKeyDown(KeyCode::RightShift))
    {
        AutoWin();
	}

    if(Input::isKeyDown(KeyCode::L) && Input::isKeyDown(KeyCode::RightShift))
    {
        AutoLose();
    }
    if(Input::isKeyDown(KeyCode::T) && Input::isKeyDown(KeyCode::RightShift))
    {
        Teleport();
    }
    if(Input::isKeyDown(KeyCode::J) && Input::isKeyDown(KeyCode::RightShift))
    {
        ActivateGodMode();
    }
    if(Input::isKeyDown(KeyCode::E) && Input::isKeyDown(KeyCode::RightShift))
    {
        SpawnEnemies();
    }
    if(Input::isKeyDown(KeyCode::W) && Input::isKeyDown(KeyCode::RightShift))
    {
        RestoreHealth();
    }
    if(Input::isKeyDown(KeyCode::D) && Input::isKeyDown(KeyCode::RightShift))
    {
        DownState();
    }
    if(Input::isKeyDown(KeyCode::R) && Input::isKeyDown(KeyCode::RightShift))
    {
        restartLevel();
	}
}

void LevelCheats::AutoWin()
{
    Debug::warn("AutoWin activated!");
	SceneAPI::requestSceneChange("WinScene");
}

void LevelCheats::AutoLose()
{
    Debug::log("AutoLose activated!");
    SceneAPI::requestSceneChange("LoseScene");
}

void LevelCheats::Teleport()
{
    Debug::log("Teleport activated!");
    GameObject* player = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER)[0];
    Transform* playerTransform = GameObjectAPI::getTransform(player);
    TransformAPI::setPosition(playerTransform, Vector3(0.0f, 0.0f, 0.0f));
}

void LevelCheats::ActivateGodMode()
{
    Debug::log("God Mode activated!");
    
}

void LevelCheats::SpawnEnemies()
{
    Debug::log("Spawn Enemies activated!");
    
}

void LevelCheats::RestoreHealth()
{
    Debug::log("Restore Health activated!");
	GameObject* player = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER)[0];
    Script* damageableScript = GameObjectAPI::getScript(player, "PlayerDamageable");
    PlayerDamageable* damageable = dynamic_cast<PlayerDamageable*>(damageableScript);
    if (damageable)
    {
        damageable->heal(100);
	}
    
}

void LevelCheats::DownState()
{
    Debug::log("Down State activated!");
	GameObject* player = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER)[0];
    Script* damageableScript = GameObjectAPI::getScript(player, "PlayerState");
    PlayerState* playerState = dynamic_cast<PlayerState*>(damageableScript);
    if (playerState)
    {
        playerState->setState(PlayerStateType::Downed);
	}
    
}

void LevelCheats::restartLevel()
{
    Debug::log("Restart Level activated!");
    SceneAPI::requestSceneChange("Level1");
}

IMPLEMENT_SCRIPT(LevelCheats)
