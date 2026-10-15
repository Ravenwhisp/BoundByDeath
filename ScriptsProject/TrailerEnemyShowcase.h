#pragma once

#include "ScriptAPI.h"
#include "ParticleLifecycle.h"

#include <unordered_map>
#include <vector>

// Trailer recording helper.
// Spawns any enemy prefab around the players on a key press (with a summon
// effect), and gives quick controls for kills, stuns, slow motion and
// automatic waves so enemy footage can be captured without editing levels.
// Every key is a combo with RIGHT SHIFT held, so it never clashes with play.
class CameraFollow;
class HUDFader;

class TrailerEnemyShowcase final : public Script
{
    DECLARE_SCRIPT(TrailerEnemyShowcase)

public:
    explicit TrailerEnemyShowcase(GameObject* owner);

    void Start() override;
    void Update() override;
    void OnGameStop() override;

    FieldList getExposedFields() const override;

public:
    // Enemy roster (empty slots are skipped)
    PrefabRef m_enemy1;
    PrefabRef m_enemy2;
    PrefabRef m_enemy3;
    PrefabRef m_enemy4;
    PrefabRef m_enemy5;
    PrefabRef m_enemy6;
    PrefabRef m_enemy7;
    PrefabRef m_enemy8;
    PrefabRef m_enemy9;
    PrefabRef m_enemy10;
    PrefabRef m_bossPrefab;

    // Spawning
    PrefabRef m_spawnEffectPrefab;
    float m_spawnEffectLifetime = 3.0f;
    float m_spawnDelay = 0.8f;          // enemy appears this long after the effect starts
    float m_spawnDistance = 7.0f;       // distance from the players
    float m_spawnDistanceJitter = 1.5f;
    int m_groupSize = 5;
    float m_groupStagger = 0.15f;       // seconds between enemies in a group

    // Hands-free: spawn without any key presses
    bool m_spawnAllOnStart = true;      // one of every enemy type shortly after Play
    float m_startDelay = 1.5f;

    // Auto waves
    bool m_autoWaves = true;
    float m_autoWaveDelay = 2.0f;
    int m_autoWaveSize = 4;

    // Recording comfort
    bool m_playersInvulnerable = true;
    float m_stunDuration = 3.0f;
    float m_slowMotionScale = 0.35f;
    bool m_logControlsOnStart = true;

    // Close-up mode: one selected enemy at a time, framed by a cinematic camera
    bool m_closeUpMode = false;
    float m_closeUpDistance = 3.5f;     // camera distance from the enemy
    float m_closeUpHeight = 1.6f;       // camera height above the enemy's feet
    float m_closeUpLookHeight = 1.1f;   // point on the enemy the camera looks at
    float m_closeUpAngle = 35.0f;       // degrees around from the enemy's face (0 = straight on)
    float m_closeUpSharpness = 5.0f;    // camera smoothing (higher = snappier)
    float m_closeUpOrbitSpeed = 20.0f;  // degrees per second for orbit keys / auto orbit
    bool m_closeUpAutoOrbit = false;
    bool m_closeUpHideHUD = true;
    float m_closeUpFov = 0.0f;          // 0 keeps the game's FOV
    bool m_closeUpAutoFrame = true;     // measure the enemy and fit its whole body in shot
    float m_closeUpPadding = 1.35f;     // extra room around the enemy when auto framing (1 = tight)
    float m_closeUpRaise = 0.5f;        // auto framing: camera height above the enemy's middle
    bool m_closeUpInPlace = true;       // enemy animates (walk, attack, stun, die) without leaving its spot
    float m_closeUpBringDistance = 1.8f;// how close "bring players" puts Death and Lyriel to the enemy
    bool m_closeUpFaceCamera = true;    // enemy spawns and stays facing the camera
    ComponentRef<Transform> m_closeUpLight;  // a light that follows above the enemy (optional)
    float m_closeUpLightHeight = 1.2f;  // metres above the top of the enemy
    float m_closeUpLightForward = 1.5f; // metres from the enemy towards the camera (front lighting)
    bool m_closeUpUseStage = true;      // spawn at this object's position; its Y rotation sets the camera side
    bool m_closeUpParkPlayers = true;   // move Death and Lyriel behind the camera on every spawn
    float m_closeUpParkDistance = 9.0f; // how far from the enemy (towards/behind the camera) they are parked

private:
    struct PendingSpawn
    {
        bool isFocus = false;
        AssetId prefab;
        Vector3 position = Vector3::Zero;
        float yawDegrees = 0.0f;
        float timer = 0.0f;
        bool effectPlayed = false;
    };

