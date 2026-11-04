#include "pch.h"
#include "TrailerPlayerShowcase.h"

#include "KeyCode.h"
#include "Damageable.h"
#include "EnemyBaseController.h"
#include "CharacterBase.h"
#include "DeathCharacter.h"
#include "LyrielCharacter.h"
#include "AbilityBase.h"
#include "AbilityDash.h"
#include "DeathTaunt.h"
#include "LyrielArrowVolley.h"
#include "PlayerGamepadBinding.h"
#include "PlayerController.h"
#include "HealthPickup.h"
#include "CameraFollow.h"
#include "HUDFader.h"
#include "EnemyDamageable.h"
#include "ShadowExecution.h"
#include "ReaperGauge.h"

#include <cmath>

IMPLEMENT_SCRIPT_FIELDS(TrailerPlayerShowcase,
    SERIALIZED_INT(m_startCharacter, "Start Character (0 Death, 1 Lyriel)"),
    SERIALIZED_BOOL(m_reelBothCharacters, "Reel Both Characters"),
    SERIALIZED_BOOL(m_autoReel, "Auto Reel"),
    SERIALIZED_FLOAT(m_startDelay, "Start Delay", 0.0f, 10.0f, 0.1f),
    SERIALIZED_ASSET_REF(m_dummyPrefab, "Dummy Enemy Prefab", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_spawnEffectPrefab, "Dummy Spawn Effect Prefab", AssetType::PREFAB),
    SERIALIZED_BOOL(m_useDummy, "Use Dummy Enemy"),
    SERIALIZED_FLOAT(m_deathDummyDistance, "Death Dummy Distance", 0.5f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_deathTauntDummyDistance, "Death Taunt Dummy Distance", 0.5f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_dummyDistanceLyriel, "Dummy Distance (Lyriel)", 0.5f, 15.0f, 0.1f),
    SERIALIZED_BOOL(m_dummyImmortal, "Dummy Immortal"),
    SERIALIZED_BOOL(m_dummyHoldInPlace, "Dummy Hold In Place"),
    SERIALIZED_FLOAT(m_modelShotTime, "Model Shot Time", 0.5f, 60.0f, 0.1f),
    SERIALIZED_FLOAT(m_turntableSpeed, "Turntable Speed", -180.0f, 180.0f, 1.0f),
    SERIALIZED_FLOAT(m_modelHeight, "Model Height", 0.5f, 6.0f, 0.05f),
    SERIALIZED_FLOAT(m_modelPadding, "Model Framing Padding", 1.0f, 4.0f, 0.05f),
    SERIALIZED_FLOAT(m_modelLookHeight, "Model Look Height", 0.0f, 4.0f, 0.05f),
    SERIALIZED_FLOAT(m_abilityCameraAngle, "Ability Camera Angle", -90.0f, 90.0f, 1.0f),
    SERIALIZED_FLOAT(m_abilityPadding, "Ability Framing Padding", 1.0f, 4.0f, 0.05f),
    SERIALIZED_FLOAT(m_abilityLookHeight, "Ability Look Height", 0.0f, 4.0f, 0.05f),
    SERIALIZED_FLOAT(m_stepGap, "Pause Between Abilities", 0.0f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_specialAimTime, "Special Aim Time", 0.0f, 5.0f, 0.05f),
    SERIALIZED_INT(m_basicComboPresses, "Basic Combo Presses"),
    SERIALIZED_FLOAT(m_cameraSmoothing, "Camera Smoothing", 0.5f, 30.0f, 0.1f),
    SERIALIZED_FLOAT(m_cameraTilt, "Camera Tilt", -0.5f, 2.0f, 0.01f),
    SERIALIZED_FLOAT(m_fov, "FOV (0 = keep)", 0.0f, 120.0f, 0.5f),
    SERIALIZED_BOOL(m_hideHUD, "Hide HUD"),
    SERIALIZED_COMPONENT_REF(m_light, "Key Light", ComponentType::TRANSFORM),
    SERIALIZED_FLOAT(m_lightHeight, "Key Light Height", -2.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_lightForward, "Key Light Forward", -10.0f, 10.0f, 0.05f),
    SERIALIZED_BOOL(m_duoMode, "Duo Mode (asymmetric roles)"),
    SERIALIZED_ASSET_REF(m_duoEnemyPrefab2, "Duo Second Enemy Prefab", AssetType::PREFAB),
    SERIALIZED_INT(m_duoEnemyCount, "Duo Enemy Count"),
    SERIALIZED_FLOAT(m_duoLyrielBack, "Duo Lyriel Back Distance", -10.0f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_duoLyrielSide, "Duo Lyriel Side Distance", -10.0f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_duoDeathFront, "Duo Death Front Distance", -5.0f, 5.0f, 0.1f),
    SERIALIZED_FLOAT(m_duoEnemyDistance, "Duo Enemy Distance From Death", 1.0f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_duoEnemySpread, "Duo Enemy Spread", 0.0f, 6.0f, 0.1f),
    SERIALIZED_FLOAT(m_duoCameraAngle, "Duo Camera Angle", -90.0f, 90.0f, 1.0f),
    SERIALIZED_FLOAT(m_duoPadding, "Duo Framing Padding", 1.0f, 3.0f, 0.05f),
    SERIALIZED_FLOAT(m_duoMeleeDistance, "Duo Death Melee Distance", 0.5f, 5.0f, 0.1f),
    SERIALIZED_FLOAT(m_duoTauntDistance, "Duo Death Taunt Distance", 1.0f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_duoDashRunUp, "Duo Death Dash Run-up", 0.0f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_duoDashMinDistance, "Duo Death Dash Min Distance", 0.0f, 15.0f, 0.1f),
    SERIALIZED_FLOAT(m_duoLyrielDashSpot, "Duo Lyriel Dash Spot Distance", 0.0f, 10.0f, 0.1f),
    SERIALIZED_INT(m_duoEarlyKills, "Duo Enemies Killed By First Combo"),
    SERIALIZED_INT(m_duoFinaleSurvivors, "Duo Enemies Left For Finale"),
    SERIALIZED_FLOAT(m_duoMortalHealth, "Duo Health When Marked To Die", 0.01f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_duoBeatGap, "Duo Death Pause Between Moves", 0.0f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_duoLyrielGap, "Duo Lyriel Pause Between Attacks", 0.0f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_duoFinaleSlowMotion, "Duo Finale Slow Motion (1 = off)", 0.05f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_duoLoopDelay, "Duo Loop Delay", 0.0f, 20.0f, 0.1f),
    SERIALIZED_BOOL(m_lastStandMode, "Last Stand Mode"),
    SERIALIZED_INT(m_lsWaves, "Last Stand Waves"),
    SERIALIZED_INT(m_lsWaveSize, "Last Stand Wave Size"),
    SERIALIZED_FLOAT(m_lsWaveInterval, "Last Stand Wave Interval", 0.5f, 30.0f, 0.1f),
    SERIALIZED_FLOAT(m_lsRingRadius, "Last Stand Ring Radius", 2.0f, 15.0f, 0.1f),
    SERIALIZED_FLOAT(m_lsClimaxDelay, "Last Stand Climax Delay", 0.0f, 30.0f, 0.1f),
    SERIALIZED_FLOAT(m_lsClimaxSlowMotion, "Last Stand Climax Slow Motion", 0.05f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_lsClimaxRamp, "Last Stand Climax Ramp", 0.1f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_lsCameraDistance, "Last Stand Camera Distance", 2.0f, 30.0f, 0.1f),
    SERIALIZED_FLOAT(m_lsCameraHeight, "Last Stand Camera Height", 0.2f, 20.0f, 0.1f),
    SERIALIZED_FLOAT(m_lsLookHeight, "Last Stand Look Height", 0.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_lsOrbitSpeed, "Last Stand Orbit Speed", -90.0f, 90.0f, 0.5f),
    SERIALIZED_FLOAT(m_lsPushIn, "Last Stand Climax Push-in", 0.2f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_lsClimaxHeight, "Last Stand Climax Camera Height", 0.2f, 20.0f, 0.1f),
    SERIALIZED_BOOL(m_lsHideHealthBars, "Last Stand Hide Health Bars In Climax"),
    SERIALIZED_FLOAT(m_lsRevealHeight, "Last Stand Reveal Height", 1.0f, 40.0f, 0.1f),
    SERIALIZED_FLOAT(m_lsRevealDistance, "Last Stand Reveal Distance", 0.5f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_lsRevealTime, "Last Stand Reveal Time", 0.5f, 20.0f, 0.1f),
    SERIALIZED_BOOL(m_syncMode, "Sync Mode (paired abilities + execution showpiece)"),
    SERIALIZED_FLOAT(m_syncSeparation, "Sync Separation", 0.5f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_syncOutwardAngle, "Sync Outward Angle", -90.0f, 180.0f, 1.0f),
    SERIALIZED_FLOAT(m_syncCameraDistance, "Sync Camera Distance", 1.0f, 20.0f, 0.1f),
    SERIALIZED_FLOAT(m_syncCameraHeight, "Sync Camera Height", 0.1f, 15.0f, 0.1f),
    SERIALIZED_FLOAT(m_syncLookHeight, "Sync Look Height", 0.0f, 4.0f, 0.05f),
    SERIALIZED_FLOAT(m_syncBeatGap, "Sync Pause Between Beats", 0.0f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_execCameraDistance, "Execution Showpiece Camera Distance", 1.0f, 15.0f, 0.05f),
    SERIALIZED_FLOAT(m_execCameraHeight, "Execution Showpiece Camera Height", 0.05f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_execLookHeight, "Execution Showpiece Look Height", 0.0f, 4.0f, 0.05f),
    SERIALIZED_FLOAT(m_execOrbitSpeed, "Execution Showpiece Orbit Speed", -90.0f, 90.0f, 0.5f),
    SERIALIZED_FLOAT(m_execSlowMotion, "Execution Showpiece Slow Motion", 0.05f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_execHold, "Execution Showpiece Hold", 0.0f, 10.0f, 0.1f),
    SERIALIZED_BOOL(m_introMode, "Intro Mode (character intros + bullet-time cross)"),
    SERIALIZED_FLOAT(m_introWalkDistance, "Intro Walk Distance", 0.0f, 20.0f, 0.1f),
    SERIALIZED_FLOAT(m_introCameraDistance, "Intro Camera Distance", 1.0f, 15.0f, 0.05f),
    SERIALIZED_FLOAT(m_introCameraHeight, "Intro Camera Height", 0.1f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_introLookHeight, "Intro Look Height", 0.0f, 4.0f, 0.05f),
    SERIALIZED_FLOAT(m_introCameraAngle, "Intro Camera Angle", -90.0f, 90.0f, 1.0f),
    SERIALIZED_FLOAT(m_introFlourishAngle, "Intro Flourish Angle", -180.0f, 180.0f, 1.0f),
    SERIALIZED_FLOAT(m_introHoldTime, "Intro Hero Hold Time", 0.0f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_introHoldPushIn, "Intro Hold Push-in", 0.3f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_crossStandoff, "Cross Stand-off Time", 0.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_crossBulletTime, "Cross Bullet Time Scale", 0.01f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_crossOrbitDegrees, "Cross Camera Swing Degrees", -360.0f, 360.0f, 5.0f),
    SERIALIZED_FLOAT(m_crossOrbitTime, "Cross Camera Swing Time", 0.2f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_crossCameraDistance, "Cross Camera Distance", 1.0f, 15.0f, 0.05f),
    SERIALIZED_FLOAT(m_crossCameraHeight, "Cross Camera Height", 0.1f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_crossLandHold, "Cross Landing Hold", 0.0f, 10.0f, 0.1f),
    SERIALIZED_BOOL(m_playersInvulnerable, "Players Invulnerable"),
    SERIALIZED_BOOL(m_ignorePlayerInput, "Ignore Player Input"),
    SERIALIZED_FLOAT(m_parkDistance, "Park Other Character Distance", 2.0f, 60.0f, 0.5f),
    SERIALIZED_FLOAT(m_slowMotionScale, "Slow Motion Scale", 0.05f, 1.0f, 0.01f),
    SERIALIZED_BOOL(m_removeHealthDrops, "Remove Health Drops"),
    SERIALIZED_BOOL(m_logControlsOnStart, "Log Controls On Start")
)

namespace
{
    constexpr float kPi = 3.14159265358979f;
    constexpr NavAgentProfile kEnemyNavProfile = static_cast<NavAgentProfile>(2); // see TrailerEnemyShowcase
    constexpr float kPressTimeout = 2.5f;
    constexpr float kRunTimeout = 10.0f;
    constexpr float kMinRunTime = 0.8f;
    constexpr float kDummySpawnDelay = 0.8f;
    constexpr int kUnusedGamepadSlot = 1;

    Vector3 forwardFromYaw(float yawDegrees)
    {
        const float radians = yawDegrees * (kPi / 180.0f);
        return Vector3(std::sin(radians), 0.0f, std::cos(radians));
    }

    float yawFromDirection(const Vector3& direction)
    {
        return std::atan2(direction.x, direction.z) * (180.0f / kPi);
    }

    Vector3 snapToNavMesh(const Vector3& position, NavAgentProfile profile)
    {
        Vector3 sampled;
        if (NavigationAPI::hasNavMesh() &&
            NavigationAPI::samplePosition(position, sampled, Vector3(2.0f, 5.0f, 2.0f), profile))
        {
            return sampled;
        }
        return position;
    }
}

TrailerPlayerShowcase::TrailerPlayerShowcase(GameObject* owner)
    : Script(owner)
{
}

void TrailerPlayerShowcase::Start()
{
    m_death = nullptr;
    m_lyriel = nullptr;
    m_characterIndex = (m_startCharacter == 1) ? 1 : 0;
    m_reelRunning = false;
    m_singleStep = false;
    m_dummy = nullptr;
    m_dummyPending = false;
    m_cameraActive = false;
    m_cameraFollow = nullptr;
    m_slowMotionActive = false;
    m_startDone = false;
    m_startTimer = 0.0f;
    m_invulnerabilityApplied = false;
    m_zoomScale = 1.0f;
    m_orbitOffset = 0.0f;
    m_duoEnemies.clear();
    m_deathTrack = DuoTrack();
    m_lyrielTrack = DuoTrack();
    m_duoRunning = false;
    m_shadowExecution = nullptr;
    m_finaleSlowApplied = false;

    if (m_logControlsOnStart)
    {
        logControls();
    }
}

void TrailerPlayerShowcase::OnGameStop()
{
    Time::setTimeScale(1.0f);
    m_slowMotionActive = false;
    setCamera(false);
    applyInputDetach(false);
    m_effectTracker.clear();
    m_dummy = nullptr;
    m_dummyPending = false;
    // No calls into the characters here: the scene is shutting down. PlayerController starts fresh next Play.
    m_deathTrack = DuoTrack();
    m_lyrielTrack = DuoTrack();
    m_duoEnemies.clear();
    m_duoRunning = false;
}

