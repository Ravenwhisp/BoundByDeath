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
    if(Input::isKeyDown(KeyCode::Q) && Input::isKeyDown(KeyCode::RightShift))
    {
        AutoWin();
	}

    if(Input::isKeyDown(KeyCode::W) && Input::isKeyDown(KeyCode::RightShift))
    {
        AutoLose();
    }
    if(Input::isKeyDown(KeyCode::E) && Input::isKeyDown(KeyCode::RightShift))
    {
        Teleport();
    }
    if(Input::isKeyDown(KeyCode::R) && Input::isKeyDown(KeyCode::RightShift))
    {
        ActivateGodMode();
    }
    if(Input::isKeyDown(KeyCode::T) && Input::isKeyDown(KeyCode::RightShift))
    {
        SpawnEnemies();
    }
    if(Input::isKeyDown(KeyCode::A) && Input::isKeyDown(KeyCode::RightShift))
    {
        if(Input::isKeyDown(KeyCode::Num1))
        {
			m_playerIndex = 0;
            RestoreHealth();
		}
        else if (Input::isKeyDown(KeyCode::Num2))
        {
            m_playerIndex = 1;
            RestoreHealth();
        }      
    }
    if(Input::isKeyDown(KeyCode::S) && Input::isKeyDown(KeyCode::RightShift))
    {
        if (Input::isKeyDown(KeyCode::Num1))
        {
            m_playerIndex = 0;
            DownState();
        }
        else if (Input::isKeyDown(KeyCode::Num2))
        {
            m_playerIndex = 1;
            DownState();
        }
    }
    if(Input::isKeyDown(KeyCode::D) && Input::isKeyDown(KeyCode::RightShift))
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
    std::vector<GameObject*> players = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER);

    for (GameObject* player : players)
    {
        Transform* playerTransform = GameObjectAPI::getTransform(player);
        TransformAPI::setPosition(playerTransform, Vector3(0.0f, 0.0f, 0.0f));
    }
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
    Debug::log("Restore Health activated! Player %i healed",m_playerIndex+1);
	GameObject* player = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER)[m_playerIndex];
    Script* damageableScript = GameObjectAPI::getScript(player, "PlayerDamageable");
    PlayerDamageable* damageable = dynamic_cast<PlayerDamageable*>(damageableScript);
    if (damageable)
    {
        damageable->heal(100);
	}
    
}

void LevelCheats::DownState()
{
    Debug::log("Down State activated! Player % i healed", m_playerIndex+1);
	GameObject* player = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER)[m_playerIndex];
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