    void buildRoster();
    void handleInput();
    bool comboPressed(KeyCode key);

    void selectNext(int direction);
    void spawnSelected(int count);
    void spawnOneOfEach();
    void spawnBoss();
    void spawnRandomWave(int count);
    void queueSpawn(const AssetId& prefab, int slotIndex, int slotCount, float extraDelay);
    void updatePendingSpawns(float realDeltaTime);

    void killAllEnemies();
    void removeAllEnemies();
    void stunAllEnemies();
    void toggleSlowMotion();
    void toggleAutoWaves();
    void setPlayersInvulnerable(bool invulnerable);
    void updateAutoWaves(float realDeltaTime);

    std::vector<GameObject*> findLiveEnemies() const;
    bool getPlayersCenter(Vector3& outCenter) const;
    Vector3 pickSpawnPosition(const Vector3& center, int slotIndex, int slotCount) const;
    const char* rosterName(int index) const;
    void logControls() const;

    // Close-up mode
    void handleCloseUpInput(float deltaTime);
    bool comboHeld(KeyCode key) const;
    void spawnFocusEnemy();
    void removeFocusEnemy();
    bool isFocusEnemyAlive() const;
    void setCloseUpCamera(bool active);
    void updateCloseUpCamera(float deltaTime);
    void measureFocusEnemy();
    void applyInPlace();
    void bringPlayersToFocus();
    void updateCloseUpLight();
    bool getStage(Vector3& outPosition, float& outCameraYaw) const;
    void parkPlayers(const Vector3& stagePosition);
    void placePlayersAround(const Vector3& point, float yawDegrees);
    void tauntFocusEnemy();

    std::vector<const PrefabRef*> m_roster;
    std::vector<PendingSpawn> m_pending;
    std::unordered_map<KeyCode, bool> m_previousKeyStates;
    ParticleLifecycle::TimedParticleTracker m_effectTracker;

    int m_selectedIndex = 0;
    bool m_slowMotionActive = false;
    bool m_invulnerabilityApplied = false;
    float m_autoWaveTimer = 0.0f;
    float m_startTimer = 0.0f;
    bool m_startSpawnDone = false;

    // Close-up state
    GameObject* m_focusEnemy = nullptr;
    int m_focusFramesSinceSpawn = 0;
    bool m_focusSeenInScene = false;
    bool m_focusPending = false;
    float m_orbitYaw = 0.0f;            // world yaw of the camera around the enemy, degrees
    float m_zoomDistance = 3.5f;
    float m_zoomScale = 1.0f;           // Up/Down keys scale the framed distance
    float m_focusMinY = 0.0f;           // enemy bounds relative to its root, grown while it animates
    float m_focusMaxY = 1.8f;
    float m_focusRadius = 0.5f;
    bool m_focusMeasured = false;
    Vector3 m_focusAnchor = Vector3::Zero;   // where the enemy spawned; pinned there when in place
    float m_focusFacingYaw = 0.0f;           // direction the enemy faced on arrival (towards the players)
    bool m_focusSpeedLocked = false;
    float m_focusOriginalSpeed = 0.0f;
    float m_focusOriginalTurnSpeed = 0.0f;
    bool m_focusTaunted = false;
    bool m_closeUpCameraActive = false;
    bool m_cameraSnapNext = true;
    Vector3 m_lastFocusPosition = Vector3::Zero;
    float m_savedFov = 0.0f;
    CameraFollow* m_cameraFollow = nullptr;
};