void TrailerPlayerShowcase::Update()
{
    const float deltaTime = Time::getDeltaTime();

    // First frame: every other script has run Start, so the characters and their abilities are wired up.
    if (!m_invulnerabilityApplied)
    {
        findCharacters();
        setPlayersInvulnerable(m_playersInvulnerable);
        applyInputDetach(m_ignorePlayerInput);
        m_invulnerabilityApplied = true;
    }

    if (m_removeHealthDrops)
    {
        removeHealthDrops();
    }

    handleInput(deltaTime);

    if (m_syncMode)
    {
        if (!m_startDone)
        {
            m_startTimer += deltaTime;
            if (m_startTimer >= m_startDelay)
            {
                m_startDone = true;
                setCamera(true);
                startSync();
            }
        }

        updateSync(deltaTime);
        updateSyncCamera(deltaTime);
        return;
    }

    if (m_introMode)
    {
        if (!m_startDone)
        {
            m_startTimer += deltaTime;
            if (m_startTimer >= m_startDelay)
            {
                m_startDone = true;
                setCamera(true);
                startIntroShot(IntroShot::DeathIntro);
            }
        }

        updateIntro(deltaTime);
        updateIntroCamera(deltaTime);
        return;
    }

    if (m_lastStandMode)
    {
        if (!m_startDone)
        {
            m_startTimer += deltaTime;
            if (m_startTimer >= m_startDelay)
            {
                m_startDone = true;
                setCamera(true);
                startLastStand();
            }
        }

        m_effectTracker.update(deltaTime);
        updateDuoEnemies(deltaTime);
        updateLastStand(deltaTime);
        updateLastStandCamera(deltaTime);
        return;
    }

    if (m_duoMode)
    {
        if (!m_startDone)
        {
            m_startTimer += deltaTime;
            if (m_startTimer >= m_startDelay)
            {
                m_startDone = true;
                setCamera(true);
                startDuo();
            }
        }

        m_effectTracker.update(deltaTime);
        updateDuoEnemies(deltaTime);
        updateDuo(deltaTime);
        updateDuoCamera(deltaTime);
        return;
    }

    if (!m_startDone)
    {
        m_startTimer += deltaTime;
        if (m_startTimer >= m_startDelay)
        {
            m_startDone = true;
            setCamera(true);
            m_singleStep = !m_autoReel;
            enterStep(ReelStep::Model);
        }
    }

    m_effectTracker.update(deltaTime);
    updateDummy();
    updateReel(deltaTime);
    updateCamera(deltaTime);
    updateLight();
}

// ---------------------------------------------------------------------------
// Input
// ---------------------------------------------------------------------------

bool TrailerPlayerShowcase::comboPressed(KeyCode key)
{
    const bool isDown = Input::isKeyDown(KeyCode::RightShift) && Input::isKeyDown(key);
    const bool wasDown = m_previousKeyStates[key];
    m_previousKeyStates[key] = isDown;
    return isDown && !wasDown;
}

bool TrailerPlayerShowcase::comboHeld(KeyCode key) const
{
    return Input::isKeyDown(KeyCode::RightShift) && Input::isKeyDown(key);
}

void TrailerPlayerShowcase::handleInput(float deltaTime)
{
    if (m_syncMode)
    {
        handleSyncInput();
    }
    else if (m_introMode)
    {
        handleIntroInput();
    }
    else if (m_duoMode || m_lastStandMode)
    {
        handleDuoInput();
    }
    else
    {
        handleSoloInput();
    }

    if (comboPressed(KeyCode::Tab)) setCamera(!m_cameraActive);
    if (comboPressed(KeyCode::O))   toggleSlowMotion();

    if (comboPressed(KeyCode::U))
    {
        m_playersInvulnerable = !m_playersInvulnerable;
        setPlayersInvulnerable(m_playersInvulnerable);
    }

    const float zoomRate = 0.6f;
    if (comboHeld(KeyCode::Up))   m_zoomScale -= zoomRate * deltaTime;
    if (comboHeld(KeyCode::Down)) m_zoomScale += zoomRate * deltaTime;
    if (m_zoomScale < 0.3f) m_zoomScale = 0.3f;
    if (m_zoomScale > 3.0f) m_zoomScale = 3.0f;

    const float orbitRate = 30.0f;
    if (comboHeld(KeyCode::Num1)) m_orbitOffset -= orbitRate * deltaTime;
    if (comboHeld(KeyCode::Num2)) m_orbitOffset += orbitRate * deltaTime;
}

void TrailerPlayerShowcase::handleSoloInput()
{
    // Manual shots. With the auto reel on, the reel carries on from the chosen step.
    struct ManualKey { KeyCode key; ReelStep step; };
    const ManualKey manualKeys[] =
    {
        { KeyCode::Space, ReelStep::Model },
        { KeyCode::J, ReelStep::Basic },
        { KeyCode::K, ReelStep::Charged },
        { KeyCode::L, ReelStep::Dash },
        { KeyCode::I, ReelStep::Special },
    };

    for (const ManualKey& manual : manualKeys)
    {
        if (comboPressed(manual.key))
        {
            if (characterBusy())
            {
                Debug::log("[TrailerPlayerShowcase] %s is still busy, try again in a moment.", characterName());
                continue;
            }

            m_singleStep = !m_autoReel;
            enterStep(manual.step);
        }
    }

    const bool nextCharacter = comboPressed(KeyCode::Right);
    const bool previousCharacter = comboPressed(KeyCode::Left);
    if (nextCharacter || previousCharacter)
    {
        if (!characterBusy())
        {
            switchCharacter();
            m_singleStep = !m_autoReel;
            enterStep(ReelStep::Model);
        }
    }

    if (comboPressed(KeyCode::Num3))
    {
        m_autoReel = !m_autoReel;
        m_singleStep = !m_autoReel;
        if (m_autoReel && !m_reelRunning)
        {
            enterStep(ReelStep::Model);
        }
        Debug::log("[TrailerPlayerShowcase] Auto reel %s", m_autoReel ? "ON" : "OFF");
    }

    if (comboPressed(KeyCode::R))
    {
        removeDummy();
        m_reelRunning = false;
        m_singleStep = !m_autoReel;
        enterStep(ReelStep::Model);
    }

    if (comboPressed(KeyCode::Num4))
    {
        m_useDummy = !m_useDummy;
        if (!m_useDummy)
        {
            removeDummy();
        }
        Debug::log("[TrailerPlayerShowcase] Dummy enemy %s (from the next ability).", m_useDummy ? "ON" : "OFF");
    }
}

// ---------------------------------------------------------------------------
// Characters and abilities
// ---------------------------------------------------------------------------

void TrailerPlayerShowcase::findCharacters()
{
    if (m_death == nullptr)
    {
        const std::vector<GameObject*> holders = SceneAPI::findAllGameObjectsWithScript<DeathCharacter>();
        if (!holders.empty())
        {
            m_death = GameObjectAPI::findScript<DeathCharacter>(holders[0]);
        }
    }

    if (m_lyriel == nullptr)
    {
        const std::vector<GameObject*> holders = SceneAPI::findAllGameObjectsWithScript<LyrielCharacter>();
        if (!holders.empty())
        {
            m_lyriel = GameObjectAPI::findScript<LyrielCharacter>(holders[0]);
        }
    }

    if (m_death == nullptr || m_lyriel == nullptr)
    {
        Debug::warn("[TrailerPlayerShowcase] Couldn't find %s in this scene.",
            m_death == nullptr ? (m_lyriel == nullptr ? "Death or Lyriel" : "Death") : "Lyriel");
    }
}

CharacterBase* TrailerPlayerShowcase::activeCharacter() const
{
    return m_characterIndex == 0 ? m_death : m_lyriel;
}

CharacterBase* TrailerPlayerShowcase::otherCharacter() const
{
    return m_characterIndex == 0 ? m_lyriel : m_death;
}

const char* TrailerPlayerShowcase::characterName() const
{
    return m_characterIndex == 0 ? "Death" : "Lyriel";
}

const char* TrailerPlayerShowcase::stepName(ReelStep step) const
{
    switch (step)
    {
    case ReelStep::Model:   return "model shot";
    case ReelStep::Basic:   return "basic combo";
    case ReelStep::Charged: return "charged attack";
    case ReelStep::Dash:    return "dash";
    case ReelStep::Special: return m_characterIndex == 0 ? "taunt" : "arrow volley";
    default:                return "?";
    }
}

AbilityBase* TrailerPlayerShowcase::abilityFor(ReelStep step) const
{
    CharacterBase* character = activeCharacter();
    if (character == nullptr)
    {
        return nullptr;
    }

    switch (step)
    {
    case ReelStep::Basic:   return character->getBasicAttack();
    case ReelStep::Charged: return character->getChargedAttack();
    case ReelStep::Special: return character->getSpecialAbility();
    case ReelStep::Dash:    return GameObjectAPI::findScript<AbilityDash>(character->getOwner());
    default:                return nullptr;
    }
}

bool TrailerPlayerShowcase::characterBusy() const
{
    CharacterBase* character = activeCharacter();
    if (character == nullptr)
    {
        return false;
    }

    return character->isUsingAbility() || character->getActiveAbility() != nullptr;
}

void TrailerPlayerShowcase::switchCharacter()
{
    m_characterIndex = 1 - m_characterIndex;
    removeDummy(); // different range for the other character
    Debug::log("[TrailerPlayerShowcase] Now showing %s.", characterName());
}

// ---------------------------------------------------------------------------
// Reel
// ---------------------------------------------------------------------------

void TrailerPlayerShowcase::enterStep(ReelStep step)
{
    findCharacters();
    if (activeCharacter() == nullptr)
    {
        return;
    }

    m_step = step;
    m_phase = StepPhase::Prepare;
    m_stepTimer = 0.0f;
    m_reelRunning = true;
    m_specialReleased = false;

    if (!getStage(m_stage, m_stageYaw))
    {
        return;
    }

    const bool modelShot = (step == ReelStep::Model);
    if (modelShot)
    {
        removeDummy();
        m_characterMark = snapToNavMesh(m_stage, NavAgentProfile::PlayerNormal);
    }
    else
    {
        // Side-on layout: character on the left of frame, dummy on the right, camera slightly round
        // towards the character's front.
        float distance = m_dummyDistanceLyriel;
        if (m_characterIndex == 0)
        {
            distance = (step == ReelStep::Special) ? m_deathTauntDummyDistance : m_deathDummyDistance;
        }
        const float yawRadians = m_stageYaw * (kPi / 180.0f);
        const Vector3 right(std::cos(yawRadians), 0.0f, -std::sin(yawRadians));

        m_characterMark = snapToNavMesh(m_stage - right * (distance * 0.5f), NavAgentProfile::PlayerNormal);
        m_dummyMark = snapToNavMesh(m_stage + right * (distance * 0.5f), kEnemyNavProfile);

        // Without a dummy the mark still sets which way the character faces and what the camera frames.
        if (m_useDummy)
        {
            ensureDummy();
        }
        else
        {
            removeDummy();
        }
    }

    stageCharacter(modelShot);
    parkOtherCharacter();

    m_orbitOffset = 0.0f;
    m_cameraSnapNext = true;

    Debug::log("[TrailerPlayerShowcase] %s: %s", characterName(), stepName(step));
}

void TrailerPlayerShowcase::stageCharacter(bool faceCamera)
{
    CharacterBase* character = activeCharacter();
    Transform* transform = character ? GameObjectAPI::getTransform(character->getOwner()) : nullptr;
    if (transform == nullptr)
    {
        return;
    }

    TransformAPI::setGlobalPosition(transform, m_characterMark);

    const Vector3 facing = faceCamera ? forwardFromYaw(m_stageYaw) : (m_dummyMark - m_characterMark);
    TransformAPI::setGlobalRotationEuler(transform, Vector3(0.0f, yawFromDirection(facing), 0.0f));
}

void TrailerPlayerShowcase::parkOtherCharacter()
{
    CharacterBase* other = otherCharacter();
    Transform* transform = other ? GameObjectAPI::getTransform(other->getOwner()) : nullptr;
    if (transform == nullptr)
    {
        return;
    }

    // Behind the camera on the same line: out of frame and out of the dummy's way.
    const Vector3 parked = snapToNavMesh(m_stage + forwardFromYaw(m_stageYaw) * m_parkDistance, NavAgentProfile::PlayerNormal);
    TransformAPI::setGlobalPosition(transform, parked);
}

void TrailerPlayerShowcase::updateReel(float deltaTime)
{
    if (!m_reelRunning)
    {
        return;
    }

    m_stepTimer += deltaTime;

    if (m_step == ReelStep::Model)
    {
        m_orbitOffset += m_turntableSpeed * deltaTime;

        if (m_stepTimer >= m_modelShotTime)
        {
            m_phase = StepPhase::Gap;
        }
        else
        {
            return;
        }
    }

    switch (m_phase)
    {
    case StepPhase::Prepare:
        // Give the dummy time to arrive and the character a beat on its mark.
        if (m_stepTimer >= 0.6f && (!m_useDummy || !m_dummyPrefab.m_id.isValid() || (isDummyAlive() && !m_dummyPending && m_dummyFrames > 3)))
        {
            beginPressing();
        }
        else if (m_stepTimer >= 4.0f)
        {
            beginPressing(); // dummy failed to appear: show the ability anyway
        }
        break;

    case StepPhase::Pressing:
        updatePressing(deltaTime);
        break;

    case StepPhase::Running:
        if (m_step == ReelStep::Special && !m_specialReleased && m_stepTimer >= m_specialAimTime)
        {
            releaseSpecial();
        }

        if ((m_stepTimer >= kMinRunTime && !characterBusy()) || m_stepTimer >= kRunTimeout)
        {
            m_phase = StepPhase::Gap;
            m_stepTimer = 0.0f;
        }
        break;

    case StepPhase::Gap:
        if (m_step == ReelStep::Model)
        {
            m_stepTimer = m_stepGap; // the turntable already held long enough
        }

        if (m_stepTimer >= m_stepGap)
        {
            if (m_singleStep)
            {
                m_reelRunning = false;
                return;
            }

            ReelStep next = static_cast<ReelStep>(static_cast<int>(m_step) + 1);
            if (next == ReelStep::Count)
            {
                if (m_reelBothCharacters)
                {
                    switchCharacter();
                }
                next = ReelStep::Model;
            }

            enterStep(next);
        }
        break;
    }
}

void TrailerPlayerShowcase::beginPressing()
{
    AbilityBase* ability = abilityFor(m_step);
    if (ability == nullptr)
    {
        Debug::warn("[TrailerPlayerShowcase] %s has no %s ability script.", characterName(), stepName(m_step));
        m_phase = StepPhase::Gap;
        m_stepTimer = 0.0f;
        return;
    }

    // Face the dummy right before the move, in case the last step left the character turned.
    stageCharacter(false);

    m_phase = StepPhase::Pressing;
    m_stepTimer = 0.0f;
    m_pressTimer = 0.0f;
    m_pressCooldown = 0.0f;
    m_pressBaseline = ability->getSuccessfulUse();
    m_pressesLeft = (m_step == ReelStep::Basic) ? (m_basicComboPresses < 1 ? 1 : m_basicComboPresses) : 1;

    if (m_step != ReelStep::Basic)
    {
        // One press only: a second tryAbility could cancel and restart a charge or aim.
        ability->tryAbility();
        m_phase = StepPhase::Running;
        m_stepTimer = 0.0f;
    }
}

