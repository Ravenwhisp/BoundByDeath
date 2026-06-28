#include "pch.h"
#include "MusicManager.h"
#include "MusicStates.h"

namespace
{
    constexpr const char* k_bank       = "MusicBoundByDeath.bnk";
    constexpr const char* k_stateGroup = "MusicState";

    constexpr const char* k_playMusic  = "Play_Music";
    constexpr const char* k_stopMusic  = "Stop_Music";
}

MusicManager* MusicManager::s_instance     = nullptr;
bool          MusicManager::s_musicStarted = false;

IMPLEMENT_SCRIPT(MusicManager)

IMPLEMENT_SCRIPT_FIELDS(MusicManager,
    SERIALIZED_ENUM_INT(m_sceneBaseState, "Scene Base State", kMusicStateNames, kMusicStateCount),
    SERIALIZED_BOOL(m_playMusicOnStart, "Play Music On Start")
)

MusicManager::MusicManager(GameObject* owner)
    : Script(owner)
{
}

void MusicManager::Start()
{
    s_instance = this;

    m_source = AudioAPI::getSoundSourceComponent(getOwner());
    if (m_source == nullptr)
    {
        Debug::error("[MusicManager] No SOUND_SOURCE component on '%s'.",
                     GameObjectAPI::getName(getOwner()));
    }

    // Arranca la música una sola vez (guard estático). El motor de sonido Wwise es
    // global y persiste entre escenas, así que solo se cambia el State -> Wwise hace
    // el crossfade entre canciones.
    if (m_playMusicOnStart && !s_musicStarted)
    {
        PlayMusic();
    }

    // Estado base de esta escena (configurable por escena desde el inspector).
    if (m_sceneBaseState >= 0 && m_sceneBaseState < kMusicStateCount)
    {
        SetMusicState(kMusicStateNames[m_sceneBaseState]);
    }
}

uint32_t MusicManager::postEvent(const char* eventName)
{
    if (m_source == nullptr)
    {
        return 0;
    }
    return AudioAPI::postEvent(m_source, k_bank, eventName);
}

void MusicManager::PlayMusic()
{
    if (s_musicStarted)
    {
        return;
    }
    m_musicPlayingId = postEvent(k_playMusic);
    s_musicStarted = true;
}

void MusicManager::StopMusic()
{
    postEvent(k_stopMusic);
    s_musicStarted   = false;
    m_musicPlayingId = 0;
}

void MusicManager::SetMusicState(const char* stateValue)
{
    if (stateValue == nullptr)
    {
        return;
    }

    // Cambia el State Group "MusicState"; el Music Switch Container (MUS_Main)
    // transiciona solo a la canción asociada (crossfade horneado en el bank).
    AudioAPI::setState(k_stateGroup, stateValue);

    Debug::log("[MusicManager] MusicState -> %s", stateValue);
}

void MusicManager::SetState_None()           { SetMusicState("None"); }
void MusicManager::SetState_MainMenu()       { SetMusicState("MainMenu"); }
void MusicManager::SetState_Level1Upper()    { SetMusicState("Level1_Upper"); }
void MusicManager::SetState_Level1Chapel()   { SetMusicState("Level1_Chapel"); }
void MusicManager::SetState_Level1Boss()     { SetMusicState("Level1_Boss"); }
void MusicManager::SetState_Level2Upper()    { SetMusicState("Level2_Upper"); }
void MusicManager::SetState_Level2Elevator() { SetMusicState("Level2_Elevator"); }
void MusicManager::SetState_FinalBoss()      { SetMusicState("FinalBoss"); }
