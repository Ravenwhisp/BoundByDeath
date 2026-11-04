#pragma once

#include "ScriptAPI.h"
#include "ParticleLifecycle.h"

#include <unordered_map>

// Trailer recording helper for the playable characters.
// Stages Death or Lyriel in the middle of the room, gives a turntable "model" shot, then plays every
// ability against a training dummy, all hands-free (auto reel) or from RIGHT SHIFT key combos.
class CameraFollow;
class CharacterBase;
class AbilityBase;
class ShadowExecution;

class TrailerPlayerShowcase final : public Script
{
    DECLARE_SCRIPT(TrailerPlayerShowcase)

public:
    explicit TrailerPlayerShowcase(GameObject* owner);

    void Start() override;
    void Update() override;
    void OnGameStop() override;

    FieldList getExposedFields() const override;

public:
    // Who and what
    int m_startCharacter = 0;               // 0 = Death, 1 = Lyriel
    bool m_reelBothCharacters = true;       // after one character's reel, switch to the other
    bool m_autoReel = true;                 // hands-free: model shot, then every ability, repeat
    float m_startDelay = 1.0f;

    // Training dummy (an enemy prefab the abilities hit)
    PrefabRef m_dummyPrefab;
    PrefabRef m_spawnEffectPrefab;
    bool m_useDummy = true;                 // false = abilities play into empty space (RShift+4 toggles)
    float m_deathDummyDistance = 1.6f;      // Death's combo, charged attack and dash: right up close
    float m_deathTauntDummyDistance = 4.5f; // Death's chain taunt: far enough to see the pull (cone reaches 5 m)
    float m_dummyDistanceLyriel = 6.5f;     // bow range (keep under the 8 m targeting range)
    bool m_dummyImmortal = true;            // heals itself back up so it survives the whole reel
    bool m_dummyHoldInPlace = true;         // never walks off its mark (taunt can still pull it)

    // Model shot (turntable)
    float m_modelShotTime = 7.0f;           // seconds per turntable
    float m_turntableSpeed = 45.0f;         // degrees per second
    float m_modelHeight = 2.1f;             // character height used for framing
    float m_modelPadding = 1.25f;           // 1 = tight
    float m_modelLookHeight = 1.05f;

    // Ability shots
    float m_abilityCameraAngle = 30.0f;     // degrees round towards the character's front
    float m_abilityPadding = 1.3f;
    float m_abilityLookHeight = 1.0f;
    float m_stepGap = 1.2f;                 // pause between abilities
    float m_specialAimTime = 0.6f;          // how long the taunt / volley aim cone shows before release
    int m_basicComboPresses = 3;

    // Camera and look
    float m_cameraSmoothing = 4.0f;
    float m_cameraTilt = 0.18f;             // camera height above the look point, per metre of distance
    float m_fov = 0.0f;                     // 0 = keep the game's FOV
    bool m_hideHUD = true;
    ComponentRef<Transform> m_light;        // optional key light that follows the character
    float m_lightHeight = 1.2f;
    float m_lightForward = 1.5f;

    // Duo mode: both characters together, showing their asymmetric roles. Each runs its own routine:
    // Death walks in, attacks, backs off, chain-pulls, walks in, attacks; Lyriel keeps shooting from the side.
    // Then Shadow Execution.
    bool m_duoMode = false;
    PrefabRef m_duoEnemyPrefab2;            // optional second enemy type, alternated with the dummy prefab
    int m_duoEnemyCount = 4;
    float m_duoLyrielBack = 1.0f;           // Lyriel's spot: this far behind Death's fighting spot...
    float m_duoLyrielSide = 2.0f;           // ...and this far off to the side (away from the camera)
    float m_duoDeathFront = 0.5f;           // Death's fighting spot: this far in front of the stage point
    float m_duoEnemyDistance = 1.8f;        // enemies stand this far beyond Death's fighting spot
    float m_duoEnemySpread = 1.2f;          // how wide the group is
    float m_duoMeleeDistance = 1.4f;        // Death walks up to this distance from an enemy before attacking
    float m_duoTauntDistance = 4.0f;        // Death backs off to this distance before the chain pull (taunt reaches 5 m)
    float m_duoDashRunUp = 5.0f;            // Death starts this far behind his fighting spot; his 5 m dash lands him on it
    float m_duoDashMinDistance = 4.0f;      // Death only dashes in if the nearest enemy is further than this
    float m_duoLyrielDashSpot = 2.0f;       // Lyriel dashes between her spot and one this far further back
    int m_duoEarlyKills = 1;                // enemies Death's first combo finishes off
    int m_duoFinaleSurvivors = 2;           // enemies kept alive for Shadow Execution; the rest fall to Death's finishers
    float m_duoMortalHealth = 0.15f;        // health (fraction) enemies drop to when it's their turn to die
    float m_duoCameraAngle = 15.0f;
    float m_duoPadding = 1.05f;
    float m_duoBeatGap = 0.5f;              // Death's pause between moves
    float m_duoLyrielGap = 0.4f;            // Lyriel's pause between attacks
    float m_duoFinaleSlowMotion = 0.5f;     // time scale during Shadow Execution (1 = off)
    float m_duoLoopDelay = 4.0f;            // after the finale, before the next take