void TrailerPlayerShowcase::updatePressing(float deltaTime)
{
    AbilityBase* ability = abilityFor(m_step);
    if (ability == nullptr)
    {
        m_phase = StepPhase::Running;
        return;
    }

    m_pressTimer += deltaTime;
    if (m_pressCooldown > 0.0f)
    {
        m_pressCooldown -= deltaTime;
        return;
    }

    // Like mashing the button: try every frame and count each hit that actually starts.
    ability->tryAbility();

    if (ability->getSuccessfulUse() > m_pressBaseline)
    {
        m_pressBaseline = ability->getSuccessfulUse();
        --m_pressesLeft;
        m_pressTimer = 0.0f;
        m_pressCooldown = 0.15f;
    }

    if (m_pressesLeft <= 0 || m_pressTimer >= kPressTimeout)
    {
        m_phase = StepPhase::Running;
        m_stepTimer = 0.0f;
    }
}

void TrailerPlayerShowcase::releaseSpecial()
{
    m_specialReleased = true;

    CharacterBase* character = activeCharacter();
    if (character == nullptr)
    {
        return;
    }

    // Both specials aim while the trigger is held and fire when it comes up; there is no trigger here.
    if (DeathTaunt* taunt = GameObjectAPI::findScript<DeathTaunt>(character->getOwner()))
    {
        taunt->releaseFromScript();
    }
    else if (LyrielArrowVolley* volley = GameObjectAPI::findScript<LyrielArrowVolley>(character->getOwner()))
    {
        volley->releaseFromScript();
    }
}

// ---------------------------------------------------------------------------
// Training dummy
// ---------------------------------------------------------------------------

void TrailerPlayerShowcase::ensureDummy()
{
    if (isDummyAlive() || m_dummyPending)
    {
        // Already there: just put it back on its mark.
        if (m_dummy != nullptr && SceneAPI::containsGameObject(m_dummy))
        {
            if (Transform* transform = GameObjectAPI::getTransform(m_dummy))
            {
                TransformAPI::setGlobalPosition(transform, m_dummyMark);
            }
        }
        return;
    }

    if (!m_dummyPrefab.m_id.isValid())
    {
        Debug::warn("[TrailerPlayerShowcase] No Dummy Enemy Prefab assigned; abilities will play without a target.");
        return;
    }

    removeDummy();
    m_dummyPending = true;
    m_dummySpawnTimer = 0.0f;
    m_dummyEffectPlayed = false;
}

void TrailerPlayerShowcase::removeDummy()
{
    if (m_dummy != nullptr && SceneAPI::containsGameObject(m_dummy))
    {
        GameObjectAPI::removeGameObject(m_dummy);
    }

    m_dummy = nullptr;
    m_dummyPending = false;
    m_dummySpeedLocked = false;
}

bool TrailerPlayerShowcase::isDummyAlive() const
{
    if (m_dummy == nullptr)
    {
        return false;
    }

    if (!SceneAPI::containsGameObject(m_dummy))
    {
        // New objects only join the scene list a frame later.
        return !m_dummySeen && m_dummyFrames < 10;
    }

    const Damageable* damageable = GameObjectAPI::findScript<Damageable>(m_dummy);
    return damageable == nullptr || !damageable->isDead();
}

void TrailerPlayerShowcase::updateDummy()
{
    if (m_dummyPending)
    {
        m_dummySpawnTimer += Time::getDeltaTime();

        const bool useEffect = m_spawnEffectPrefab.m_id.isValid();
        if (useEffect && !m_dummyEffectPlayed)
        {
            m_dummyEffectPlayed = true;
            ParticleLifecycle::spawnOneShotTimed(m_effectTracker, m_spawnEffectPrefab.m_id, m_dummyMark, Vector3::Zero, 3.0f);
        }

        if (!useEffect || m_dummySpawnTimer >= kDummySpawnDelay)
        {
            m_dummyPending = false;
            const float yaw = yawFromDirection(m_characterMark - m_dummyMark);
            m_dummy = GameObjectAPI::instantiatePrefab(m_dummyPrefab.m_id, m_dummyMark, Vector3(0.0f, yaw, 0.0f));
            m_dummyFrames = 0;
            m_dummySeen = false;
            m_dummySpeedLocked = false;

            if (m_dummy == nullptr)
            {
                Debug::warn("[TrailerPlayerShowcase] Failed to spawn the dummy enemy.");
            }
        }
        return;
    }

    if (m_dummy == nullptr)
    {
        return;
    }

    ++m_dummyFrames;
    if (!SceneAPI::containsGameObject(m_dummy))
    {
        if (m_dummySeen || m_dummyFrames > 10)
        {
            m_dummy = nullptr; // died and was cleaned up; the next ability step spawns a fresh one
        }
        return;
    }
    m_dummySeen = true;

    Damageable* damageable = GameObjectAPI::findScript<Damageable>(m_dummy);
    if (damageable == nullptr || damageable->isDead() || m_dummyFrames < 3)
    {
        return;
    }

    // Hits still land (flash, shake, health bar), it just never runs out.
    if (m_dummyImmortal && damageable->getCurrentHp() < damageable->getMaxHp() * 0.6f)
    {
        damageable->heal(damageable->getMaxHp());
    }

    if (!m_dummyHoldInPlace)
    {
        return;
    }

    if (EnemyBaseController* controller = GameObjectAPI::findScript<EnemyBaseController>(m_dummy))
    {
        if (!m_dummySpeedLocked)
        {
            controller->m_moveSpeed = 0.0f;
            m_dummySpeedLocked = true;
        }
    }

    // Death's taunt pulls enemies towards him; let that play out instead of fighting it.
    const bool tauntPlaying = (m_characterIndex == 0 && m_step == ReelStep::Special && m_phase != StepPhase::Prepare);
    if (!tauntPlaying)
    {
        if (Transform* transform = GameObjectAPI::getTransform(m_dummy))
        {
            TransformAPI::setGlobalPosition(transform, m_dummyMark);
        }
    }
}

// ---------------------------------------------------------------------------
// Camera
// ---------------------------------------------------------------------------

bool TrailerPlayerShowcase::getStage(Vector3& outPosition, float& outYaw) const
{
    Transform* stageTransform = GameObjectAPI::getTransform(getOwner());
    if (stageTransform == nullptr)
    {
        return false;
    }

    outPosition = snapToNavMesh(TransformAPI::getGlobalPosition(stageTransform), NavAgentProfile::PlayerNormal);
    outYaw = TransformAPI::getGlobalEulerDegrees(stageTransform).y;
    return true;
}

Vector3 TrailerPlayerShowcase::characterPosition() const
{
    CharacterBase* character = activeCharacter();
    Transform* transform = character ? GameObjectAPI::getTransform(character->getOwner()) : nullptr;
    return transform ? TransformAPI::getGlobalPosition(transform) : m_characterMark;
}

void TrailerPlayerShowcase::setCamera(bool active)
{
    GameObject* camera = SceneAPI::getDefaultCameraGameObject();
    if (camera == nullptr)
    {
        m_cameraActive = false;
        return;
    }

    if (m_cameraFollow == nullptr)
    {
        m_cameraFollow = GameObjectAPI::findScript<CameraFollow>(camera);
    }

    if (active == m_cameraActive)
    {
        return;
    }

    m_cameraActive = active;

    if (m_cameraFollow)
    {
        m_cameraFollow->setFollowEnabled(!active);
    }

    if (CameraComponent* cameraComponent = CameraAPI::getCameraComponent(camera))
    {
        if (active)
        {
            m_savedFov = CameraAPI::getFov(cameraComponent);
            if (m_fov > 0.0f)
            {
                CameraAPI::setFov(cameraComponent, m_fov);
            }
        }
        else if (m_savedFov > 0.0f)
        {
            CameraAPI::setFov(cameraComponent, m_savedFov);
        }
    }

    if (m_hideHUD)
    {
        for (GameObject* faderObject : SceneAPI::findAllGameObjectsWithScript<HUDFader>())
        {
            if (HUDFader* fader = GameObjectAPI::findScript<HUDFader>(faderObject))
            {
                fader->fadeTo(active ? 0.0f : 1.0f, 0.4f);
            }
        }
    }

    m_cameraSnapNext = true;
    Debug::log("[TrailerPlayerShowcase] Trailer camera %s", active ? "ON" : "OFF");
}

void TrailerPlayerShowcase::updateCamera(float deltaTime)
{
    if (!m_cameraActive || !m_startDone)
    {
        return;
    }

    GameObject* camera = SceneAPI::getDefaultCameraGameObject();
    Transform* cameraTransform = camera ? GameObjectAPI::getTransform(camera) : nullptr;
    if (cameraTransform == nullptr)
    {
        return;
    }

    // The engine's FOV is horizontal; derive the vertical one for a 16:9 frame.
    float horizontalFov = m_fov;
    if (horizontalFov <= 0.0f)
    {
        if (CameraComponent* cameraComponent = CameraAPI::getCameraComponent(camera))
        {
            horizontalFov = CameraAPI::getFov(cameraComponent);
        }
    }
    if (horizontalFov <= 1.0f || horizontalFov > 170.0f)
    {
        horizontalFov = 48.0f;
    }
    const float tanHalfH = std::tan(horizontalFov * 0.5f * (kPi / 180.0f));
    const float tanHalfV = tanHalfH / (16.0f / 9.0f);

    const Vector3 character = characterPosition();
    Vector3 focus;
    float yaw;
    float halfWidth;
    float halfHeight;
    float lookHeight;

    if (m_step == ReelStep::Model)
    {
        focus = character;
        yaw = m_stageYaw + m_orbitOffset;
        halfHeight = 0.5f * m_modelHeight * m_modelPadding;
        halfWidth = 0.6f * m_modelPadding;
        lookHeight = m_modelLookHeight;
    }
    else
    {
        // Keep the character and the dummy (or its mark) in frame together.
        Vector3 dummy = m_dummyMark;
        if (m_dummy != nullptr && SceneAPI::containsGameObject(m_dummy))
        {
            if (Transform* dummyTransform = GameObjectAPI::getTransform(m_dummy))
            {
                dummy = TransformAPI::getGlobalPosition(dummyTransform);
            }
        }

        focus = (character + dummy) * 0.5f;
        yaw = m_stageYaw + m_abilityCameraAngle + m_orbitOffset;

        const Vector3 span = dummy - character;
        const float separation = std::sqrt(span.x * span.x + span.z * span.z);
        const float sideways = separation * std::cos(m_abilityCameraAngle * (kPi / 180.0f));
        halfWidth = (0.5f * sideways + 1.0f) * m_abilityPadding;
        halfHeight = 0.5f * m_modelHeight * m_abilityPadding;
        lookHeight = m_abilityLookHeight;
    }

    const float fitHeight = halfHeight / tanHalfV;
    const float fitWidth = halfWidth / tanHalfH;
    const float distance = ((fitHeight > fitWidth ? fitHeight : fitWidth) + 0.5f) * m_zoomScale;

    const Vector3 lookTarget = focus + Vector3(0.0f, lookHeight, 0.0f);
    const Vector3 desiredPosition = lookTarget + forwardFromYaw(yaw) * distance + Vector3(0.0f, distance * m_cameraTilt, 0.0f);

    Vector3 newPosition = desiredPosition;
    if (!m_cameraSnapNext)
    {
        const float dt = deltaTime > 0.05f ? 0.05f : deltaTime;
        const float blend = 1.0f - std::exp(-m_cameraSmoothing * dt);
        const Vector3 current = TransformAPI::getGlobalPosition(cameraTransform);
        newPosition = current + (desiredPosition - current) * blend;
    }
    m_cameraSnapNext = false;

    TransformAPI::setGlobalPosition(cameraTransform, newPosition);
    TransformAPI::lookAt(cameraTransform, lookTarget);
}

void TrailerPlayerShowcase::updateLight()
{
    Transform* lightTransform = m_light.getReferencedComponent();
    if (lightTransform == nullptr)
    {
        return;
    }

    float yaw = m_stageYaw + m_orbitOffset;
    if (m_step != ReelStep::Model)
    {
        yaw += m_abilityCameraAngle;
    }

    const Vector3 lightPosition = characterPosition()
        + Vector3(0.0f, m_modelHeight + m_lightHeight, 0.0f)
        + forwardFromYaw(yaw) * m_lightForward;
    TransformAPI::setGlobalPosition(lightTransform, lightPosition);
}

// ---------------------------------------------------------------------------
// Comfort
// ---------------------------------------------------------------------------

void TrailerPlayerShowcase::setPlayersInvulnerable(bool invulnerable)
{
    for (GameObject* player : SceneAPI::findAllGameObjectsByTag(Tag::PLAYER))
    {
        if (Damageable* damageable = GameObjectAPI::findScript<Damageable>(player))
        {
            damageable->setInvulnerable(invulnerable);
        }
    }

    Debug::log("[TrailerPlayerShowcase] Players invulnerable: %s", invulnerable ? "ON" : "OFF");
}

void TrailerPlayerShowcase::applyInputDetach(bool detach)
{
    if (!m_ignorePlayerInput)
    {
        return;
    }

    if (detach)
    {
        // Point both players at a pad slot nobody uses, so the mouse can't aim and keys can't move them.
        PlayerGamepadBinding::setGamepad(0, kUnusedGamepadSlot);
        PlayerGamepadBinding::setGamepad(1, kUnusedGamepadSlot);
    }
    else
    {
        // The game's default: player 0 on keyboard + mouse, player 1 on the first pad.
        PlayerGamepadBinding::setKeyboard(0);
        PlayerGamepadBinding::setGamepad(1, 0);
    }
}

void TrailerPlayerShowcase::removeHealthDrops()
{
    for (GameObject* drop : SceneAPI::findAllGameObjectsWithScript<HealthPickup>())
    {
        if (drop != nullptr)
        {
            GameObjectAPI::removeGameObject(drop);
        }
    }
}

void TrailerPlayerShowcase::toggleSlowMotion()
{
    m_slowMotionActive = !m_slowMotionActive;
    Time::setTimeScale(m_slowMotionActive ? m_slowMotionScale : 1.0f);
    Debug::log("[TrailerPlayerShowcase] Slow motion %s", m_slowMotionActive ? "ON" : "OFF");
}

