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

    // Arrancar la música una sola vez por proceso (el motor de sonido Wwise es global
    // y persiste entre escenas). Si en runtime se observa que el audio se reinicia al
    // cambiar de escena, cambiar este guard por un PlayMusic() incondicional por escena.
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
        return; // ya está sonando (persistente entre escenas)
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

    // ===========================================================================
    // PENDIENTE DEL ENGINE: el AudioAPI actual NO expone SetState de Wwise.
    // En cuanto el engine añada AudioAPI::setState, descomentar la línea de abajo
    // (y quitar el Debug::log). Ver Downloads/PETICION_ENGINE_SetState.md
    //
    // AudioAPI::setState(k_stateGroup, stateValue);
    // ===========================================================================

    Debug::log("[MusicManager] (PENDIENTE setState) %s -> %s", k_stateGroup, stateValue);
}

void MusicManager::SetState_None()           { SetMusicState("None"); }
void MusicManager::SetState_MainMenu()       { SetMusicState("MainMenu"); }
void MusicManager::SetState_Level1Upper()    { SetMusicState("Level1_Upper"); }
void MusicManager::SetState_Level1Chapel()   { SetMusicState("Level1_Chapel"); }
void MusicManager::SetState_Level1Boss()     { SetMusicState("Level1_Boss"); }
void MusicManager::SetState_Level2Upper()    { SetMusicState("Level2_Upper"); }
void MusicManager::SetState_Level2Elevator() { SetMusicState("Level2_Elevator"); }
void MusicManager::SetState_FinalBoss()      { SetMusicState("FinalBoss"); }