    // Last Stand mode: back to back in the middle, waves of enemies close in from every side, both fight,
    // time slows to a crawl, Shadow Execution wipes the horde, the camera cranes up over the empty arena.
    bool m_lastStandMode = false;
    int m_lsWaves = 3;
    int m_lsWaveSize = 4;
    float m_lsWaveInterval = 5.0f;          // seconds between waves
    float m_lsRingRadius = 5.5f;            // waves appear this far out and charge in
    float m_lsClimaxDelay = 4.0f;           // after the last wave arrives, before time slows
    float m_lsClimaxSlowMotion = 0.25f;     // time scale for the climax and the execution
    float m_lsClimaxRamp = 1.2f;            // seconds (real time) to slow down and push in
    float m_lsCameraDistance = 8.5f;
    float m_lsCameraHeight = 2.0f;          // low, heroic angle
    float m_lsLookHeight = 1.3f;
    float m_lsOrbitSpeed = 10.0f;           // degrees per second the camera circles the pair
    float m_lsPushIn = 0.72f;               // camera distance multiplier at the height of the climax
    float m_lsClimaxHeight = 3.4f;          // camera rises a little for the climax, to see over the crowd
    bool m_lsHideHealthBars = true;         // enemy health bars off for the slow-motion climax
    float m_lsRevealHeight = 9.0f;          // crane-up height for the closing shot
    float m_lsRevealDistance = 1.7f;        // closing shot distance, as a multiple of the normal one
    float m_lsRevealTime = 4.0f;            // seconds (real time) the closing shot lasts

    // Intro mode: no enemies. Each character walks up to the camera, does a flourish (fully charged attack)
    // and holds a hero pose for the title card; then the two dash across each other in bullet time.
    bool m_introMode = false;
    float m_introWalkDistance = 6.0f;       // how far back each character starts their walk
    float m_introCameraDistance = 3.8f;
    float m_introCameraHeight = 1.4f;
    float m_introLookHeight = 1.35f;
    float m_introCameraAngle = 20.0f;       // degrees off straight-on, for a three-quarter view
    float m_introFlourishAngle = 40.0f;     // the flourish is aimed this far off the lens
    float m_introHoldTime = 2.5f;           // hero pose hold (title card time)
    float m_introHoldPushIn = 0.8f;         // slow push-in during the hold
    float m_crossStandoff = 1.2f;           // stare-down before the dash
    float m_crossBulletTime = 0.04f;        // time scale while the camera swings round
    float m_crossOrbitDegrees = 180.0f;
    float m_crossOrbitTime = 3.0f;          // real seconds for the swing
    float m_crossCameraDistance = 4.5f;
    float m_crossCameraHeight = 1.1f;
    float m_crossLandHold = 1.5f;           // after landing, before the sequence repeats

    // Sync mode: side by side in mirror image, both fire matching abilities on the same beat
    // (combo + shots, charged + charged, taunt + volley); then Shadow Execution alone on an empty stage.
    bool m_syncMode = false;
    float m_syncSeparation = 2.6f;          // distance between them
    float m_syncOutwardAngle = 55.0f;       // each faces this far out from the camera direction, mirrored
    float m_syncCameraDistance = 7.0f;
    float m_syncCameraHeight = 2.0f;
    float m_syncLookHeight = 1.1f;
    float m_syncBeatGap = 0.8f;
    float m_execCameraDistance = 3.6f;      // the execution showpiece: low and close...
    float m_execCameraHeight = 0.5f;
    float m_execLookHeight = 1.1f;
    float m_execOrbitSpeed = 14.0f;         // ...slowly circling (real-time degrees per second)
    float m_execSlowMotion = 0.3f;
    float m_execHold = 2.0f;                // after the execution, before the loop

    // Comfort
    bool m_playersInvulnerable = true;
    bool m_ignorePlayerInput = true;        // keyboard / mouse / pads can't move or aim the characters
    float m_parkDistance = 10.0f;           // the other character waits this far behind the camera
    float m_slowMotionScale = 0.35f;
    bool m_removeHealthDrops = true;
    bool m_logControlsOnStart = true;

private:
    enum class ReelStep { Model = 0, Basic, Charged, Dash, Special, Count };
    enum class StepPhase { Prepare, Pressing, Running, Gap };