void TrailerPlayerShowcase::logControls() const
{
    if (m_syncMode)
    {
        Debug::log("[TrailerPlayerShowcase] SYNC MODE. Hold RIGHT SHIFT and press:");
        Debug::log("  Space  restart the paired abilities  |  J  Shadow Execution showpiece");
        Debug::log("  3  auto loop on/off                  |  Tab  trailer camera on/off");
        Debug::log("  Up / Down (hold)  zoom               |  1 / 2 (hold)  orbit");
        return;
    }

    if (m_introMode)
    {
        Debug::log("[TrailerPlayerShowcase] INTRO MODE. Hold RIGHT SHIFT and press:");
        Debug::log("  Space  restart from Death's intro    |  L  Lyriel's intro   |  J  bullet-time cross");
        Debug::log("  3  auto loop on/off                  |  Tab  trailer camera on/off");
        Debug::log("  Up / Down (hold)  zoom               |  1 / 2 (hold)  orbit");
        return;
    }

    if (m_lastStandMode)
    {
        Debug::log("[TrailerPlayerShowcase] LAST STAND MODE. Hold RIGHT SHIFT and press:");
        Debug::log("  Space  new take from the start       |  J  jump to the climax");
        Debug::log("  3  auto loop on/off                  |  Tab  trailer camera on/off");
        Debug::log("  Up / Down (hold)  zoom               |  1 / 2 (hold)  orbit   O  slow motion");
        return;
    }

    if (m_duoMode)
    {
        Debug::log("[TrailerPlayerShowcase] DUO MODE (asymmetric roles). Hold RIGHT SHIFT and press:");
        Debug::log("  Space  new take from the start       |  J  Shadow Execution finale now");
        Debug::log("  3  auto loop on/off                  |  Tab  trailer camera on/off");
        Debug::log("  Up / Down (hold)  zoom               |  1 / 2 (hold)  orbit");
        Debug::log("  O  slow motion   U  invulnerability");
        Debug::log("[TrailerPlayerShowcase] Death: walk in, attack, back off, chain pull, walk in, attack. Lyriel shoots from the flank throughout. Then Shadow Execution.");
        return;
    }

    Debug::log("[TrailerPlayerShowcase] Hold RIGHT SHIFT and press:");
    Debug::log("  Space  model shot (turntable)        |  Left / Right  switch Death / Lyriel");
    Debug::log("  J  basic combo   K  charged attack   |  L  dash   I  special (taunt / arrow volley)");
    Debug::log("  3  auto reel on/off                  |  R  restart from the model shot");
    Debug::log("  4  dummy enemy on/off (with / without a target)");
    Debug::log("  Up / Down (hold)  zoom               |  1 / 2 (hold)  orbit");
    Debug::log("  Tab  trailer camera on/off   O  slow motion   U  invulnerability");
    Debug::log("[TrailerPlayerShowcase] Auto reel is %s.", m_autoReel ? "ON (no keys needed)" : "OFF");
}

// ---------------------------------------------------------------------------
// Duo mode: asymmetric roles
// ---------------------------------------------------------------------------

ShadowExecution* TrailerPlayerShowcase::findShadowExecution()
{
    if (m_shadowExecution == nullptr)
    {
        const std::vector<GameObject*> holders = SceneAPI::findAllGameObjectsWithScript<ShadowExecution>();
        if (!holders.empty())
        {
            m_shadowExecution = GameObjectAPI::findScript<ShadowExecution>(holders[0]);
        }
    }
    return m_shadowExecution;
}

void TrailerPlayerShowcase::fillReaperGauge()
{
    const std::vector<GameObject*> holders = SceneAPI::findAllGameObjectsWithScript<ReaperGauge>();
    ReaperGauge* gauge = holders.empty() ? nullptr : GameObjectAPI::findScript<ReaperGauge>(holders[0]);
    if (gauge == nullptr)
    {
        return;
    }

    for (int i = 0; i < 200 && !gauge->isFull(); ++i)
    {
        gauge->onMarkExploited();
    }
    gauge->updateUI();
}

void TrailerPlayerShowcase::handleDuoInput()
{
    if (comboPressed(KeyCode::Space))
    {
        if (m_lastStandMode) startLastStand(); else startDuo();
    }

    if (comboPressed(KeyCode::J) && liveDuoEnemyCount() > 0 && !m_finaleStarted && m_duoRunning)
    {
        if (m_lastStandMode) enterClimax(); else beginFinale();
    }

    if (comboPressed(KeyCode::Num3))
    {
        m_autoReel = !m_autoReel;
        Debug::log("[TrailerPlayerShowcase] Auto loop %s", m_autoReel ? "ON" : "OFF");
        if (m_autoReel && !m_duoRunning)
        {
            if (m_lastStandMode) startLastStand(); else startDuo();
        }
    }
}

void TrailerPlayerShowcase::startDuo()
{
    findCharacters();
    if (m_death == nullptr || m_lyriel == nullptr)
    {
        return;
    }

    ShadowExecution* shadowExecution = findShadowExecution();
    if (shadowExecution != nullptr && shadowExecution->isActive())
    {
        return; // let the finale finish
    }

    if (m_finaleSlowApplied)
    {
        Time::setTimeScale(m_slowMotionActive ? m_slowMotionScale : 1.0f);
        m_finaleSlowApplied = false;
    }

    stopTracks();
    removeDuoEnemies();
    removeDummy();

    if (!getStage(m_stage, m_stageYaw))
    {
        return;
    }

    placeDuoCharacters();
    spawnDuoEnemies();

    // Death: the tank. Goes in, hits, backs off, drags them all onto himself, goes in again.
    m_deathTrack = DuoTrack();
    m_deathTrack.character = m_death;
    m_deathTrack.gap = m_duoBeatGap;
    m_deathTrack.moves = { DuoMove::DashToEnemies, DuoMove::WalkToEnemies, DuoMove::MarkEarlyKills, DuoMove::Basic,
                           DuoMove::WalkBack, DuoMove::Special,
                           DuoMove::MarkFinisherKills, DuoMove::WalkToEnemies, DuoMove::Basic, DuoMove::Charged };

    // Lyriel: the support. Keeps firing from the flank, repositions with her dash, drifts back to her spot.
    m_lyrielTrack = DuoTrack();
    m_lyrielTrack.character = m_lyriel;
    m_lyrielTrack.gap = m_duoLyrielGap;
    m_lyrielTrack.loop = true;
    m_lyrielTrack.moves = { DuoMove::Basic, DuoMove::Charged, DuoMove::DashToSpot, DuoMove::Basic,
                            DuoMove::Special, DuoMove::DashToHome, DuoMove::Basic, DuoMove::WalkHome };

    m_duoRunning = true;
    m_duoPhase = DuoPhase::Setup;
    m_phaseTimer = 0.0f;
    m_finaleStarted = false;
    m_finaleKills = false;
    m_orbitOffset = 0.0f;
    m_zoomScale = 1.0f;
    m_cameraSnapNext = true;
    m_characterIndex = 0; // the key light follows Death, at the front of the fight

    Debug::log("[TrailerPlayerShowcase] Duo: new take.");
}

void TrailerPlayerShowcase::placeDuoCharacters()
{
    const float yawRadians = m_stageYaw * (kPi / 180.0f);
    const Vector3 right(std::cos(yawRadians), 0.0f, -std::sin(yawRadians));
    const Vector3 towardsCamera = forwardFromYaw(m_stageYaw);

    // Death fights from the front spot but starts a dash-length behind it, so he dashes past Lyriel into the fight.
    // Lyriel stands just behind that spot and off to the flank (away from the lens), with a clear line of fire.
    m_duoFightMark = snapToNavMesh(m_stage + right * m_duoDeathFront, NavAgentProfile::PlayerNormal);
    m_duoDeathMark = snapToNavMesh(m_duoFightMark - right * m_duoDashRunUp, NavAgentProfile::PlayerNormal);
    m_duoLyrielMark = snapToNavMesh(m_duoFightMark - right * m_duoLyrielBack - towardsCamera * m_duoLyrielSide,
                                    NavAgentProfile::PlayerNormal);

    CharacterBase* characters[] = { m_death, m_lyriel };
    const Vector3 marks[] = { m_duoDeathMark, m_duoLyrielMark };
    for (int i = 0; i < 2; ++i)
    {
        Transform* transform = characters[i] ? GameObjectAPI::getTransform(characters[i]->getOwner()) : nullptr;
        if (transform != nullptr)
        {
            TransformAPI::setGlobalPosition(transform, marks[i]);
            TransformAPI::setGlobalRotationEuler(transform, Vector3(0.0f, yawFromDirection(right), 0.0f));
        }
    }
}

void TrailerPlayerShowcase::faceEnemies(CharacterBase* character)
{
    Transform* transform = character ? GameObjectAPI::getTransform(character->getOwner()) : nullptr;
    if (transform == nullptr || liveDuoEnemyCount() == 0)
    {
        return;
    }

    const Vector3 toEnemies = duoEnemyCentroid() - TransformAPI::getGlobalPosition(transform);
    if (toEnemies.x * toEnemies.x + toEnemies.z * toEnemies.z > 0.01f)
    {
        TransformAPI::setGlobalRotationEuler(transform, Vector3(0.0f, yawFromDirection(toEnemies), 0.0f));
    }
}

bool TrailerPlayerShowcase::nearestEnemy(const Vector3& from, Vector3& outPosition) const
{
    float best = 1.0e9f;
    bool found = false;
    for (const DuoEnemy& enemy : m_duoEnemies)
    {
        if (enemy.object == nullptr || !SceneAPI::containsGameObject(enemy.object))
        {
            continue;
        }

        const Damageable* damageable = GameObjectAPI::findScript<Damageable>(enemy.object);
        Transform* transform = GameObjectAPI::getTransform(enemy.object);
        if (transform == nullptr || (damageable != nullptr && damageable->isDead()))
        {
            continue;
        }

        const Vector3 position = TransformAPI::getGlobalPosition(transform);
        const Vector3 delta = position - from;
        const float distanceSq = delta.x * delta.x + delta.z * delta.z;
        if (distanceSq < best)
        {
            best = distanceSq;
            outPosition = position;
            found = true;
        }
    }
    return found;
}

void TrailerPlayerShowcase::spawnDuoEnemies()
{
    m_duoEnemies.clear();
    m_duoEnemiesSpawned = false;
    m_duoEffectsPlayed = false;

    if (!m_dummyPrefab.m_id.isValid())
    {
        Debug::warn("[TrailerPlayerShowcase] Duo mode needs the Dummy Enemy Prefab set.");
        return;
    }

    const float yawRadians = m_stageYaw * (kPi / 180.0f);
    const Vector3 right(std::cos(yawRadians), 0.0f, -std::sin(yawRadians));
    const Vector3 towardsCamera = forwardFromYaw(m_stageYaw);

    int count = m_duoEnemyCount;
    if (count < 1) count = 1;
    if (count > 10) count = 10;

    for (int i = 0; i < count; ++i)
    {
        const float t = (count == 1) ? 0.0f : (static_cast<float>(i) / static_cast<float>(count - 1)) * 2.0f - 1.0f;
        const float depth = m_duoEnemyDistance + ((i % 2) == 0 ? 0.0f : 0.5f);
        DuoEnemy enemy;
        enemy.mark = snapToNavMesh(m_duoFightMark + right * depth + towardsCamera * (t * m_duoEnemySpread), kEnemyNavProfile);
        enemy.secondType = m_duoEnemyPrefab2.m_id.isValid() && (i % 2) == 1;
        m_duoEnemies.push_back(enemy);
    }
}

void TrailerPlayerShowcase::updateDuoEnemies(float deltaTime)
{
    // Arrive with the summon effect, then appear a beat later.
    const float spawnDelay = m_spawnEffectPrefab.m_id.isValid() ? kDummySpawnDelay : 0.0f;
    bool anyPending = false;

    for (size_t i = m_duoEnemies.size(); i-- > 0;)
    {
        DuoEnemy& enemy = m_duoEnemies[i];

        if (enemy.pending)
        {
            if (!enemy.effectPlayed)
            {
                enemy.effectPlayed = true;
                if (m_spawnEffectPrefab.m_id.isValid())
                {
                    ParticleLifecycle::spawnOneShotTimed(m_effectTracker, m_spawnEffectPrefab.m_id, enemy.mark, Vector3::Zero, 3.0f);
                }
            }

            enemy.spawnTimer += deltaTime;
            if (enemy.spawnTimer < spawnDelay)
            {
                anyPending = true;
                continue;
            }

            enemy.pending = false;
            const AssetId& prefab = enemy.secondType ? m_duoEnemyPrefab2.m_id : m_dummyPrefab.m_id;
            const Vector3 faceTowards = m_lastStandMode ? duoCentre() : m_duoFightMark;
            const float yaw = yawFromDirection(faceTowards - enemy.mark);
            enemy.object = GameObjectAPI::instantiatePrefab(prefab, enemy.mark, Vector3(0.0f, yaw, 0.0f));
            if (enemy.object == nullptr)
            {
                m_duoEnemies.erase(m_duoEnemies.begin() + static_cast<std::ptrdiff_t>(i));
            }
            continue;
        }

        if (enemy.object == nullptr)
        {
            continue;
        }

        ++enemy.frames;
        if (!SceneAPI::containsGameObject(enemy.object))
        {
            if (enemy.seen || enemy.frames > 10)
            {
                m_duoEnemies.erase(m_duoEnemies.begin() + static_cast<std::ptrdiff_t>(i));
            }
            continue;
        }
        enemy.seen = true;

        Damageable* damageable = GameObjectAPI::findScript<Damageable>(enemy.object);
        if (damageable == nullptr || damageable->isDead() || enemy.frames < 3)
        {
            continue;
        }

        if (!enemy.prepared)
        {
            enemy.prepared = true;
            if (m_lastStandMode)
            {
                // The horde charges in for real. The ones picked to fall early start already hurt.
                if (enemy.mortal && damageable->getCurrentHp() > damageable->getMaxHp() * 0.5f)
                {
                    damageable->takeDamage(damageable->getCurrentHp() - damageable->getMaxHp() * 0.5f);
                }
            }
            else if (EnemyBaseController* controller = GameObjectAPI::findScript<EnemyBaseController>(enemy.object))
            {
                // They hold their ground (attacks and turning still play); only Death's taunt moves them.
                controller->m_moveSpeed = 0.0f;
            }
        }

        // Hits land but nobody dies before the finale.
        if (!m_finaleKills && !enemy.mortal && damageable->getCurrentHp() < damageable->getMaxHp() * 0.6f)
        {
            damageable->heal(damageable->getMaxHp());
        }
    }

    if (!anyPending && !m_duoEnemies.empty())
    {
        m_duoEnemiesSpawned = true;
    }
}

void TrailerPlayerShowcase::removeDuoEnemies()
{
    for (DuoEnemy& enemy : m_duoEnemies)
    {
        if (enemy.object != nullptr && SceneAPI::containsGameObject(enemy.object))
        {
            GameObjectAPI::removeGameObject(enemy.object);
        }
    }
    m_duoEnemies.clear();

    // Anything else left over from the last take (corpses, summons).
    for (GameObject* object : SceneAPI::findAllGameObjectsByTag(Tag::ENEMY))
    {
        if (GameObjectAPI::findScript<EnemyBaseController>(object) != nullptr)
        {
            GameObjectAPI::removeGameObject(object);
        }
    }
}

Vector3 TrailerPlayerShowcase::duoEnemyCentroid() const
{
    Vector3 sum = Vector3::Zero;
    int count = 0;
    for (const DuoEnemy& enemy : m_duoEnemies)
    {
        Vector3 position = enemy.mark;
        if (enemy.object != nullptr && SceneAPI::containsGameObject(enemy.object))
        {
            if (Transform* transform = GameObjectAPI::getTransform(enemy.object))
            {
                position = TransformAPI::getGlobalPosition(transform);
            }
        }
        sum += position;
        ++count;
    }

    return count > 0 ? sum / static_cast<float>(count) : m_duoFightMark;
}

int TrailerPlayerShowcase::liveDuoEnemyCount() const
{
    int count = 0;
    for (const DuoEnemy& enemy : m_duoEnemies)
    {
        if (enemy.object == nullptr || !SceneAPI::containsGameObject(enemy.object))
        {
            continue;
        }

        const Damageable* damageable = GameObjectAPI::findScript<Damageable>(enemy.object);
        if (damageable == nullptr || !damageable->isDead())
        {
            ++count;
        }
    }
    return count;
}

AbilityBase* TrailerPlayerShowcase::abilityOf(CharacterBase* character, ReelStep step) const
{
    if (character == nullptr)
    {
        return nullptr;
    }

    switch (step)
    {
    case ReelStep::Basic:   return character->getBasicAttack();
    case ReelStep::Charged: return character->getChargedAttack();
    case ReelStep::Special: return character->getSpecialAbility();
    case ReelStep::Dash:    return GameObjectAPI::findScript<AbilityDash>(character->getOwner());
    default:                return nullptr;
    }
}

void TrailerPlayerShowcase::releaseSpecialOf(CharacterBase* character)
{
    if (character == nullptr)
    {
        return;
    }

    if (DeathTaunt* taunt = GameObjectAPI::findScript<DeathTaunt>(character->getOwner()))
    {
        taunt->releaseFromScript();
    }
    else if (LyrielArrowVolley* volley = GameObjectAPI::findScript<LyrielArrowVolley>(character->getOwner()))
    {
        volley->releaseFromScript();
    }
}

// --- Routines ---------------------------------------------------------------

void TrailerPlayerShowcase::setWalkDirection(CharacterBase* character, const Vector3& direction)
{
    if (character == nullptr)
    {
        return;
    }

    if (PlayerController* controller = character->getPlayerController())
    {
        // Goes through the normal movement code: run animation, speed, turning and navmesh all as in play.
        controller->setScriptedMoveDirection(direction);
    }
}

void TrailerPlayerShowcase::stopTracks()
{
    setWalkDirection(m_death, Vector3::Zero);
    setWalkDirection(m_lyriel, Vector3::Zero);
    m_deathTrack.finished = true;
    m_deathTrack.walking = false;
    m_lyrielTrack.finished = true;
    m_lyrielTrack.walking = false;
}

bool TrailerPlayerShowcase::walkTargetFor(const DuoTrack& track, DuoMove move, Vector3& outTarget) const
{
    Transform* transform = track.character ? GameObjectAPI::getTransform(track.character->getOwner()) : nullptr;
    if (transform == nullptr)
    {
        return false;
    }
    const Vector3 position = TransformAPI::getGlobalPosition(transform);
    const float stageYawRadians = m_stageYaw * (kPi / 180.0f);
    const Vector3 stageRight(std::cos(stageYawRadians), 0.0f, -std::sin(stageYawRadians));

    switch (move)
    {
    case DuoMove::WalkToEnemies:
    {
        // Up to the nearest enemy, always from his own side (towards Lyriel), stopping at sword's reach.
        Vector3 enemy;
        if (!nearestEnemy(position, enemy))
        {
            return false;
        }
        if (m_lastStandMode)
        {
            // Surrounded: go straight at whoever is nearest, from wherever he is.
            Vector3 away = position - enemy;
            away.y = 0.0f;
            const float length = std::sqrt(away.x * away.x + away.z * away.z);
            if (length < 0.001f)
            {
                return false;
            }
            outTarget = enemy + away * (m_duoMeleeDistance / length);
            return true;
        }
        outTarget = enemy - stageRight * m_duoMeleeDistance;
        return true;
    }

    case DuoMove::WalkBack:
    {
        // Back out on his own side so the whole group is in front of him, inside the chain's reach,
        // and the chains pull them away from the camera's side rather than across it.
        if (liveDuoEnemyCount() == 0)
        {
            return false;
        }
        outTarget = duoEnemyCentroid() - stageRight * m_duoTauntDistance;
        return true;
    }

    case DuoMove::WalkHome:
        outTarget = (track.character == m_lyriel) ? m_duoLyrielMark : m_duoFightMark;
        return true;

    default:
        return false;
    }
}

void TrailerPlayerShowcase::facePoint(CharacterBase* character, const Vector3& point)
{
    Transform* transform = character ? GameObjectAPI::getTransform(character->getOwner()) : nullptr;
    if (transform == nullptr)
    {
        return;
    }

    const Vector3 direction = point - TransformAPI::getGlobalPosition(transform);
    if (direction.x * direction.x + direction.z * direction.z > 0.01f)
    {
        TransformAPI::setGlobalRotationEuler(transform, Vector3(0.0f, yawFromDirection(direction), 0.0f));
    }
}

void TrailerPlayerShowcase::markMortal(int count)
{
    // The enemies nearest Death get their turn: no more healing, and low enough that his hits finish them.
    Transform* deathTransform = m_death ? GameObjectAPI::getTransform(m_death->getOwner()) : nullptr;
    const Vector3 deathPosition = deathTransform ? TransformAPI::getGlobalPosition(deathTransform) : m_duoDeathMark;

    for (int picked = 0; picked < count; ++picked)
    {
        DuoEnemy* best = nullptr;
        float bestDistance = 1.0e9f;
        for (DuoEnemy& enemy : m_duoEnemies)
        {
            if (enemy.mortal || enemy.object == nullptr || !SceneAPI::containsGameObject(enemy.object))
            {
                continue;
            }
            const Damageable* damageable = GameObjectAPI::findScript<Damageable>(enemy.object);
            Transform* transform = GameObjectAPI::getTransform(enemy.object);
            if (transform == nullptr || (damageable != nullptr && damageable->isDead()))
            {
                continue;
            }
            const Vector3 delta = TransformAPI::getGlobalPosition(transform) - deathPosition;
            const float distance = delta.x * delta.x + delta.z * delta.z;
            if (distance < bestDistance)
            {
                bestDistance = distance;
                best = &enemy;
            }
        }

        if (best == nullptr)
        {
            return;
        }

        best->mortal = true;
        if (Damageable* damageable = GameObjectAPI::findScript<Damageable>(best->object))
        {
            const float targetHp = damageable->getMaxHp() * m_duoMortalHealth;
            if (damageable->getCurrentHp() > targetHp)
            {
                damageable->takeDamage(damageable->getCurrentHp() - targetHp);
            }
        }
    }
}

bool TrailerPlayerShowcase::startAction(CharacterBase* character, ReelStep step, int presses, ActorAction& outAction, const Vector3* facePointOverride)
{
    AbilityBase* ability = abilityOf(character, step);
    if (ability == nullptr)
    {
        return false;
    }

    // Dashes go the way the character faces, so they turn to their destination; everything else turns to the fight.
    if (facePointOverride != nullptr)
    {
        facePoint(character, *facePointOverride);
    }
    else
    {
        faceEnemies(character);
    }

    outAction = ActorAction();
    outAction.character = character;
    outAction.step = step;
    outAction.pressesLeft = presses < 1 ? 1 : presses;
    outAction.baseline = ability->getSuccessfulUse();

    if (outAction.pressesLeft == 1)
    {
        // Single press: a second tryAbility could cancel and restart a charge or aim.
        ability->tryAbility();
        outAction.pressesLeft = 0;
    }
    return true;
}

void TrailerPlayerShowcase::updateAction(ActorAction& action, float deltaTime)
{
    if (action.done)
    {
        return;
    }

    action.timer += deltaTime;
    AbilityBase* ability = abilityOf(action.character, action.step);

    // Repeated presses (basic combos and volleys of shots): mash until each one actually starts.
    if (action.pressesLeft > 0 && ability != nullptr)
    {
        if (action.pressCooldown > 0.0f)
        {
            action.pressCooldown -= deltaTime;
        }
        else
        {
            ability->tryAbility();
            if (ability->getSuccessfulUse() > action.baseline)
            {
                action.baseline = ability->getSuccessfulUse();
                --action.pressesLeft;
                action.pressCooldown = 0.15f;
                action.timer = 0.0f;
            }
            else if (action.timer >= kPressTimeout)
            {
                action.pressesLeft = 0;
            }
        }
    }

    if (action.step == ReelStep::Special && !action.released && action.timer >= m_specialAimTime)
    {
        action.released = true;
        releaseSpecialOf(action.character);
    }

    const bool busy = action.character != nullptr &&
        (action.character->isUsingAbility() || action.character->getActiveAbility() != nullptr);
    if ((action.pressesLeft <= 0 && action.timer >= kMinRunTime && !busy) || action.timer >= kRunTimeout)
    {
        action.done = true;
    }
}

void TrailerPlayerShowcase::advanceTrack(DuoTrack& track)
{
    ++track.index;
    track.gapTimer = track.gap;
    track.acting = false;
    track.walking = false;
}

void TrailerPlayerShowcase::startTrackMove(DuoTrack& track)
{
    if (track.index >= track.moves.size())
    {
        if (track.loop)
        {
            track.index = 0;
        }
        else
        {
            track.finished = true;
            return;
        }
    }

    const DuoMove move = track.moves[track.index];
    switch (move)
    {
    case DuoMove::WalkToEnemies:
    case DuoMove::WalkBack:
    case DuoMove::WalkHome:
    {
        Vector3 target;
        Transform* transform = track.character ? GameObjectAPI::getTransform(track.character->getOwner()) : nullptr;
        if (transform == nullptr || !walkTargetFor(track, move, target))
        {
            advanceTrack(track);
            return;
        }

        const Vector3 delta = target - TransformAPI::getGlobalPosition(transform);
        const float minimum = (move == DuoMove::WalkHome) ? 2.5f : 0.4f; // Lyriel only heads back if she's drifted
        if (delta.x * delta.x + delta.z * delta.z < minimum * minimum)
        {
            track.index++;
            return; // already there: no pause, go straight to the next move
        }

        track.walking = true;
        track.walkTarget = target;
        track.walkTimer = 0.0f;
        return;
    }

    case DuoMove::MarkEarlyKills:
    case DuoMove::MarkFinisherKills:
    {
        int count = m_duoEarlyKills;
        if (move == DuoMove::MarkFinisherKills)
        {
            int unmarked = 0;
            for (const DuoEnemy& enemy : m_duoEnemies)
            {
                if (!enemy.mortal && enemy.object != nullptr && SceneAPI::containsGameObject(enemy.object))
                {
                    ++unmarked;
                }
            }
            count = unmarked - (m_duoFinaleSurvivors < 0 ? 0 : m_duoFinaleSurvivors);
        }
        if (count > 0)
        {
            markMortal(count);
        }
        track.index++; // instant: no pause
        return;
    }

    case DuoMove::DashToEnemies:
    {
        Transform* transform = track.character ? GameObjectAPI::getTransform(track.character->getOwner()) : nullptr;
        Vector3 enemy;
        if (transform == nullptr || !nearestEnemy(TransformAPI::getGlobalPosition(transform), enemy))
        {
            track.index++;
            return;
        }
        const Vector3 delta = enemy - TransformAPI::getGlobalPosition(transform);
        if (delta.x * delta.x + delta.z * delta.z < m_duoDashMinDistance * m_duoDashMinDistance)
        {
            track.index++; // close enough to just walk
            return;
        }
        if (startAction(track.character, ReelStep::Dash, 1, track.action, &enemy))
        {
            track.acting = true;
        }
        else
        {
            advanceTrack(track);
        }
        return;
    }

    case DuoMove::DashAway:
    {
        // Kiting: leap away from whoever got closest.
        Transform* transform = track.character ? GameObjectAPI::getTransform(track.character->getOwner()) : nullptr;
        Vector3 enemy;
        if (transform == nullptr || !nearestEnemy(TransformAPI::getGlobalPosition(transform), enemy))
        {
            track.index++;
            return;
        }
        const Vector3 position = TransformAPI::getGlobalPosition(transform);
        Vector3 away = position - enemy;
        away.y = 0.0f;
        const float length = std::sqrt(away.x * away.x + away.z * away.z);
        if (length < 0.001f || length > 4.0f)
        {
            track.index++; // nobody close enough to need it
            return;
        }
        const Vector3 landing = position + away * (3.0f / length);
        if (startAction(track.character, ReelStep::Dash, 1, track.action, &landing))
        {
            track.acting = true;
        }
        else
        {
            advanceTrack(track);
        }
        return;
    }

    case DuoMove::DashToSpot:
    case DuoMove::DashToHome:
    {
        // Lyriel kites between two spots on her flank: her own, and one further back along the line.
        const float yawRadians = m_stageYaw * (kPi / 180.0f);
        const Vector3 right(std::cos(yawRadians), 0.0f, -std::sin(yawRadians));
        const Vector3 spot = (move == DuoMove::DashToHome) ? m_duoLyrielMark : (m_duoLyrielMark - right * m_duoLyrielDashSpot);

        Transform* transform = track.character ? GameObjectAPI::getTransform(track.character->getOwner()) : nullptr;
        if (transform == nullptr)
        {
            track.index++;
            return;
        }
        const Vector3 delta = spot - TransformAPI::getGlobalPosition(transform);
        if (delta.x * delta.x + delta.z * delta.z < 1.5f * 1.5f)
        {
            track.index++;
            return;
        }
        if (startAction(track.character, ReelStep::Dash, 1, track.action, &spot))
        {
            track.acting = true;
        }
        else
        {
            advanceTrack(track);
        }
        return;
    }

    case DuoMove::Basic:
    case DuoMove::Charged:
    case DuoMove::Special:
    {
        const ReelStep step = move == DuoMove::Basic ? ReelStep::Basic
                            : move == DuoMove::Charged ? ReelStep::Charged
                            : ReelStep::Special;
        const int presses = (move == DuoMove::Basic) ? m_basicComboPresses : 1;
        if (startAction(track.character, step, presses, track.action))
        {
            track.acting = true;
        }
        else
        {
            advanceTrack(track);
        }
        return;
    }
    }
}

void TrailerPlayerShowcase::updateTrack(DuoTrack& track, float deltaTime)
{
    if (track.finished || track.character == nullptr)
    {
        return;
    }

    if (track.walking)
    {
        Transform* transform = GameObjectAPI::getTransform(track.character->getOwner());
        if (transform == nullptr)
        {
            setWalkDirection(track.character, Vector3::Zero);
            advanceTrack(track);
            return;
        }

        // Walking at enemies: keep re-aiming at the nearest one, they shift about.
        const DuoMove move = track.moves[track.index];
        if (move == DuoMove::WalkToEnemies)
        {
            Vector3 updated;
            if (walkTargetFor(track, move, updated))
            {
                track.walkTarget = updated;
            }
        }

        track.walkTimer += deltaTime;
        Vector3 delta = track.walkTarget - TransformAPI::getGlobalPosition(transform);
        delta.y = 0.0f;
        const float distance = std::sqrt(delta.x * delta.x + delta.z * delta.z);

        if (distance < 0.3f || track.walkTimer > 5.0f)
        {
            setWalkDirection(track.character, Vector3::Zero);
            advanceTrack(track);
            return;
        }

        setWalkDirection(track.character, delta * (1.0f / distance));
        return;
    }

    if (track.acting)
    {
        updateAction(track.action, deltaTime);
        if (track.action.done)
        {
            advanceTrack(track);
        }
        return;
    }

    if (track.gapTimer > 0.0f)
    {
        track.gapTimer -= deltaTime;
        return;
    }

    startTrackMove(track);
}