    void handleInput(float deltaTime);
    void handleSoloInput();
    bool comboPressed(KeyCode key);
    bool comboHeld(KeyCode key) const;

    void findCharacters();
    CharacterBase* activeCharacter() const;
    CharacterBase* otherCharacter() const;
    AbilityBase* abilityFor(ReelStep step) const;
    const char* stepName(ReelStep step) const;
    const char* characterName() const;

    void switchCharacter();
    void enterStep(ReelStep step);
    void updateReel(float deltaTime);
    void beginPressing();
    void updatePressing(float deltaTime);
    void releaseSpecial();
    bool characterBusy() const;

    void stageCharacter(bool faceCamera);
    void parkOtherCharacter();
    void ensureDummy();
    void removeDummy();
    void updateDummy();
    bool isDummyAlive() const;

    bool getStage(Vector3& outPosition, float& outYaw) const;
    Vector3 characterPosition() const;
    void setCamera(bool active);
    void updateCamera(float deltaTime);
    void updateLight();

    void setPlayersInvulnerable(bool invulnerable);
    void applyInputDetach(bool detach);
    void removeHealthDrops();
    void toggleSlowMotion();
    void logControls() const;

    // Duo mode
    enum class DuoPhase { Setup, Fight, Finale, Aftermath };
    enum class DuoMove { WalkToEnemies, WalkBack, WalkHome, Basic, Charged, Special,
                         DashToEnemies, DashToSpot, DashToHome, MarkEarlyKills, MarkFinisherKills, DashAway };
    enum class LastStandPhase { Intro, Fight, Climax, Reveal };

    struct DuoEnemy
    {
        GameObject* object = nullptr;
        Vector3 mark = Vector3::Zero;
        int frames = 0;
        bool seen = false;
        bool prepared = false;
        bool mortal = false;                // stops healing: allowed to die before the finale
        bool pending = true;                // summon effect playing, not spawned yet
        bool effectPlayed = false;
        bool secondType = false;
        float spawnTimer = 0.0f;
    };

    struct ActorAction
    {
        CharacterBase* character = nullptr;
        ReelStep step = ReelStep::Model;
        int pressesLeft = 0;
        int baseline = 0;
        float timer = 0.0f;
        float pressCooldown = 0.0f;
        bool released = false;
        bool done = false;
    };

    struct DuoTrack
    {
        CharacterBase* character = nullptr;
        std::vector<DuoMove> moves;
        size_t index = 0;
        bool loop = false;
        bool finished = false;
        float gap = 0.0f;
        float gapTimer = 0.0f;
        bool acting = false;
        ActorAction action;
        bool walking = false;
        Vector3 walkTarget = Vector3::Zero;
        float walkTimer = 0.0f;
    };

    void handleDuoInput();
    void startDuo();
    void updateDuo(float deltaTime);
    void beginFinale();
    void stopTracks();
    void updateTrack(DuoTrack& track, float deltaTime);
    void startTrackMove(DuoTrack& track);
    void advanceTrack(DuoTrack& track);
    bool walkTargetFor(const DuoTrack& track, DuoMove move, Vector3& outTarget) const;
    void setWalkDirection(CharacterBase* character, const Vector3& direction);
    bool startAction(CharacterBase* character, ReelStep step, int presses, ActorAction& outAction, const Vector3* facePoint = nullptr);
    void facePoint(CharacterBase* character, const Vector3& point);
    void markMortal(int count);
    void updateAction(ActorAction& action, float deltaTime);
    AbilityBase* abilityOf(CharacterBase* character, ReelStep step) const;
    void releaseSpecialOf(CharacterBase* character);
    void placeDuoCharacters();
    void faceEnemies(CharacterBase* character);
    bool nearestEnemy(const Vector3& from, Vector3& outPosition) const;
    void spawnDuoEnemies();
    void updateDuoEnemies(float deltaTime);
    void removeDuoEnemies();
    Vector3 duoEnemyCentroid() const;
    int liveDuoEnemyCount() const;
    void startFinale();
    void updateDuoCamera(float deltaTime);

    // Last Stand
    void startLastStand();
    void spawnWave();
    void updateLastStand(float deltaTime);
    void enterClimax();
    void updateLastStandCamera(float deltaTime);
    float realDeltaTime(float deltaTime) const;
    Vector3 duoCentre() const;