// --- Flow -------------------------------------------------------------------

void TrailerPlayerShowcase::beginFinale()
{
    stopTracks();
    m_duoPhase = DuoPhase::Finale;
    m_phaseTimer = 0.0f;
    startFinale();
    Debug::log("[TrailerPlayerShowcase] Duo: Shadow Execution");
}

void TrailerPlayerShowcase::startFinale()
{
    m_finaleStarted = true;
    m_finaleKills = true;

    ShadowExecution* shadowExecution = findShadowExecution();
    if (shadowExecution == nullptr)
    {
        Debug::warn("[TrailerPlayerShowcase] No ShadowExecution script in this scene (it lives on GameManager).");
        return;
    }

    // Everyone under their execution threshold, so the move finishes the fight.
    for (const DuoEnemy& enemy : m_duoEnemies)
    {
        if (enemy.object == nullptr || !SceneAPI::containsGameObject(enemy.object))
        {
            continue;
        }

        EnemyDamageable* damageable = GameObjectAPI::findScript<EnemyDamageable>(enemy.object);
        if (damageable == nullptr || damageable->isDead() || damageable->getMaxHp() <= 0.0f)
        {
            continue;
        }

        const float threshold = shadowExecution->getExecutionThresholdPercent(damageable);
        const float targetHp = damageable->getMaxHp() * threshold * 0.75f;
        if (threshold > 0.0f && damageable->getCurrentHp() > targetHp)
        {
            damageable->takeDamage(damageable->getCurrentHp() - targetHp);
        }
    }

    faceEnemies(m_death);
    faceEnemies(m_lyriel);
    fillReaperGauge();

    if (!shadowExecution->triggerNow())
    {
        Debug::warn("[TrailerPlayerShowcase] Shadow Execution didn't start.");
        return;
    }

    if (m_duoFinaleSlowMotion < 0.999f)
    {
        Time::setTimeScale(m_duoFinaleSlowMotion);
        m_finaleSlowApplied = true;
    }
}

void TrailerPlayerShowcase::updateDuo(float deltaTime)
{
    if (!m_duoRunning)
    {
        return;
    }

    m_phaseTimer += deltaTime;

    switch (m_duoPhase)
    {
    case DuoPhase::Setup:
        // Enemies in, a moment to read the line-up, then both get to work.
        if (m_duoEnemiesSpawned && m_phaseTimer >= 2.0f)
        {
            m_duoPhase = DuoPhase::Fight;
            m_phaseTimer = 0.0f;
            Debug::log("[TrailerPlayerShowcase] Duo: fight (Death tanks up front, Lyriel covers from the flank).");
        }
        break;

    case DuoPhase::Fight:
        updateTrack(m_deathTrack, deltaTime);
        updateTrack(m_lyrielTrack, deltaTime);

        // Death's routine sets the pace; once it's done and Lyriel isn't mid-shot, go to the finale.
        if (m_deathTrack.finished)
        {
            const bool lyrielBusy = m_lyriel != nullptr && m_lyrielTrack.acting && !m_lyrielTrack.action.done;
            if (!lyrielBusy || m_phaseTimer > 60.0f)
            {
                beginFinale();
            }
        }
        else if (m_phaseTimer > 60.0f)
        {
            beginFinale(); // safety net if something gets stuck
        }
        break;

    case DuoPhase::Finale:
    {
        ShadowExecution* shadowExecution = findShadowExecution();
        const bool playing = shadowExecution != nullptr && shadowExecution->isActive();
        if (!playing && m_phaseTimer >= 0.3f)
        {
            m_duoPhase = DuoPhase::Aftermath;
            m_phaseTimer = 0.0f;
        }
        break;
    }

    case DuoPhase::Aftermath:
        if (m_finaleSlowApplied && m_phaseTimer >= 0.6f)
        {
            Time::setTimeScale(m_slowMotionActive ? m_slowMotionScale : 1.0f);
            m_finaleSlowApplied = false;
        }

        if (m_phaseTimer >= m_duoLoopDelay)
        {
            if (m_autoReel)
            {
                startDuo();
            }
            else
            {
                m_duoRunning = false;
            }
        }
        break;
    }
}

void TrailerPlayerShowcase::updateDuoCamera(float deltaTime)
{
    updateLight();

    if (!m_cameraActive || !m_startDone)
    {
        return;
    }

    GameObject* camera = SceneAPI::getDefaultCameraGameObject();
    Transform* cameraTransform = camera ? GameObjectAPI::getTransform(camera) : nullptr;
    if (cameraTransform == nullptr)
    {
        return;
    }

    float horizontalFov = m_fov;
    if (horizontalFov <= 0.0f)
    {
        if (CameraComponent* cameraComponent = CameraAPI::getCameraComponent(camera))
        {
            horizontalFov = CameraAPI::getFov(cameraComponent);
        }
    }
    if (horizontalFov <= 1.0f || horizontalFov > 170.0f)
    {
        horizontalFov = 48.0f;
    }
    const float tanHalfH = std::tan(horizontalFov * 0.5f * (kPi / 180.0f));
    const float tanHalfV = tanHalfH / (16.0f / 9.0f);

    // Everyone in frame: both characters wherever they've moved to, and the enemies.
    std::vector<Vector3> points;
    CharacterBase* characters[] = { m_death, m_lyriel };
    for (CharacterBase* character : characters)
    {
        if (Transform* transform = character ? GameObjectAPI::getTransform(character->getOwner()) : nullptr)
        {
            points.push_back(TransformAPI::getGlobalPosition(transform));
        }
    }
    for (const DuoEnemy& enemy : m_duoEnemies)
    {
        Vector3 position = enemy.mark;
        if (enemy.object != nullptr && SceneAPI::containsGameObject(enemy.object))
        {
            if (Transform* transform = GameObjectAPI::getTransform(enemy.object))
            {
                position = TransformAPI::getGlobalPosition(transform);
            }
        }
        points.push_back(position);
    }
    if (points.empty())
    {
        return;
    }

    const float yaw = m_stageYaw + m_duoCameraAngle + m_orbitOffset;
    const float yawRadians = yaw * (kPi / 180.0f);
    const Vector3 right(std::cos(yawRadians), 0.0f, -std::sin(yawRadians));
    const Vector3 forward = forwardFromYaw(yaw);

    float minRight = 1.0e9f, maxRight = -1.0e9f, minForward = 1.0e9f, maxForward = -1.0e9f, sumY = 0.0f;
    for (const Vector3& point : points)
    {
        const float r = point.x * right.x + point.z * right.z;
        const float f = point.x * forward.x + point.z * forward.z;
        if (r < minRight) minRight = r;
        if (r > maxRight) maxRight = r;
        if (f < minForward) minForward = f;
        if (f > maxForward) maxForward = f;
        sumY += point.y;
    }
    const float count = static_cast<float>(points.size());
    const Vector3 focus = right * (0.5f * (minRight + maxRight)) + forward * (0.5f * (minForward + maxForward))
                        + Vector3(0.0f, sumY / count, 0.0f);

    const float halfWidth = (0.5f * (maxRight - minRight) + 1.2f) * m_duoPadding;
    const float halfHeight = 0.5f * m_modelHeight * m_duoPadding;
    // Depth spread (Lyriel on the far flank) needs a little extra distance too.
    float distance = (halfHeight / tanHalfV > halfWidth / tanHalfH ? halfHeight / tanHalfV : halfWidth / tanHalfH)
                   + 0.5f + 0.5f * (maxForward - minForward);

    // Push in a little for the finale.
    if (m_duoPhase == DuoPhase::Finale || m_duoPhase == DuoPhase::Aftermath)
    {
        float push = m_duoPhase == DuoPhase::Aftermath ? 1.0f : m_phaseTimer / 1.5f;
        if (push > 1.0f) push = 1.0f;
        const float eased = push * push * (3.0f - 2.0f * push);
        distance *= 1.0f - 0.2f * eased;
    }
    distance *= m_zoomScale;

    const Vector3 lookTarget = focus + Vector3(0.0f, m_abilityLookHeight, 0.0f);
    const Vector3 desiredPosition = lookTarget + forward * distance + Vector3(0.0f, distance * m_cameraTilt, 0.0f);

    Vector3 newPosition = desiredPosition;
    if (!m_cameraSnapNext)
    {
        const float dt = deltaTime > 0.05f ? 0.05f : deltaTime;
        const float blend = 1.0f - std::exp(-m_cameraSmoothing * dt);
        const Vector3 current = TransformAPI::getGlobalPosition(cameraTransform);
        newPosition = current + (desiredPosition - current) * blend;
    }
    m_cameraSnapNext = false;

    TransformAPI::setGlobalPosition(cameraTransform, newPosition);
    TransformAPI::lookAt(cameraTransform, lookTarget);
}

// ---------------------------------------------------------------------------
// Last Stand: back to back against the horde, then Shadow Execution wipes it
// ---------------------------------------------------------------------------

float TrailerPlayerShowcase::realDeltaTime(float deltaTime) const
{
    // Camera moves and climax timing run in real time, so they keep their pace while the world is in slow motion.
    const float timeScale = Time::getTimeScale();
    return timeScale > 0.01f ? deltaTime / timeScale : deltaTime;
}

Vector3 TrailerPlayerShowcase::duoCentre() const
{
    Vector3 sum = Vector3::Zero;
    int count = 0;
    CharacterBase* characters[] = { m_death, m_lyriel };
    for (CharacterBase* character : characters)
    {
        if (Transform* transform = character ? GameObjectAPI::getTransform(character->getOwner()) : nullptr)
        {
            sum += TransformAPI::getGlobalPosition(transform);
            ++count;
        }
    }
    return count > 0 ? sum / static_cast<float>(count) : m_stage;
}

void TrailerPlayerShowcase::startLastStand()
{
    findCharacters();
    if (m_death == nullptr || m_lyriel == nullptr)
    {
        return;
    }

    ShadowExecution* shadowExecution = findShadowExecution();
    if (shadowExecution != nullptr && shadowExecution->isActive())
    {
        return; // let the execution finish
    }

    Time::setTimeScale(m_slowMotionActive ? m_slowMotionScale : 1.0f);
    m_finaleSlowApplied = false;

    stopTracks();
    removeDuoEnemies();
    removeDummy();

    if (!getStage(m_stage, m_stageYaw))
    {
        return;
    }

    // Back to back in the middle of the arena.
    const float yawRadians = m_stageYaw * (kPi / 180.0f);
    const Vector3 right(std::cos(yawRadians), 0.0f, -std::sin(yawRadians));
    m_duoFightMark = snapToNavMesh(m_stage + right * 0.7f, NavAgentProfile::PlayerNormal);
    m_duoLyrielMark = snapToNavMesh(m_stage - right * 0.7f, NavAgentProfile::PlayerNormal);

    if (Transform* transform = GameObjectAPI::getTransform(m_death->getOwner()))
    {
        TransformAPI::setGlobalPosition(transform, m_duoFightMark);
        TransformAPI::setGlobalRotationEuler(transform, Vector3(0.0f, yawFromDirection(right), 0.0f));
    }
    if (Transform* transform = GameObjectAPI::getTransform(m_lyriel->getOwner()))
    {
        TransformAPI::setGlobalPosition(transform, m_duoLyrielMark);
        TransformAPI::setGlobalRotationEuler(transform, Vector3(0.0f, yawFromDirection(right * -1.0f), 0.0f));
    }

    // Death wades in: dash at them, cut, chain-pull a group, cut, slam, back to the middle, repeat.
    m_deathTrack = DuoTrack();
    m_deathTrack.character = m_death;
    m_deathTrack.gap = m_duoBeatGap;
    m_deathTrack.loop = true;
    m_deathTrack.moves = { DuoMove::DashToEnemies, DuoMove::WalkToEnemies, DuoMove::Basic, DuoMove::Special,
                           DuoMove::WalkToEnemies, DuoMove::Basic, DuoMove::Charged,
                           DuoMove::WalkToEnemies, DuoMove::Basic, DuoMove::WalkHome };

    // Lyriel holds the middle: shoots, leaps away from anyone who gets close, volleys, drifts back.
    m_lyrielTrack = DuoTrack();
    m_lyrielTrack.character = m_lyriel;
    m_lyrielTrack.gap = m_duoLyrielGap;
    m_lyrielTrack.loop = true;
    m_lyrielTrack.moves = { DuoMove::Basic, DuoMove::DashAway, DuoMove::Basic, DuoMove::Charged,
                            DuoMove::DashAway, DuoMove::Special, DuoMove::Basic, DuoMove::WalkHome };

    m_duoEnemies.clear();
    m_duoRunning = true;
    m_finaleStarted = false;
    m_finaleKills = false;
    m_lsPhase = LastStandPhase::Intro;
    m_lsTimer = 0.0f;
    m_lsWaveTimer = 0.0f;
    m_lsWavesSpawned = 0;
    m_lsOrbitYaw = 0.0f;
    m_lsExecutionStarted = false;
    m_lsCameraCentre = m_stage;
    m_orbitOffset = 0.0f;
    m_zoomScale = 1.0f;
    m_cameraSnapNext = true;
    m_characterIndex = 0;

    Debug::log("[TrailerPlayerShowcase] Last Stand: new take.");
}

void TrailerPlayerShowcase::spawnWave()
{
    if (!m_dummyPrefab.m_id.isValid())
    {
        Debug::warn("[TrailerPlayerShowcase] Last Stand needs the Dummy Enemy Prefab set.");
        return;
    }

    int count = m_lsWaveSize;
    if (count < 1) count = 1;
    if (count > 10) count = 10;

    const Vector3 centre = duoCentre();
    const float baseAngle = static_cast<float>(std::rand() % 360) * (kPi / 180.0f);
    const bool finalWave = (m_lsWavesSpawned >= m_lsWaves - 1);

    for (int i = 0; i < count; ++i)
    {
        // Evenly round the pair, alternating depth so it reads as a crowd closing in.
        const float angle = baseAngle + 2.0f * kPi * static_cast<float>(i) / static_cast<float>(count);
        const float radius = m_lsRingRadius + ((i % 2) == 0 ? 0.0f : 0.8f);
        DuoEnemy enemy;
        enemy.mark = snapToNavMesh(centre + Vector3(std::sin(angle) * radius, 0.0f, std::cos(angle) * radius), kEnemyNavProfile);
        enemy.secondType = m_duoEnemyPrefab2.m_id.isValid() && (i % 2) == 1;
        // Half of every wave but the last can fall during the fight; the rest are saved for the execution.
        enemy.mortal = !finalWave && (i % 2) == 0;
        m_duoEnemies.push_back(enemy);
    }

    ++m_lsWavesSpawned;
    Debug::log("[TrailerPlayerShowcase] Last Stand: wave %d of %d.", m_lsWavesSpawned, m_lsWaves);
}

void TrailerPlayerShowcase::enterClimax()
{
    if (m_lsPhase == LastStandPhase::Climax || m_lsPhase == LastStandPhase::Reveal)
    {
        return;
    }

    stopTracks();
    m_lsPhase = LastStandPhase::Climax;
    m_lsTimer = 0.0f;
    m_lsExecutionStarted = false;
    m_finaleKills = true;

    faceEnemies(m_death);
    faceEnemies(m_lyriel);
    fillReaperGauge(); // the gauge's colour grade washes in as time slows

    // Clean frame for the slow motion: no floating health bars over the crowd.
    if (m_lsHideHealthBars)
    {
        for (const DuoEnemy& enemy : m_duoEnemies)
        {
            if (enemy.object == nullptr || !SceneAPI::containsGameObject(enemy.object))
            {
                continue;
            }
            Transform* root = GameObjectAPI::getTransform(enemy.object);
            Transform* healthBar = root ? TransformAPI::findChildByName(root, "Health Bar") : nullptr;
            if (healthBar != nullptr)
            {
                GameObjectAPI::setActive(healthBar->getOwner(), false);
            }
        }
    }

    Debug::log("[TrailerPlayerShowcase] Last Stand: climax.");
}

void TrailerPlayerShowcase::updateLastStand(float deltaTime)
{
    if (!m_duoRunning)
    {
        return;
    }

    const float realDelta = realDeltaTime(deltaTime);
    m_lsTimer += realDelta;

    switch (m_lsPhase)
    {
    case LastStandPhase::Intro:
        // A beat of the two of them alone, then the first wave.
        if (m_lsTimer >= 1.5f)
        {
            spawnWave();
            m_lsPhase = LastStandPhase::Fight;
            m_lsTimer = 0.0f;
            m_lsWaveTimer = 0.0f;
        }
        break;

    case LastStandPhase::Fight:
    {
        updateTrack(m_deathTrack, deltaTime);
        updateTrack(m_lyrielTrack, deltaTime);

        m_lsWaveTimer += deltaTime;

        bool anyPending = false;
        for (const DuoEnemy& enemy : m_duoEnemies)
        {
            anyPending = anyPending || enemy.pending;
        }

        if (m_lsWavesSpawned < m_lsWaves)
        {
            // Next wave on the timer, or straight away if they've cleared the floor.
            if (m_lsWaveTimer >= m_lsWaveInterval || (!anyPending && liveDuoEnemyCount() == 0))
            {
                spawnWave();
                m_lsWaveTimer = 0.0f;
            }
        }
        else if (m_lsWaveTimer >= m_lsClimaxDelay && !anyPending)
        {
            if (liveDuoEnemyCount() == 0)
            {
                // Nothing left for the execution: one more wave first.
                m_lsWaves = m_lsWavesSpawned + 1;
                spawnWave();
                m_lsWaveTimer = 0.0f;
            }
            else
            {
                enterClimax();
            }
        }
        break;
    }

    case LastStandPhase::Climax:
    {
        // Time drains away while the camera pushes in on the pair...
        float k = m_lsTimer / m_lsClimaxRamp;
        if (k > 1.0f) k = 1.0f;
        const float eased = k * k * (3.0f - 2.0f * k);

        if (!m_lsExecutionStarted)
        {
            Time::setTimeScale(1.0f + (m_lsClimaxSlowMotion - 1.0f) * eased);

            // ...then Shadow Execution, played out at the crawl.
            if (m_lsTimer >= m_lsClimaxRamp + 0.2f)
            {
                m_lsExecutionStarted = true;
                startFinale();
                Time::setTimeScale(m_lsClimaxSlowMotion);
                m_finaleSlowApplied = true;
            }
        }
        else
        {
            ShadowExecution* shadowExecution = findShadowExecution();
            const bool playing = shadowExecution != nullptr && shadowExecution->isActive();
            if (!playing && m_lsTimer >= m_lsClimaxRamp + 0.8f)
            {
                // Anyone the shockwave didn't reach (archers keep their distance) falls as it fades,
                // so the closing shot is a cleared arena.
                for (const DuoEnemy& enemy : m_duoEnemies)
                {
                    if (enemy.object == nullptr || !SceneAPI::containsGameObject(enemy.object))
                    {
                        continue;
                    }
                    EnemyDamageable* damageable = GameObjectAPI::findScript<EnemyDamageable>(enemy.object);
                    if (damageable != nullptr && !damageable->isDead())
                    {
                        damageable->forceKill();
                    }
                }

                m_lsPhase = LastStandPhase::Reveal;
                m_lsTimer = 0.0f;
                Debug::log("[TrailerPlayerShowcase] Last Stand: reveal.");
            }
        }
        break;
    }

    case LastStandPhase::Reveal:
    {
        // Speed comes back over a second while the camera cranes up over the cleared arena.
        float k = m_lsTimer / 1.0f;
        if (k > 1.0f) k = 1.0f;
        if (m_finaleSlowApplied)
        {
            const float target = m_slowMotionActive ? m_slowMotionScale : 1.0f;
            Time::setTimeScale(m_lsClimaxSlowMotion + (target - m_lsClimaxSlowMotion) * k);
            if (k >= 1.0f)
            {
                m_finaleSlowApplied = false;
            }
        }

        if (m_lsTimer >= m_lsRevealTime)
        {
            if (m_autoReel)
            {
                startLastStand();
            }
            else
            {
                m_duoRunning = false;
            }
        }
        break;
    }
    }
}

void TrailerPlayerShowcase::updateLastStandCamera(float deltaTime)
{
    updateLight();

    if (!m_cameraActive || !m_startDone)
    {
        return;
    }

    GameObject* camera = SceneAPI::getDefaultCameraGameObject();
    Transform* cameraTransform = camera ? GameObjectAPI::getTransform(camera) : nullptr;
    if (cameraTransform == nullptr)
    {
        return;
    }

    const float realDelta = realDeltaTime(deltaTime);
    const float dt = realDelta > 0.05f ? 0.05f : realDelta;

    // Follow the pair's midpoint gently, so Death's dashes don't yank the frame around.
    const Vector3 centreTarget = duoCentre();
    if (m_cameraSnapNext)
    {
        m_lsCameraCentre = centreTarget;
    }
    else
    {
        const float follow = 1.0f - std::exp(-2.0f * dt);
        m_lsCameraCentre = m_lsCameraCentre + (centreTarget - m_lsCameraCentre) * follow;
    }

    float orbitFactor = 1.0f;
    if (m_lsPhase == LastStandPhase::Climax) orbitFactor = 0.35f;
    if (m_lsPhase == LastStandPhase::Reveal) orbitFactor = 0.5f;
    m_lsOrbitYaw += m_lsOrbitSpeed * orbitFactor * dt;

    const float baseDistance = m_lsCameraDistance * m_zoomScale;
    const float pushedDistance = baseDistance * m_lsPushIn;
    const float climaxHeight = m_lsClimaxHeight;

    float distance = baseDistance;
    float height = m_lsCameraHeight;

    if (m_lsPhase == LastStandPhase::Climax)
    {
        float k = m_lsTimer / m_lsClimaxRamp;
        if (k > 1.0f) k = 1.0f;
        const float eased = k * k * (3.0f - 2.0f * k);
        distance = baseDistance + (pushedDistance - baseDistance) * eased;
        height = m_lsCameraHeight + (climaxHeight - m_lsCameraHeight) * eased;
    }
    else if (m_lsPhase == LastStandPhase::Reveal)
    {
        float k = m_lsTimer / m_lsRevealTime;
        if (k > 1.0f) k = 1.0f;
        const float eased = k * k * (3.0f - 2.0f * k);
        const float revealDistance = baseDistance * m_lsRevealDistance;
        distance = pushedDistance + (revealDistance - pushedDistance) * eased;
        height = climaxHeight + (m_lsRevealHeight - climaxHeight) * eased;
    }

    const float yaw = m_stageYaw + m_lsOrbitYaw + m_orbitOffset;
    const Vector3 lookTarget = m_lsCameraCentre + Vector3(0.0f, m_lsLookHeight, 0.0f);
    const Vector3 desiredPosition = m_lsCameraCentre + forwardFromYaw(yaw) * distance + Vector3(0.0f, height, 0.0f);

    Vector3 newPosition = desiredPosition;
    if (!m_cameraSnapNext)
    {
        const float blend = 1.0f - std::exp(-m_cameraSmoothing * dt);
        const Vector3 current = TransformAPI::getGlobalPosition(cameraTransform);
        newPosition = current + (desiredPosition - current) * blend;
    }
    m_cameraSnapNext = false;

    TransformAPI::setGlobalPosition(cameraTransform, newPosition);
    TransformAPI::lookAt(cameraTransform, lookTarget);
}

// ---------------------------------------------------------------------------
// Intro: character intros (walk up, flourish, hero hold) and the bullet-time cross
// ---------------------------------------------------------------------------

CharacterBase* TrailerPlayerShowcase::introCharacter() const
{
    return m_introShot == IntroShot::LyrielIntro ? m_lyriel : m_death;
}

void TrailerPlayerShowcase::handleIntroInput()
{
    if (comboPressed(KeyCode::Space)) startIntroShot(IntroShot::DeathIntro);
    if (comboPressed(KeyCode::L))     startIntroShot(IntroShot::LyrielIntro);
    if (comboPressed(KeyCode::J))     startIntroShot(IntroShot::Cross);

    if (comboPressed(KeyCode::Num3))
    {
        m_autoReel = !m_autoReel;
        Debug::log("[TrailerPlayerShowcase] Auto loop %s", m_autoReel ? "ON" : "OFF");
        if (m_autoReel && !m_introRunning)
        {
            startIntroShot(IntroShot::DeathIntro);
        }
    }
}

void TrailerPlayerShowcase::startIntroShot(IntroShot shot)
{
    findCharacters();
    if (m_death == nullptr || m_lyriel == nullptr)
    {
        return;
    }

    if (m_crossSlowApplied)
    {
        Time::setTimeScale(m_slowMotionActive ? m_slowMotionScale : 1.0f);
        m_crossSlowApplied = false;
    }

    setWalkDirection(m_death, Vector3::Zero);
    setWalkDirection(m_lyriel, Vector3::Zero);

    if (!getStage(m_stage, m_stageYaw))
    {
        return;
    }

    const Vector3 towardsCamera = forwardFromYaw(m_stageYaw);
    const float yawRadians = m_stageYaw * (kPi / 180.0f);
    const Vector3 right(std::cos(yawRadians), 0.0f, -std::sin(yawRadians));

    m_introShot = shot;
    m_introTimer = 0.0f;
    m_introRunning = true;
    m_introActing = false;
    m_cameraSnapNext = true;
    m_zoomScale = 1.0f;
    m_orbitOffset = 0.0f;
    m_introStopMark = snapToNavMesh(m_stage, NavAgentProfile::PlayerNormal);

    if (shot == IntroShot::Cross)
    {
        // An X across the middle. Each starts half a dash-length out, so both reach the centre together:
        // Death from back-left to front-right, Lyriel from back-right to front-left.
        Vector3 deathDiagonal = right + towardsCamera;
        Vector3 lyrielDiagonal = towardsCamera - right;
        deathDiagonal = deathDiagonal * (1.0f / std::sqrt(deathDiagonal.x * deathDiagonal.x + deathDiagonal.z * deathDiagonal.z));
        lyrielDiagonal = lyrielDiagonal * (1.0f / std::sqrt(lyrielDiagonal.x * lyrielDiagonal.x + lyrielDiagonal.z * lyrielDiagonal.z));

        const Vector3 deathStart = snapToNavMesh(m_introStopMark - deathDiagonal * 2.5f, NavAgentProfile::PlayerNormal);
        const Vector3 lyrielStart = snapToNavMesh(m_introStopMark - lyrielDiagonal * 1.5f, NavAgentProfile::PlayerNormal);

        if (Transform* transform = GameObjectAPI::getTransform(m_death->getOwner()))
        {
            TransformAPI::setGlobalPosition(transform, deathStart);
        }
        if (Transform* transform = GameObjectAPI::getTransform(m_lyriel->getOwner()))
        {
            TransformAPI::setGlobalPosition(transform, lyrielStart);
        }
        facePoint(m_death, m_introStopMark);
        facePoint(m_lyriel, m_introStopMark);

        m_characterIndex = 0;
        m_introStage = IntroStage::Standoff;
        Debug::log("[TrailerPlayerShowcase] Intro: bullet-time cross.");
        return;
    }

    // Character intro: start well back, walk up to the mark in front of the lens.
    CharacterBase* hero = introCharacter();
    CharacterBase* other = (hero == m_death) ? m_lyriel : m_death;
    m_characterIndex = (hero == m_death) ? 0 : 1;

    m_introStartMark = snapToNavMesh(m_introStopMark - towardsCamera * m_introWalkDistance, NavAgentProfile::PlayerNormal);
    if (Transform* transform = GameObjectAPI::getTransform(hero->getOwner()))
    {
        TransformAPI::setGlobalPosition(transform, m_introStartMark);
        TransformAPI::setGlobalRotationEuler(transform, Vector3(0.0f, yawFromDirection(towardsCamera), 0.0f));
    }

    // The other one waits behind the camera.
    if (Transform* transform = GameObjectAPI::getTransform(other->getOwner()))
    {
        TransformAPI::setGlobalPosition(transform,
            snapToNavMesh(m_introStopMark + towardsCamera * (m_introCameraDistance + 8.0f), NavAgentProfile::PlayerNormal));
    }

    m_introStage = (m_introWalkDistance < 0.3f) ? IntroStage::Flourish : IntroStage::Walk;
    Debug::log("[TrailerPlayerShowcase] Intro: %s.", hero == m_death ? "Death" : "Lyriel");
}