    // Sync
    enum class SyncStage { Setup, Basic, Charged, Special, ExecSetup, Execution, ExecHold };
    void handleSyncInput();
    void startSync();
    void enterSyncStage(SyncStage stage);
    void updateSync(float deltaTime);
    void updateSyncCamera(float deltaTime);

    // Intro
    enum class IntroShot { DeathIntro, LyrielIntro, Cross };
    enum class IntroStage { Walk, Flourish, Hold, Standoff, BulletTime, Land };
    void handleIntroInput();
    void startIntroShot(IntroShot shot);
    void updateIntro(float deltaTime);
    void updateIntroCamera(float deltaTime);
    CharacterBase* introCharacter() const;
    void fillReaperGauge();
    ShadowExecution* findShadowExecution();

    std::unordered_map<KeyCode, bool> m_previousKeyStates;
    ParticleLifecycle::TimedParticleTracker m_effectTracker;

    CharacterBase* m_death = nullptr;
    CharacterBase* m_lyriel = nullptr;
    int m_characterIndex = 0;

    ReelStep m_step = ReelStep::Model;
    StepPhase m_phase = StepPhase::Prepare;
    float m_stepTimer = 0.0f;
    bool m_reelRunning = false;             // true while a step (auto or manual) is in progress
    bool m_singleStep = false;              // manual key press: run one step, then stop

    int m_pressesLeft = 0;
    int m_pressBaseline = 0;
    float m_pressTimer = 0.0f;
    float m_pressCooldown = 0.0f;
    bool m_specialReleased = false;

    Vector3 m_stage = Vector3::Zero;
    float m_stageYaw = 0.0f;
    Vector3 m_characterMark = Vector3::Zero;
    Vector3 m_dummyMark = Vector3::Zero;

    GameObject* m_dummy = nullptr;
    int m_dummyFrames = 0;
    bool m_dummySeen = false;
    bool m_dummyPending = false;
    float m_dummySpawnTimer = 0.0f;
    bool m_dummyEffectPlayed = false;
    bool m_dummySpeedLocked = false;

    bool m_cameraActive = false;
    bool m_cameraSnapNext = true;
    float m_orbitOffset = 0.0f;             // extra yaw from orbit keys / turntable
    float m_zoomScale = 1.0f;
    float m_savedFov = 0.0f;
    CameraFollow* m_cameraFollow = nullptr;

    bool m_slowMotionActive = false;
    bool m_startDone = false;
    float m_startTimer = 0.0f;
    bool m_invulnerabilityApplied = false;

    // Duo state
    std::vector<DuoEnemy> m_duoEnemies;
    DuoTrack m_deathTrack;
    DuoTrack m_lyrielTrack;
    DuoPhase m_duoPhase = DuoPhase::Setup;
    float m_phaseTimer = 0.0f;
    bool m_duoRunning = false;
    bool m_duoEnemiesSpawned = false;
    bool m_duoEffectsPlayed = false;
    bool m_finaleStarted = false;
    bool m_finaleKills = false;             // enemies stop healing once the finale is coming
    bool m_finaleSlowApplied = false;
    Vector3 m_duoDeathMark = Vector3::Zero; // where Death starts (behind, for the dash in)
    Vector3 m_duoFightMark = Vector3::Zero; // where Death fights from

    // Last Stand state
    LastStandPhase m_lsPhase = LastStandPhase::Intro;
    float m_lsTimer = 0.0f;                 // real seconds in the current phase
    float m_lsWaveTimer = 0.0f;
    int m_lsWavesSpawned = 0;
    float m_lsOrbitYaw = 0.0f;
    bool m_lsExecutionStarted = false;
    Vector3 m_lsCameraCentre = Vector3::Zero;

    // Sync state
    SyncStage m_syncStage = SyncStage::Setup;
    float m_syncTimer = 0.0f;               // real seconds in the current stage
    bool m_syncRunning = false;
    bool m_syncWaiting = false;             // both finished, counting the gap
    ActorAction m_syncDeathAction;
    ActorAction m_syncLyrielAction;
    bool m_syncSlowApplied = false;
    bool m_syncExecutionStarted = false;
    float m_execOrbitYaw = 0.0f;

    // Intro state
    IntroShot m_introShot = IntroShot::DeathIntro;
    IntroStage m_introStage = IntroStage::Walk;
    float m_introTimer = 0.0f;              // real seconds in the current stage
    bool m_introRunning = false;
    Vector3 m_introStopMark = Vector3::Zero;
    Vector3 m_introStartMark = Vector3::Zero;
    ActorAction m_introAction;
    bool m_introActing = false;
    bool m_crossSlowApplied = false;
    Vector3 m_duoLyrielMark = Vector3::Zero;
    ShadowExecution* m_shadowExecution = nullptr;
};