void TrailerPlayerShowcase::updateIntro(float deltaTime)
{
    if (!m_introRunning)
    {
        return;
    }

    m_introTimer += realDeltaTime(deltaTime);

    if (m_introShot == IntroShot::Cross)
    {
        switch (m_introStage)
        {
        case IntroStage::Standoff:
            // A beat of them squaring up, then both go at once and time all but stops.
            if (m_introTimer >= m_crossStandoff)
            {
                facePoint(m_death, m_introStopMark);
                facePoint(m_lyriel, m_introStopMark);
                if (AbilityBase* dash = abilityOf(m_death, ReelStep::Dash)) dash->tryAbility();
                if (AbilityBase* dash = abilityOf(m_lyriel, ReelStep::Dash)) dash->tryAbility();

                Time::setTimeScale(m_crossBulletTime);
                m_crossSlowApplied = true;
                m_introStage = IntroStage::BulletTime;
                m_introTimer = 0.0f;
            }
            break;

        case IntroStage::BulletTime:
            // The camera swings round them while they hang in the air; then time snaps back and they land.
            if (m_introTimer >= m_crossOrbitTime)
            {
                Time::setTimeScale(m_slowMotionActive ? m_slowMotionScale : 1.0f);
                m_crossSlowApplied = false;
                m_introStage = IntroStage::Land;
                m_introTimer = 0.0f;
            }
            break;

        case IntroStage::Land:
            if (m_introTimer >= m_crossLandHold)
            {
                if (m_autoReel)
                {
                    startIntroShot(IntroShot::DeathIntro);
                }
                else
                {
                    m_introRunning = false;
                }
            }
            break;

        default:
            break;
        }
        return;
    }

    CharacterBase* hero = introCharacter();
    Transform* heroTransform = hero ? GameObjectAPI::getTransform(hero->getOwner()) : nullptr;
    if (heroTransform == nullptr)
    {
        return;
    }
    const Vector3 position = TransformAPI::getGlobalPosition(heroTransform);

    switch (m_introStage)
    {
    case IntroStage::Walk:
    {
        Vector3 delta = m_introStopMark - position;
        delta.y = 0.0f;
        const float distance = std::sqrt(delta.x * delta.x + delta.z * delta.z);
        if (distance < 0.25f || m_introTimer > 8.0f)
        {
            setWalkDirection(hero, Vector3::Zero);
            m_introStage = IntroStage::Flourish;
            m_introTimer = 0.0f;
        }
        else
        {
            setWalkDirection(hero, delta * (1.0f / distance));
        }
        break;
    }

    case IntroStage::Flourish:
    {
        if (!m_introActing && m_introTimer < 0.1f)
        {
            // A fully charged attack, aimed a little off the lens so it reads as a swing, not at the viewer.
            const Vector3 aim = position + forwardFromYaw(m_stageYaw + m_introFlourishAngle) * 5.0f;
            m_introActing = startAction(hero, ReelStep::Charged, 1, m_introAction, &aim);
            if (!m_introActing)
            {
                m_introTimer = 0.1f; // no charged attack: straight to the hold
            }
        }

        if (m_introActing)
        {
            updateAction(m_introAction, deltaTime);
            if (m_introAction.done)
            {
                m_introActing = false;
            }
        }

        if (!m_introActing && m_introTimer >= 0.1f)
        {
            // Turn to the lens for the hero hold.
            facePoint(hero, position + forwardFromYaw(m_stageYaw + m_introCameraAngle));
            m_introStage = IntroStage::Hold;
            m_introTimer = 0.0f;
        }
        else if (m_introTimer > 8.0f)
        {
            m_introActing = false;
        }
        break;
    }

    case IntroStage::Hold:
        if (m_introTimer >= m_introHoldTime)
        {
            if (!m_autoReel)
            {
                m_introRunning = false;
            }
            else
            {
                startIntroShot(m_introShot == IntroShot::DeathIntro ? IntroShot::LyrielIntro : IntroShot::Cross);
            }
        }
        break;

    default:
        break;
    }
}

void TrailerPlayerShowcase::updateIntroCamera(float deltaTime)
{
    updateLight();

    if (!m_cameraActive || !m_startDone)
    {
        return;
    }

    GameObject* camera = SceneAPI::getDefaultCameraGameObject();
    Transform* cameraTransform = camera ? GameObjectAPI::getTransform(camera) : nullptr;
    if (cameraTransform == nullptr)
    {
        return;
    }

    const float realDelta = realDeltaTime(deltaTime);
    const float dt = realDelta > 0.05f ? 0.05f : realDelta;

    Vector3 lookTarget;
    Vector3 desiredPosition;
    bool exact = false;

    if (m_introShot == IntroShot::Cross)
    {
        // Low and close on the crossing point; the swing is eased so it starts and ends gently.
        float swing = 0.0f;
        if (m_introStage == IntroStage::BulletTime)
        {
            float k = m_introTimer / m_crossOrbitTime;
            if (k > 1.0f) k = 1.0f;
            swing = m_crossOrbitDegrees * (k * k * (3.0f - 2.0f * k));
            exact = true;
        }
        else if (m_introStage == IntroStage::Land)
        {
            swing = m_crossOrbitDegrees;
        }

        const float yaw = m_stageYaw + m_orbitOffset + swing;
        lookTarget = m_introStopMark + Vector3(0.0f, 1.0f, 0.0f);
        desiredPosition = m_introStopMark + forwardFromYaw(yaw) * (m_crossCameraDistance * m_zoomScale)
                        + Vector3(0.0f, m_crossCameraHeight, 0.0f);
    }
    else
    {
        // Three-quarter view that backs away as they walk up (same distance from them), then a slow push-in.
        CharacterBase* hero = introCharacter();
        Transform* heroTransform = hero ? GameObjectAPI::getTransform(hero->getOwner()) : nullptr;
        if (heroTransform == nullptr)
        {
            return;
        }
        const Vector3 heroPosition = TransformAPI::getGlobalPosition(heroTransform);

        float distance = m_introCameraDistance * m_zoomScale;
        if (m_introStage == IntroStage::Hold && m_introHoldTime > 0.0f)
        {
            float k = m_introTimer / m_introHoldTime;
            if (k > 1.0f) k = 1.0f;
            const float eased = k * k * (3.0f - 2.0f * k);
            distance *= 1.0f + (m_introHoldPushIn - 1.0f) * eased;
        }

        const float yaw = m_stageYaw + m_introCameraAngle + m_orbitOffset;
        lookTarget = heroPosition + Vector3(0.0f, m_introLookHeight, 0.0f);
        desiredPosition = heroPosition + forwardFromYaw(yaw) * distance + Vector3(0.0f, m_introCameraHeight, 0.0f);
    }

    Vector3 newPosition = desiredPosition;
    if (!m_cameraSnapNext && !exact)
    {
        const float blend = 1.0f - std::exp(-m_cameraSmoothing * dt);
        const Vector3 current = TransformAPI::getGlobalPosition(cameraTransform);
        newPosition = current + (desiredPosition - current) * blend;
    }
    m_cameraSnapNext = false;

    TransformAPI::setGlobalPosition(cameraTransform, newPosition);
    TransformAPI::lookAt(cameraTransform, lookTarget);
}

// ---------------------------------------------------------------------------
// Sync: paired abilities in mirror image, then the Shadow Execution showpiece
// ---------------------------------------------------------------------------

void TrailerPlayerShowcase::handleSyncInput()
{
    if (comboPressed(KeyCode::Space)) startSync();
    if (comboPressed(KeyCode::J) && m_syncRunning && m_syncStage != SyncStage::Execution)
    {
        enterSyncStage(SyncStage::ExecSetup);
    }

    if (comboPressed(KeyCode::Num3))
    {
        m_autoReel = !m_autoReel;
        Debug::log("[TrailerPlayerShowcase] Auto loop %s", m_autoReel ? "ON" : "OFF");
        if (m_autoReel && !m_syncRunning)
        {
            startSync();
        }
    }
}

void TrailerPlayerShowcase::startSync()
{
    findCharacters();
    if (m_death == nullptr || m_lyriel == nullptr)
    {
        return;
    }

    ShadowExecution* shadowExecution = findShadowExecution();
    if (shadowExecution != nullptr && shadowExecution->isActive())
    {
        return; // let the execution finish
    }

    if (m_syncSlowApplied)
    {
        Time::setTimeScale(m_slowMotionActive ? m_slowMotionScale : 1.0f);
        m_syncSlowApplied = false;
    }

    setWalkDirection(m_death, Vector3::Zero);
    setWalkDirection(m_lyriel, Vector3::Zero);

    if (!getStage(m_stage, m_stageYaw))
    {
        return;
    }

    m_syncRunning = true;
    m_characterIndex = 0;
    m_zoomScale = 1.0f;
    m_orbitOffset = 0.0f;
    m_cameraSnapNext = true;
    enterSyncStage(SyncStage::Setup);
}

void TrailerPlayerShowcase::enterSyncStage(SyncStage stage)
{
    m_syncStage = stage;
    m_syncTimer = 0.0f;
    m_syncWaiting = false;

    const Vector3 towardsCamera = forwardFromYaw(m_stageYaw);
    const float yawRadians = m_stageYaw * (kPi / 180.0f);
    const Vector3 right(std::cos(yawRadians), 0.0f, -std::sin(yawRadians));

    // Mirror image: each faces out to their own side, angled towards the lens.
    const float outward = m_syncOutwardAngle * (kPi / 180.0f);
    const Vector3 deathFacing = towardsCamera * std::cos(outward) - right * std::sin(outward);
    const Vector3 lyrielFacing = towardsCamera * std::cos(outward) + right * std::sin(outward);

    auto place = [](CharacterBase* character, const Vector3& position, const Vector3& facing)
    {
        if (Transform* transform = character ? GameObjectAPI::getTransform(character->getOwner()) : nullptr)
        {
            TransformAPI::setGlobalPosition(transform, position);
            TransformAPI::setGlobalRotationEuler(transform, Vector3(0.0f, yawFromDirection(facing), 0.0f));
        }
    };

    auto beat = [&](ReelStep step, int presses, const char* name)
    {
        // Both on the same frame, each aimed out along their own mirror line.
        CharacterBase* characters[] = { m_death, m_lyriel };
        ActorAction* actions[] = { &m_syncDeathAction, &m_syncLyrielAction };
        const Vector3 facings[] = { deathFacing, lyrielFacing };
        for (int i = 0; i < 2; ++i)
        {
            Transform* transform = characters[i] ? GameObjectAPI::getTransform(characters[i]->getOwner()) : nullptr;
            const Vector3 position = transform ? TransformAPI::getGlobalPosition(transform) : m_stage;
            const Vector3 aim = position + facings[i] * 5.0f;
            if (!startAction(characters[i], step, presses, *actions[i], &aim))
            {
                *actions[i] = ActorAction();
                actions[i]->done = true;
            }
        }
        Debug::log("[TrailerPlayerShowcase] Sync: %s", name);
    };

    switch (stage)
    {
    case SyncStage::Setup:
        place(m_death, snapToNavMesh(m_stage - right * (m_syncSeparation * 0.5f), NavAgentProfile::PlayerNormal), deathFacing);
        place(m_lyriel, snapToNavMesh(m_stage + right * (m_syncSeparation * 0.5f), NavAgentProfile::PlayerNormal), lyrielFacing);
        break;

    case SyncStage::Basic:
        beat(ReelStep::Basic, m_basicComboPresses, "combo + shots");
        break;

    case SyncStage::Charged:
        beat(ReelStep::Charged, 1, "charged attack + charged shot");
        break;

    case SyncStage::Special:
        beat(ReelStep::Special, 1, "chain taunt + arrow volley");
        break;

    case SyncStage::ExecSetup:
        // Cut: the two of them close together facing the lens, the gauge lit, the camera low and close.
        m_syncDeathAction = ActorAction();
        m_syncLyrielAction = ActorAction();
        place(m_death, snapToNavMesh(m_stage - right * 0.6f, NavAgentProfile::PlayerNormal), towardsCamera);
        place(m_lyriel, snapToNavMesh(m_stage + right * 0.6f, NavAgentProfile::PlayerNormal), towardsCamera);
        fillReaperGauge();
        m_syncExecutionStarted = false;
        m_execOrbitYaw = 0.0f;
        m_cameraSnapNext = true;
        Debug::log("[TrailerPlayerShowcase] Sync: Shadow Execution showpiece.");
        break;

    case SyncStage::Execution:
    {
        ShadowExecution* shadowExecution = findShadowExecution();
        if (shadowExecution != nullptr && shadowExecution->triggerNow())
        {
            m_syncExecutionStarted = true;
            if (m_execSlowMotion < 0.999f)
            {
                Time::setTimeScale(m_execSlowMotion);
                m_syncSlowApplied = true;
            }
        }
        else
        {
            Debug::warn("[TrailerPlayerShowcase] Shadow Execution didn't start (is ShadowExecution on GameManager?).");
        }
        break;
    }

    case SyncStage::ExecHold:
        if (m_syncSlowApplied)
        {
            Time::setTimeScale(m_slowMotionActive ? m_slowMotionScale : 1.0f);
            m_syncSlowApplied = false;
        }
        break;
    }
}

void TrailerPlayerShowcase::updateSync(float deltaTime)
{
    if (!m_syncRunning)
    {
        return;
    }

    m_syncTimer += realDeltaTime(deltaTime);

    switch (m_syncStage)
    {
    case SyncStage::Setup:
        if (m_syncTimer >= 1.0f)
        {
            enterSyncStage(SyncStage::Basic);
        }
        break;

    case SyncStage::Basic:
    case SyncStage::Charged:
    case SyncStage::Special:
        updateAction(m_syncDeathAction, deltaTime);
        updateAction(m_syncLyrielAction, deltaTime);

        if (!m_syncWaiting)
        {
            if (m_syncDeathAction.done && m_syncLyrielAction.done)
            {
                m_syncWaiting = true;
                m_syncTimer = 0.0f;
            }
        }
        else if (m_syncTimer >= m_syncBeatGap)
        {
            enterSyncStage(m_syncStage == SyncStage::Basic ? SyncStage::Charged
                         : m_syncStage == SyncStage::Charged ? SyncStage::Special
                         : SyncStage::ExecSetup);
        }
        break;

    case SyncStage::ExecSetup:
        // A beat to let the gauge's colour grade settle and the low camera find its line.
        if (m_syncTimer >= 1.2f)
        {
            enterSyncStage(SyncStage::Execution);
        }
        break;

    case SyncStage::Execution:
    {
        ShadowExecution* shadowExecution = findShadowExecution();
        const bool playing = m_syncExecutionStarted && shadowExecution != nullptr && shadowExecution->isActive();
        if (!playing && m_syncTimer >= 0.5f)
        {
            enterSyncStage(SyncStage::ExecHold);
        }
        break;
    }

    case SyncStage::ExecHold:
        if (m_syncTimer >= m_execHold)
        {
            if (m_autoReel)
            {
                startSync();
            }
            else
            {
                m_syncRunning = false;
            }
        }
        break;
    }
}

void TrailerPlayerShowcase::updateSyncCamera(float deltaTime)
{
    updateLight();

    if (!m_cameraActive || !m_startDone)
    {
        return;
    }

    GameObject* camera = SceneAPI::getDefaultCameraGameObject();
    Transform* cameraTransform = camera ? GameObjectAPI::getTransform(camera) : nullptr;
    if (cameraTransform == nullptr)
    {
        return;
    }

    const float realDelta = realDeltaTime(deltaTime);
    const float dt = realDelta > 0.05f ? 0.05f : realDelta;

    const bool showpiece = (m_syncStage == SyncStage::ExecSetup || m_syncStage == SyncStage::Execution || m_syncStage == SyncStage::ExecHold);

    float yaw = m_stageYaw + m_orbitOffset;
    float distance = m_syncCameraDistance;
    float height = m_syncCameraHeight;
    float lookHeight = m_syncLookHeight;

    if (showpiece)
    {
        // Low, close and slowly circling, in real time so it keeps moving through the slow motion.
        m_execOrbitYaw += m_execOrbitSpeed * dt;
        yaw += m_execOrbitYaw;
        distance = m_execCameraDistance;
        height = m_execCameraHeight;
        lookHeight = m_execLookHeight;
    }

    distance *= m_zoomScale;
    const Vector3 lookTarget = m_stage + Vector3(0.0f, lookHeight, 0.0f);
    const Vector3 desiredPosition = m_stage + forwardFromYaw(yaw) * distance + Vector3(0.0f, height, 0.0f);

    Vector3 newPosition = desiredPosition;
    if (!m_cameraSnapNext)
    {
        const float blend = 1.0f - std::exp(-m_cameraSmoothing * dt);
        const Vector3 current = TransformAPI::getGlobalPosition(cameraTransform);
        newPosition = current + (desiredPosition - current) * blend;
    }
    m_cameraSnapNext = false;

    TransformAPI::setGlobalPosition(cameraTransform, newPosition);
    TransformAPI::lookAt(cameraTransform, lookTarget);
}

IMPLEMENT_SCRIPT(TrailerPlayerShowcase)
