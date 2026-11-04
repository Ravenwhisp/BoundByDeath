#include "pch.h"
#include "TrailerEnemyShowcase.h"

#include "KeyCode.h"
#include "Damageable.h"
#include "EnemyDamageable.h"
#include "EnemyBaseController.h"
#include "EnemyDetectionAggro.h"
#include "CameraFollow.h"
#include "HUDFader.h"
#include "ShadowExecution.h"
#include "ReaperGauge.h"
#include "HealthPickup.h"

#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <string>

IMPLEMENT_SCRIPT_FIELDS(TrailerEnemyShowcase,
    SERIALIZED_ASSET_REF(m_enemy1, "Enemy 1", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_enemy2, "Enemy 2", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_enemy3, "Enemy 3", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_enemy4, "Enemy 4", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_enemy5, "Enemy 5", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_enemy6, "Enemy 6", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_enemy7, "Enemy 7", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_enemy8, "Enemy 8", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_enemy9, "Enemy 9", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_enemy10, "Enemy 10", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_bossPrefab, "Boss Prefab", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_spawnEffectPrefab, "Spawn Effect Prefab", AssetType::PREFAB),
    SERIALIZED_FLOAT(m_spawnEffectLifetime, "Spawn Effect Lifetime", 0.1f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_spawnDelay, "Spawn Delay After Effect", 0.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_spawnDistance, "Spawn Distance From Players", 1.0f, 30.0f, 0.1f),
    SERIALIZED_FLOAT(m_spawnDistanceJitter, "Spawn Distance Jitter", 0.0f, 10.0f, 0.1f),
    SERIALIZED_INT(m_groupSize, "Group Size"),
    SERIALIZED_FLOAT(m_groupStagger, "Group Stagger", 0.0f, 2.0f, 0.01f),
    SERIALIZED_BOOL(m_spawnAllOnStart, "Spawn All On Start"),
    SERIALIZED_FLOAT(m_startDelay, "Start Delay", 0.0f, 10.0f, 0.1f),
    SERIALIZED_BOOL(m_autoWaves, "Auto Waves"),
    SERIALIZED_FLOAT(m_autoWaveDelay, "Auto Wave Delay", 0.0f, 30.0f, 0.1f),
    SERIALIZED_INT(m_autoWaveSize, "Auto Wave Size"),
    SERIALIZED_BOOL(m_playersInvulnerable, "Players Invulnerable"),
    SERIALIZED_FLOAT(m_stunDuration, "Stun Duration", 0.1f, 20.0f, 0.1f),
    SERIALIZED_FLOAT(m_slowMotionScale, "Slow Motion Scale", 0.05f, 1.0f, 0.01f),
    SERIALIZED_BOOL(m_logControlsOnStart, "Log Controls On Start"),
    SERIALIZED_BOOL(m_removeHealthDrops, "Remove Health Drops"),
    SERIALIZED_BOOL(m_closeUpMode, "Close-up Mode"),
    SERIALIZED_FLOAT(m_closeUpDistance, "Close-up Distance", 1.0f, 15.0f, 0.05f),
    SERIALIZED_FLOAT(m_closeUpHeight, "Close-up Camera Height", -1.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_closeUpLookHeight, "Close-up Look Height", 0.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_closeUpAngle, "Close-up Angle", -180.0f, 180.0f, 1.0f),
    SERIALIZED_FLOAT(m_closeUpSharpness, "Close-up Smoothing", 0.5f, 30.0f, 0.1f),
    SERIALIZED_FLOAT(m_closeUpOrbitSpeed, "Close-up Orbit Speed", 1.0f, 180.0f, 1.0f),
    SERIALIZED_BOOL(m_closeUpAutoOrbit, "Close-up Auto Orbit"),
    SERIALIZED_BOOL(m_closeUpHideHUD, "Close-up Hide HUD"),
    SERIALIZED_FLOAT(m_closeUpFov, "Close-up FOV (0 = keep)", 0.0f, 120.0f, 0.5f),
    SERIALIZED_BOOL(m_closeUpAutoFrame, "Close-up Auto Frame"),
    SERIALIZED_FLOAT(m_closeUpPadding, "Close-up Framing Padding", 1.0f, 4.0f, 0.05f),
    SERIALIZED_FLOAT(m_closeUpRaise, "Close-up Camera Raise", -2.0f, 5.0f, 0.05f),
    SERIALIZED_BOOL(m_closeUpInPlace, "Close-up In Place"),
    SERIALIZED_FLOAT(m_closeUpBringDistance, "Close-up Bring Players Distance", 0.5f, 10.0f, 0.1f),
    SERIALIZED_BOOL(m_closeUpFaceCamera, "Close-up Face Camera"),
    SERIALIZED_COMPONENT_REF(m_closeUpLight, "Close-up Light", ComponentType::TRANSFORM),
    SERIALIZED_FLOAT(m_closeUpLightHeight, "Close-up Light Height", -2.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_closeUpLightForward, "Close-up Light Forward", -10.0f, 10.0f, 0.05f),
    SERIALIZED_BOOL(m_closeUpUseStage, "Close-up Use Stage (this object)"),
    SERIALIZED_BOOL(m_closeUpParkPlayers, "Close-up Park Players Out Of Shot"),
    SERIALIZED_FLOAT(m_closeUpParkDistance, "Close-up Park Distance", 2.0f, 40.0f, 0.5f),
    SERIALIZED_BOOL(m_executionMode, "Shadow Execution Mode"),
    SERIALIZED_INT(m_executionEnemyCount, "Execution Enemy Count"),
    SERIALIZED_FLOAT(m_executionRingRadius, "Execution Ring Radius", 1.0f, 20.0f, 0.1f),
    SERIALIZED_FLOAT(m_executionRingGap, "Execution Ring Gap (camera side)", 0.0f, 300.0f, 5.0f),
    SERIALIZED_BOOL(m_executionLethal, "Execution Kills Enemies"),
    SERIALIZED_BOOL(m_executionHoldEnemies, "Execution Hold Enemies In Place"),
    SERIALIZED_BOOL(m_executionAutoLoop, "Execution Auto Loop"),
    SERIALIZED_FLOAT(m_executionTriggerDelay, "Execution Trigger Delay", 0.0f, 20.0f, 0.1f),
    SERIALIZED_FLOAT(m_executionLoopDelay, "Execution Loop Delay", 0.0f, 20.0f, 0.1f),
    SERIALIZED_FLOAT(m_executionSlowMotion, "Execution Slow Motion (1 = off)", 0.05f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_executionSlowTail, "Execution Slow Motion Tail", 0.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_executionCamDistance, "Execution Camera Distance", 2.0f, 40.0f, 0.1f),
    SERIALIZED_FLOAT(m_executionCamHeight, "Execution Camera Height", 0.0f, 30.0f, 0.1f),
    SERIALIZED_FLOAT(m_executionLookHeight, "Execution Look Height", 0.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_executionPushIn, "Execution Camera Push-in", 0.2f, 2.0f, 0.01f),
    SERIALIZED_FLOAT(m_executionOrbitSpeed, "Execution Camera Drift Speed", -60.0f, 60.0f, 0.5f),
    SERIALIZED_BOOL(m_executionDrift, "Execution Camera Drift")
)

namespace
{
    constexpr float kPi = 3.14159265358979f;
    constexpr int kMaxSpawnsPerPress = 20;

    // Matches the "Enemy Type" value saved on every enemy prefab (2). The inspector labels it
    // "EnemyGround", but in NavAgentProfile index 2 is PlayerDash, so this is what enemies really path with.
    constexpr NavAgentProfile kEnemyNavProfile = static_cast<NavAgentProfile>(2);

    float randomRange(float minValue, float maxValue)
    {
        const float t = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        return minValue + (maxValue - minValue) * t;
    }
}

TrailerEnemyShowcase::TrailerEnemyShowcase(GameObject* owner)
    : Script(owner)
{
}

void TrailerEnemyShowcase::Start()
{
    buildRoster();

    m_selectedIndex = 0;
    m_slowMotionActive = false;
    m_invulnerabilityApplied = false;
    m_autoWaveTimer = 0.0f;
    m_startTimer = 0.0f;
    m_startSpawnDone = false;
    m_pending.clear();

    m_focusEnemy = nullptr;
    m_focusPending = false;
    m_zoomDistance = m_closeUpDistance;
    m_closeUpCameraActive = false;
    m_cameraFollow = nullptr;

    m_ringEnemies.clear();
    m_executionPhase = ExecutionPhase::Idle;
    m_executionSlowApplied = false;
    m_executionPick = -1;
    m_shadowExecution = nullptr;

    if (m_closeUpMode || m_executionMode)
    {
        // Close-up and Shadow Execution recording drive their own spawns, so the crowd features stay off.
        m_spawnAllOnStart = false;
        m_autoWaves = false;
    }

    Debug::log("[TrailerEnemyShowcase] Started. %d enemy types loaded, spawn effect %s, navmesh %s.",
        static_cast<int>(m_roster.size()),
        m_spawnEffectPrefab.m_id.isValid() ? "OK" : "missing",
        NavigationAPI::hasNavMesh() ? "OK" : "missing");

    if (m_roster.empty())
    {
        Debug::warn("[TrailerEnemyShowcase] No enemy prefabs loaded. Check the Enemy slots on the 'Trailer Enemy Showcase' object.");
    }

    if (m_logControlsOnStart)
    {
        logControls();
    }
}

void TrailerEnemyShowcase::OnGameStop()
{
    Time::setTimeScale(1.0f);
    m_slowMotionActive = false;
    setCloseUpCamera(false);
    m_pending.clear();
    m_effectTracker.clear();
    m_ringEnemies.clear();
    m_executionSlowApplied = false;
    m_executionPhase = ExecutionPhase::Idle;
}

void TrailerEnemyShowcase::Update()
{
    const float deltaTime = Time::getDeltaTime();

    // Players are set invulnerable on the first frame so their own Start has already run.
    if (!m_invulnerabilityApplied)
    {
        setPlayersInvulnerable(m_playersInvulnerable);
        m_invulnerabilityApplied = true;
    }

    if (m_removeHealthDrops)
    {
        removeHealthDrops();
    }

    if (m_executionMode)
    {
        handleExecutionInput(deltaTime);

        // First take sets itself up after the start delay.
        if (!m_startSpawnDone)
        {
            m_startTimer += deltaTime;
            if (m_startTimer >= m_startDelay)
            {
                m_startSpawnDone = true;
                setCloseUpCamera(true);
                setupExecution();
            }
        }

        m_effectTracker.update(deltaTime);
        updatePendingSpawns(deltaTime);
        updateExecution(deltaTime);
        updateExecutionCamera(deltaTime);
        return;
    }

    if (m_closeUpMode)
    {
        handleCloseUpInput(deltaTime);

        // First enemy appears on its own after the start delay.
        if (!m_startSpawnDone)
        {
            m_startTimer += deltaTime;
            if (m_startTimer >= m_startDelay)
            {
                m_startSpawnDone = true;
                setCloseUpCamera(true);
                spawnFocusEnemy();
            }
        }

        m_effectTracker.update(deltaTime);
        updatePendingSpawns(deltaTime);
        updateCloseUpCamera(deltaTime);
        return;
    }

    handleInput();

    if (m_spawnAllOnStart && !m_startSpawnDone)
    {
        m_startTimer += deltaTime;
        if (m_startTimer >= m_startDelay)
        {
            m_startSpawnDone = true;
            Debug::log("[TrailerEnemyShowcase] Spawning one of every enemy type.");
            spawnOneOfEach();
        }
    }

    m_effectTracker.update(deltaTime);
    updatePendingSpawns(deltaTime);
    updateAutoWaves(deltaTime);
}

void TrailerEnemyShowcase::buildRoster()
{
    m_roster.clear();

    const PrefabRef* slots[] =
    {
        &m_enemy1, &m_enemy2, &m_enemy3, &m_enemy4, &m_enemy5,
        &m_enemy6, &m_enemy7, &m_enemy8, &m_enemy9, &m_enemy10
    };

    for (const PrefabRef* slot : slots)
    {
        if (slot->m_id.isValid())
        {
            m_roster.push_back(slot);
        }
    }
}

bool TrailerEnemyShowcase::comboPressed(KeyCode key)
{
    const bool isDown = Input::isKeyDown(KeyCode::RightShift) && Input::isKeyDown(key);
    const bool wasDown = m_previousKeyStates[key];
    m_previousKeyStates[key] = isDown;
    return isDown && !wasDown;
}

void TrailerEnemyShowcase::handleInput()
{
    if (comboPressed(KeyCode::Right)) selectNext(1);
    if (comboPressed(KeyCode::Left))  selectNext(-1);
    if (comboPressed(KeyCode::Up))    spawnSelected(1);
    if (comboPressed(KeyCode::Down))  spawnSelected(m_groupSize);
    if (comboPressed(KeyCode::Num3))  spawnOneOfEach();
    if (comboPressed(KeyCode::J))     spawnBoss();
    if (comboPressed(KeyCode::K))     killAllEnemies();
    if (comboPressed(KeyCode::R))     removeAllEnemies();
    if (comboPressed(KeyCode::L))     stunAllEnemies();
    if (comboPressed(KeyCode::O))     toggleSlowMotion();
    if (comboPressed(KeyCode::I))     toggleAutoWaves();

    if (comboPressed(KeyCode::U))
    {
        m_playersInvulnerable = !m_playersInvulnerable;
        setPlayersInvulnerable(m_playersInvulnerable);
    }
}

void TrailerEnemyShowcase::selectNext(int direction)
{
    if (m_roster.empty())
    {
        return;
    }

    const int count = static_cast<int>(m_roster.size());
    m_selectedIndex = (m_selectedIndex + direction + count) % count;

    Debug::log("[TrailerEnemyShowcase] Selected enemy slot %d of %d: %s", m_selectedIndex + 1, count, rosterName(m_selectedIndex));
}

void TrailerEnemyShowcase::spawnSelected(int count)
{
    if (m_roster.empty())
    {
        Debug::warn("[TrailerEnemyShowcase] Nothing to spawn: no enemy prefabs assigned.");
        return;
    }

    if (count < 1) count = 1;
    if (count > kMaxSpawnsPerPress) count = kMaxSpawnsPerPress;

    const AssetId& prefab = m_roster[m_selectedIndex]->m_id;
    for (int i = 0; i < count; ++i)
    {
        queueSpawn(prefab, i, count, m_groupStagger * static_cast<float>(i));
    }
}

void TrailerEnemyShowcase::spawnOneOfEach()
{
    const int count = static_cast<int>(m_roster.size());
    for (int i = 0; i < count; ++i)
    {
        queueSpawn(m_roster[i]->m_id, i, count, m_groupStagger * static_cast<float>(i));
    }
}

void TrailerEnemyShowcase::spawnBoss()
{
    if (!m_bossPrefab.m_id.isValid())
    {
        Debug::warn("[TrailerEnemyShowcase] Boss Prefab is not assigned.");
        return;
    }

    queueSpawn(m_bossPrefab.m_id, 0, 1, 0.0f);
}

void TrailerEnemyShowcase::spawnRandomWave(int count)
{
    if (m_roster.empty())
    {
        return;
    }

    if (count < 1) count = 1;
    if (count > kMaxSpawnsPerPress) count = kMaxSpawnsPerPress;

    for (int i = 0; i < count; ++i)
    {
        const int pick = std::rand() % static_cast<int>(m_roster.size());
        queueSpawn(m_roster[pick]->m_id, i, count, m_groupStagger * static_cast<float>(i));
    }
}

void TrailerEnemyShowcase::queueSpawn(const AssetId& prefab, int slotIndex, int slotCount, float extraDelay)
{
    Vector3 center;
    if (!getPlayersCenter(center))
    {
        // No players in the scene: spawn around this object instead.
        center = TransformAPI::getGlobalPosition(GameObjectAPI::getTransform(getOwner()));
    }

    PendingSpawn spawn;
    spawn.prefab = prefab;
    spawn.position = pickSpawnPosition(center, slotIndex, slotCount);

    // Face the players on arrival. The enemy AI keeps turning towards its target afterwards.
    const Vector3 toCenter = center - spawn.position;
    spawn.yawDegrees = std::atan2(toCenter.x, toCenter.z) * (180.0f / kPi);

    // Negative timer = waiting for its stagger slot before the effect starts.
    spawn.timer = -extraDelay;

    m_pending.push_back(spawn);
}

void TrailerEnemyShowcase::updatePendingSpawns(float deltaTime)
{
    for (size_t i = m_pending.size(); i-- > 0;)
    {
        PendingSpawn& spawn = m_pending[i];
        spawn.timer += deltaTime;

        if (spawn.timer < 0.0f)
        {
            continue;
        }

        if (!spawn.effectPlayed)
        {
            spawn.effectPlayed = true;

            if (m_spawnEffectPrefab.m_id.isValid())
            {
                ParticleLifecycle::spawnOneShotTimed(m_effectTracker, m_spawnEffectPrefab.m_id, spawn.position, Vector3::Zero, m_spawnEffectLifetime);
            }
        }

        const float delay = m_spawnEffectPrefab.m_id.isValid() ? m_spawnDelay : 0.0f;
        if (spawn.timer < delay)
        {
            continue;
        }

        GameObject* enemy = GameObjectAPI::instantiatePrefab(spawn.prefab, spawn.position, Vector3(0.0f, spawn.yawDegrees, 0.0f));
        if (enemy && spawn.isFocus)
        {
            m_focusEnemy = enemy;
            m_focusPending = false;
            m_focusFramesSinceSpawn = 0;
            m_focusSeenInScene = false;
            m_focusAnchor = spawn.position;
            m_focusFacingYaw = spawn.yawDegrees;
            m_focusSpeedLocked = false;
            m_focusTaunted = false;
        }

        if (enemy && spawn.isRing)
        {
            RingEnemy ringEnemy;
            ringEnemy.object = enemy;
            ringEnemy.anchor = spawn.position;
            m_ringEnemies.push_back(ringEnemy);
        }

        if (enemy)
        {
            Debug::log("[TrailerEnemyShowcase] Spawned %s at (%.1f, %.1f, %.1f)", GameObjectAPI::getName(enemy),
                spawn.position.x, spawn.position.y, spawn.position.z);
        }
        else
        {
            Debug::warn("[TrailerEnemyShowcase] Failed to spawn an enemy prefab at (%.1f, %.1f, %.1f).",
                spawn.position.x, spawn.position.y, spawn.position.z);
        }

        m_pending.erase(m_pending.begin() + static_cast<std::ptrdiff_t>(i));
    }
}

void TrailerEnemyShowcase::killAllEnemies()
{
    const std::vector<GameObject*> enemies = findLiveEnemies();
    for (GameObject* enemy : enemies)
    {
        // forceKill skips barrier protection, so shielded variants die too.
        if (EnemyDamageable* enemyDamageable = GameObjectAPI::findScript<EnemyDamageable>(enemy))
        {
            enemyDamageable->forceKill();
        }
        else if (Damageable* damageable = GameObjectAPI::findScript<Damageable>(enemy))
        {
            damageable->kill();
        }
    }

    Debug::log("[TrailerEnemyShowcase] Killed %d enemies", static_cast<int>(enemies.size()));
}

void TrailerEnemyShowcase::removeAllEnemies()
{
    m_pending.clear();

    // Includes dying enemies so a retake starts from an empty arena.
    const std::vector<GameObject*> objects = SceneAPI::findAllGameObjectsByTag(Tag::ENEMY);
    int removed = 0;
    for (GameObject* object : objects)
    {
        if (GameObjectAPI::findScript<EnemyBaseController>(object) == nullptr)
        {
            continue;
        }

        GameObjectAPI::removeGameObject(object);
        ++removed;
    }

    // The focus enemy (if it was in the scene) went with the sweep above; never remove it twice.
    m_focusEnemy = nullptr;
    m_ringEnemies.clear();

    Debug::log("[TrailerEnemyShowcase] Removed %d enemies", removed);
}

void TrailerEnemyShowcase::stunAllEnemies()
{
    const std::vector<GameObject*> enemies = findLiveEnemies();
    for (GameObject* enemy : enemies)
    {
        if (EnemyBaseController* controller = GameObjectAPI::findScript<EnemyBaseController>(enemy))
        {
            controller->useStun(m_stunDuration);
        }
    }

    Debug::log("[TrailerEnemyShowcase] Stunned %d enemies for %.1fs", static_cast<int>(enemies.size()), m_stunDuration);
}

void TrailerEnemyShowcase::toggleSlowMotion()
{
    m_slowMotionActive = !m_slowMotionActive;
    Time::setTimeScale(m_slowMotionActive ? m_slowMotionScale : 1.0f);

    Debug::log("[TrailerEnemyShowcase] Slow motion %s", m_slowMotionActive ? "ON" : "OFF");
}

void TrailerEnemyShowcase::toggleAutoWaves()
{
    m_autoWaves = !m_autoWaves;
    m_autoWaveTimer = 0.0f;

    Debug::log("[TrailerEnemyShowcase] Auto waves %s", m_autoWaves ? "ON" : "OFF");
}

void TrailerEnemyShowcase::setPlayersInvulnerable(bool invulnerable)
{
    const std::vector<GameObject*> players = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER);
    for (GameObject* player : players)
    {
        if (Damageable* damageable = GameObjectAPI::findScript<Damageable>(player))
        {
            damageable->setInvulnerable(invulnerable);
        }
    }

    Debug::log("[TrailerEnemyShowcase] Players invulnerable: %s", invulnerable ? "ON" : "OFF");
}

void TrailerEnemyShowcase::removeHealthDrops()
{
    // Dying enemies drop health orbs (EnemyDeathState); they'd pile up over repeated takes.
    const std::vector<GameObject*> drops = SceneAPI::findAllGameObjectsWithScript<HealthPickup>();
    for (GameObject* drop : drops)
    {
        if (drop != nullptr)
        {
            GameObjectAPI::removeGameObject(drop);
        }
    }
}

void TrailerEnemyShowcase::updateAutoWaves(float deltaTime)
{
    if (!m_autoWaves || !m_pending.empty())
    {
        m_autoWaveTimer = 0.0f;
        return;
    }

    if (!findLiveEnemies().empty())
    {
        m_autoWaveTimer = 0.0f;
        return;
    }

    m_autoWaveTimer += deltaTime;
    if (m_autoWaveTimer < m_autoWaveDelay)
    {
        return;
    }

    m_autoWaveTimer = 0.0f;
    spawnRandomWave(m_autoWaveSize);
}

std::vector<GameObject*> TrailerEnemyShowcase::findLiveEnemies() const
{
    std::vector<GameObject*> result;

    const std::vector<GameObject*> objects = SceneAPI::findAllGameObjectsByTag(Tag::ENEMY);
    for (GameObject* object : objects)
    {
        // Only real enemies: crystals and other ENEMY-tagged props have no controller.
        if (GameObjectAPI::findScript<EnemyBaseController>(object) == nullptr)
        {
            continue;
        }

        const Damageable* damageable = GameObjectAPI::findScript<Damageable>(object);
        if (damageable && damageable->isDead())
        {
            continue;
        }

        result.push_back(object);
    }

    return result;
}

bool TrailerEnemyShowcase::getPlayersCenter(Vector3& outCenter) const
{
    const std::vector<GameObject*> players = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER);
    if (players.empty())
    {
        return false;
    }

    Vector3 sum = Vector3::Zero;
    int count = 0;
    for (GameObject* player : players)
    {
        if (Transform* transform = GameObjectAPI::getTransform(player))
        {
            sum += TransformAPI::getGlobalPosition(transform);
            ++count;
        }
    }

    if (count == 0)
    {
        return false;
    }

    outCenter = sum / static_cast<float>(count);
    return true;
}

Vector3 TrailerEnemyShowcase::pickSpawnPosition(const Vector3& center, int slotIndex, int slotCount) const
{
    // Spread a group evenly around the players, with a random starting angle.
    const float baseAngle = randomRange(0.0f, 2.0f * kPi);
    const float step = slotCount > 0 ? (2.0f * kPi) / static_cast<float>(slotCount) : 0.0f;
    const float angle = baseAngle + step * static_cast<float>(slotIndex);
    const float distance = m_spawnDistance + randomRange(-m_spawnDistanceJitter, m_spawnDistanceJitter);

    const Vector3 desired = center + Vector3(std::sin(angle) * distance, 0.0f, std::cos(angle) * distance);

    if (!NavigationAPI::hasNavMesh())
    {
        return desired;
    }

    const Vector3 searchExtents(3.0f, 5.0f, 3.0f);

    // Snap to the navmesh, and make sure the enemy can actually walk to the players.
    Vector3 sampled;
    if (NavigationAPI::samplePosition(desired, sampled, searchExtents, kEnemyNavProfile) &&
        NavigationAPI::canReachTarget(sampled, center, searchExtents, kEnemyNavProfile))
    {
        return sampled;
    }

    Vector3 fallback;
    if (NavigationAPI::findRandomReachablePointAround(center, m_spawnDistance, fallback, searchExtents, 12, kEnemyNavProfile))
    {
        return fallback;
    }

    return desired;
}

const char* TrailerEnemyShowcase::rosterName(int index) const
{
    static const char* fallback = "(assigned prefab)";
    if (index < 0 || index >= static_cast<int>(m_roster.size()))
    {
        return fallback;
    }

    // The inspector slot number is the easiest way to identify a prefab at runtime.
    static std::string label;
    const PrefabRef* const slots[] =
    {
        &m_enemy1, &m_enemy2, &m_enemy3, &m_enemy4, &m_enemy5,
        &m_enemy6, &m_enemy7, &m_enemy8, &m_enemy9, &m_enemy10
    };

    for (int i = 0; i < 10; ++i)
    {
        if (slots[i] == m_roster[index])
        {
            label = "inspector slot 'Enemy " + std::to_string(i + 1) + "'";
            return label.c_str();
        }
    }

    return fallback;
}

void TrailerEnemyShowcase::logControls() const
{
    if (m_executionMode)
    {
        Debug::log("[TrailerEnemyShowcase] SHADOW EXECUTION MODE. Hold RIGHT SHIFT and press:");
        Debug::log("  J  Shadow Execution now              |  L  new take (players + ring of enemies)");
        Debug::log("  Left / Right  enemy type (or a mix)  |  3  auto loop on/off");
        Debug::log("  Up / Down (hold)  zoom in / out      |  1 / 2 (hold)  orbit left / right");
        Debug::log("  I  hold enemies in place on/off      |  Tab  trailer camera on/off");
        Debug::log("  Space  camera drift on/off (still shot)");
        Debug::log("  K  kill all   O  slow motion   R  remove all   U  invulnerability");
        Debug::log("[TrailerEnemyShowcase] Auto loop is %s.", m_executionAutoLoop ? "ON (no keys needed)" : "OFF");
        return;
    }

    if (m_closeUpMode)
    {
        Debug::log("[TrailerEnemyShowcase] CLOSE-UP MODE. Hold RIGHT SHIFT and press:");
        Debug::log("  Left / Right  previous / next enemy  |  Space  respawn (replay entrance)");
        Debug::log("  Up / Down (hold)  zoom in / out      |  1 / 2 (hold)  orbit left / right");
        Debug::log("  3  auto orbit on/off                 |  Tab  close-up camera on/off");
        Debug::log("  I  in place on/off                   |  J  bring players to the enemy (triggers attacks)");
        Debug::log("  K  kill (death anim)   L  stun   O  slow motion   R  remove   U  invulnerability");
        Debug::log("[TrailerEnemyShowcase] %d enemy types loaded.", static_cast<int>(m_roster.size()));
        return;
    }

    Debug::log("[TrailerEnemyShowcase] Hold RIGHT SHIFT and press:");
    Debug::log("  Left / Right  choose enemy type   |  Up  spawn one   |  Down  spawn a group");
    Debug::log("  3  one of every type              |  J  spawn boss");
    Debug::log("  K  kill all (death anims)         |  R  remove all instantly (retake)");
    Debug::log("  L  stun all                       |  O  slow motion on/off");
    Debug::log("  I  auto waves on/off              |  U  player invulnerability on/off");
    Debug::log("[TrailerEnemyShowcase] %d enemy types loaded.", static_cast<int>(m_roster.size()));
}

// ---------------------------------------------------------------------------
// Close-up mode
// ---------------------------------------------------------------------------

bool TrailerEnemyShowcase::comboHeld(KeyCode key) const
{
    return Input::isKeyDown(KeyCode::RightShift) && Input::isKeyDown(key);
}

void TrailerEnemyShowcase::handleCloseUpInput(float deltaTime)
{
    if (comboPressed(KeyCode::Right)) { selectNext(1);  spawnFocusEnemy(); }
    if (comboPressed(KeyCode::Left))  { selectNext(-1); spawnFocusEnemy(); }
    if (comboPressed(KeyCode::Space)) spawnFocusEnemy();
    if (comboPressed(KeyCode::Tab))   setCloseUpCamera(!m_closeUpCameraActive);
    if (comboPressed(KeyCode::K))     killAllEnemies();
    if (comboPressed(KeyCode::L))     stunAllEnemies();
    if (comboPressed(KeyCode::O))     toggleSlowMotion();

    if (comboPressed(KeyCode::R))
    {
        removeAllEnemies();
        m_focusEnemy = nullptr;
        m_focusPending = false;
    }

    // I, not Enter: Enter can press whichever editor button has keyboard focus (e.g. Pause).
    if (comboPressed(KeyCode::I))
    {
        m_closeUpInPlace = !m_closeUpInPlace;
        Debug::log("[TrailerEnemyShowcase] In place %s", m_closeUpInPlace ? "ON" : "OFF");
    }

    if (comboPressed(KeyCode::J)) bringPlayersToFocus();

    if (comboPressed(KeyCode::Num3))
    {
        m_closeUpAutoOrbit = !m_closeUpAutoOrbit;
        Debug::log("[TrailerEnemyShowcase] Auto orbit %s", m_closeUpAutoOrbit ? "ON" : "OFF");
    }

    if (comboPressed(KeyCode::U))
    {
        m_playersInvulnerable = !m_playersInvulnerable;
        setPlayersInvulnerable(m_playersInvulnerable);
    }

    // Held keys: smooth zoom and orbit while recording.
    const float zoomRate = 0.6f; // fraction of the framed distance per second
    if (comboHeld(KeyCode::Up))   m_zoomScale -= zoomRate * deltaTime;
    if (comboHeld(KeyCode::Down)) m_zoomScale += zoomRate * deltaTime;
    if (m_zoomScale < 0.3f) m_zoomScale = 0.3f;
    if (m_zoomScale > 3.0f) m_zoomScale = 3.0f;

    if (comboHeld(KeyCode::Num1)) m_orbitYaw -= m_closeUpOrbitSpeed * deltaTime;
    if (comboHeld(KeyCode::Num2)) m_orbitYaw += m_closeUpOrbitSpeed * deltaTime;
    if (m_closeUpAutoOrbit)       m_orbitYaw += m_closeUpOrbitSpeed * deltaTime;
}

void TrailerEnemyShowcase::spawnFocusEnemy()
{
    if (m_roster.empty())
    {
        Debug::warn("[TrailerEnemyShowcase] Nothing to spawn: no enemy prefabs assigned.");
        return;
    }

    // Clears the current enemy and any leftovers (e.g. spiders a Summoner called in), each removed
    // exactly once, so every enemy gets a clean shot.
    removeAllEnemies();

    // Drop any focus spawn still waiting for its effect, so quick key presses don't stack enemies.
    for (size_t i = m_pending.size(); i-- > 0;)
    {
        if (m_pending[i].isFocus)
        {
            m_pending.erase(m_pending.begin() + static_cast<std::ptrdiff_t>(i));
        }
    }

    Vector3 center;
    if (!getPlayersCenter(center))
    {
        center = TransformAPI::getGlobalPosition(GameObjectAPI::getTransform(getOwner()));
    }

    PendingSpawn spawn;
    spawn.isFocus = true;
    spawn.prefab = m_roster[m_selectedIndex]->m_id;

    Vector3 stagePosition;
    float stageCameraYaw = 0.0f;
    if (m_closeUpUseStage && getStage(stagePosition, stageCameraYaw))
    {
        // Fixed stage: same spot and same camera side every time, chosen in the editor.
        spawn.position = stagePosition;
        m_orbitYaw = stageCameraYaw;
        spawn.yawDegrees = m_orbitYaw; // facing the lens (atan2(x, z) convention, same as the AI)

        if (m_closeUpParkPlayers)
        {
            parkPlayers(stagePosition);
        }
    }
    else
    {
        spawn.position = pickSpawnPosition(center, 0, 1);

        const Vector3 toCenter = center - spawn.position;
        const float towardsPlayersYaw = std::atan2(toCenter.x, toCenter.z) * (180.0f / kPi);

        // The camera sits a little to the side of the line to the players so they don't block the shot.
        m_orbitYaw = towardsPlayersYaw + m_closeUpAngle;

        // Yaw uses atan2(x, z), the same convention the enemy controllers turn with, so facing the
        // camera's direction from the enemy means looking straight down the lens.
        spawn.yawDegrees = m_closeUpFaceCamera ? m_orbitYaw : towardsPlayersYaw;
    }

    spawn.timer = 0.0f;

    m_pending.push_back(spawn);
    m_focusPending = true;

    m_zoomScale = 1.0f;
    m_focusMeasured = false;
    m_focusMinY = 0.0f;
    m_focusMaxY = 1.8f;
    m_focusRadius = 0.5f;
    m_lastFocusPosition = spawn.position;
    m_cameraSnapNext = true;

    Debug::log("[TrailerEnemyShowcase] Close-up: showing %s", rosterName(m_selectedIndex));
}

void TrailerEnemyShowcase::removeFocusEnemy()
{
    if (m_focusEnemy && SceneAPI::containsGameObject(m_focusEnemy))
    {
        GameObjectAPI::removeGameObject(m_focusEnemy);
    }

    m_focusEnemy = nullptr;
}

bool TrailerEnemyShowcase::isFocusEnemyAlive() const
{
    if (m_focusEnemy == nullptr)
    {
        return false;
    }

    if (SceneAPI::containsGameObject(m_focusEnemy))
    {
        return true;
    }

    // A freshly spawned object only joins the scene list a frame later.
    return !m_focusSeenInScene && m_focusFramesSinceSpawn < 10;
}

void TrailerEnemyShowcase::setCloseUpCamera(bool active)
{
    GameObject* camera = SceneAPI::getDefaultCameraGameObject();
    if (camera == nullptr)
    {
        if (active)
        {
            Debug::warn("[TrailerEnemyShowcase] No default camera found for close-up mode.");
        }
        m_closeUpCameraActive = false;
        return;
    }

    if (m_cameraFollow == nullptr)
    {
        m_cameraFollow = GameObjectAPI::findScript<CameraFollow>(camera);
    }

    if (active == m_closeUpCameraActive)
    {
        return;
    }

    m_closeUpCameraActive = active;

    // The game camera writes its transform every frame; pause it while we drive the shot.
    if (m_cameraFollow)
    {
        m_cameraFollow->setFollowEnabled(!active);
    }

    if (CameraComponent* cameraComponent = CameraAPI::getCameraComponent(camera))
    {
        if (active)
        {
            m_savedFov = CameraAPI::getFov(cameraComponent);
            if (m_closeUpFov > 0.0f)
            {
                CameraAPI::setFov(cameraComponent, m_closeUpFov);
            }
        }
        else if (m_savedFov > 0.0f)
        {
            CameraAPI::setFov(cameraComponent, m_savedFov);
        }
    }

    if (m_closeUpHideHUD)
    {
        const std::vector<GameObject*> faders = SceneAPI::findAllGameObjectsWithScript<HUDFader>();
        for (GameObject* faderObject : faders)
        {
            if (HUDFader* fader = GameObjectAPI::findScript<HUDFader>(faderObject))
            {
                fader->fadeTo(active ? 0.0f : 1.0f, 0.4f);
            }
        }
    }

    m_cameraSnapNext = true;
    Debug::log("[TrailerEnemyShowcase] Close-up camera %s", active ? "ON" : "OFF");
}

void TrailerEnemyShowcase::updateCloseUpCamera(float deltaTime)
{
    if (m_focusEnemy)
    {
        ++m_focusFramesSinceSpawn;
        if (SceneAPI::containsGameObject(m_focusEnemy))
        {
            m_focusSeenInScene = true;
        }
    }

    if (isFocusEnemyAlive())
    {
        applyInPlace();

        // Give it a target straight away, whatever its detection radius, so it walks, attacks and summons.
        if (!m_focusTaunted && m_focusFramesSinceSpawn > 2)
        {
            tauntFocusEnemy();
        }

        if (Transform* enemyTransform = GameObjectAPI::getTransform(m_focusEnemy))
        {
            m_lastFocusPosition = TransformAPI::getGlobalPosition(enemyTransform);
        }

        // Wait a couple of frames so the skeleton has been posed by the animator.
        if (m_focusFramesSinceSpawn > 2)
        {
            measureFocusEnemy();
        }
    }
    else if (m_focusEnemy != nullptr)
    {
        // Enemy finished dying and was removed: hold the shot on where it fell.
        m_focusEnemy = nullptr;
    }

    updateCloseUpLight();

    if (!m_closeUpCameraActive)
    {
        return;
    }

    GameObject* camera = SceneAPI::getDefaultCameraGameObject();
    Transform* cameraTransform = camera ? GameObjectAPI::getTransform(camera) : nullptr;
    if (cameraTransform == nullptr)
    {
        return;
    }

    // Settings are read every frame so inspector tweaks show up live while recording.
    float lookHeight = m_closeUpLookHeight;
    float cameraHeight = m_closeUpHeight;
    float distance = m_closeUpDistance;

    if (m_closeUpAutoFrame && m_focusMeasured)
    {
        // The engine's FOV is horizontal; derive the vertical one for a 16:9 frame.
        float horizontalFov = m_closeUpFov;
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

        const float aspect = 16.0f / 9.0f;
        const float tanHalfH = std::tan(horizontalFov * 0.5f * (kPi / 180.0f));
        const float tanHalfV = tanHalfH / aspect;

        const float enemyHeight = m_focusMaxY - m_focusMinY;
        const float halfHeight = 0.5f * enemyHeight * m_closeUpPadding;
        const float halfWidth = m_focusRadius * m_closeUpPadding;

        const float fitHeight = halfHeight / tanHalfV;
        const float fitWidth = halfWidth / tanHalfH;
        distance = (fitHeight > fitWidth ? fitHeight : fitWidth) + m_focusRadius;

        lookHeight = 0.5f * (m_focusMinY + m_focusMaxY);
        cameraHeight = lookHeight + m_closeUpRaise;
    }

    distance *= m_zoomScale;

    const float yawRadians = m_orbitYaw * (kPi / 180.0f);
    const Vector3 lookTarget = m_lastFocusPosition + Vector3(0.0f, lookHeight, 0.0f);
    const Vector3 desiredPosition = m_lastFocusPosition + Vector3(
        std::sin(yawRadians) * distance,
        cameraHeight,
        std::cos(yawRadians) * distance);

    Vector3 newPosition = desiredPosition;
    if (!m_cameraSnapNext)
    {
        // Frame-rate independent smoothing; clamp dt so a hitch doesn't make the camera jump.
        const float dt = deltaTime > 0.05f ? 0.05f : deltaTime;
        const float blend = 1.0f - std::exp(-m_closeUpSharpness * dt);
        const Vector3 current = TransformAPI::getGlobalPosition(cameraTransform);
        newPosition = current + (desiredPosition - current) * blend;
    }
    m_cameraSnapNext = false;

    TransformAPI::setGlobalPosition(cameraTransform, newPosition);
    TransformAPI::lookAt(cameraTransform, lookTarget);
}

namespace
{
    // Walks a transform tree and grows the bounds (relative to rootPosition) with every node.
    void growBounds(Transform* node, const Vector3& rootPosition, float& minY, float& maxY, float& radius, int depth)
    {
        if (node == nullptr || depth > 64)
        {
            return;
        }

        const Vector3 position = TransformAPI::getGlobalPosition(node) - rootPosition;
        if (position.y < minY) minY = position.y;
        if (position.y > maxY) maxY = position.y;

        const float horizontal = std::sqrt(position.x * position.x + position.z * position.z);
        if (horizontal > radius) radius = horizontal;

        const int childCount = TransformAPI::getChildCount(node);
        for (int i = 0; i < childCount; ++i)
        {
            growBounds(TransformAPI::getChild(node, i), rootPosition, minY, maxY, radius, depth + 1);
        }
    }
}

void TrailerEnemyShowcase::measureFocusEnemy()
{
    Transform* root = GameObjectAPI::getTransform(m_focusEnemy);
    if (root == nullptr)
    {
        return;
    }

    // Only the skeleton: health bars and stun anchors float above the head and would inflate the frame.
    Transform* bones = TransformAPI::findChildByName(root, "bones");
    if (bones == nullptr)
    {
        return;
    }

    const Vector3 rootPosition = TransformAPI::getGlobalPosition(root);

    float minY = 0.0f;
    float maxY = 0.0f;
    float radius = 0.0f;
    growBounds(bones, rootPosition, minY, maxY, radius, 0);

    // Bones stop at the joints, so add a little for the head/helmet and weapons beyond the last bone.
    maxY += 0.15f * (maxY - minY);

    if (maxY - minY < 0.3f)
    {
        return; // skeleton not posed yet
    }

    if (radius > 4.0f) radius = 4.0f;
    if (maxY > 8.0f) maxY = 8.0f;

    if (!m_focusMeasured)
    {
        m_focusMinY = minY;
        m_focusMaxY = maxY;
        m_focusRadius = radius;
        m_focusMeasured = true;
        Debug::log("[TrailerEnemyShowcase] Close-up framing: enemy is %.2f m tall, %.2f m wide.", maxY - minY, radius * 2.0f);
        return;
    }

    // Only grow, so a raised weapon in an attack widens the shot instead of making it pump in and out.
    if (minY < m_focusMinY) m_focusMinY = minY;
    if (maxY > m_focusMaxY) m_focusMaxY = maxY;
    if (radius > m_focusRadius) m_focusRadius = radius;
}

void TrailerEnemyShowcase::applyInPlace()
{
    EnemyBaseController* controller = GameObjectAPI::findScript<EnemyBaseController>(m_focusEnemy);
    if (controller == nullptr)
    {
        return;
    }

    if (!m_closeUpInPlace)
    {
        if (m_focusSpeedLocked)
        {
            controller->m_moveSpeed = m_focusOriginalSpeed;
            controller->m_turnSpeedDegrees = m_focusOriginalTurnSpeed;
            m_focusSpeedLocked = false;
        }
        return;
    }

    // Zero speed keeps the AI in its chase state, so the walk cycle plays, but it never travels.
    // Zero turn speed stops it swinging round towards the players, so it keeps facing the lens.
    if (!m_focusSpeedLocked && m_focusFramesSinceSpawn > 1)
    {
        m_focusOriginalSpeed = controller->m_moveSpeed;
        m_focusOriginalTurnSpeed = controller->m_turnSpeedDegrees;
        controller->m_moveSpeed = 0.0f;
        if (m_closeUpFaceCamera)
        {
            controller->m_turnSpeedDegrees = 0.0f;
        }
        m_focusSpeedLocked = true;
    }

    // Lunges, charges, somersaults and teleports move the enemy directly; pin it back every frame.
    if (Transform* enemyTransform = GameObjectAPI::getTransform(m_focusEnemy))
    {
        TransformAPI::setGlobalPosition(enemyTransform, m_focusAnchor);

        if (m_closeUpFaceCamera)
        {
            TransformAPI::setGlobalRotationEuler(enemyTransform, Vector3(0.0f, m_focusFacingYaw, 0.0f));
        }
    }
}

void TrailerEnemyShowcase::bringPlayersToFocus()
{
    if (!isFocusEnemyAlive())
    {
        Debug::warn("[TrailerEnemyShowcase] No enemy on screen to bring the players to.");
        return;
    }

    // Attacks only check distance, so when the enemy is held facing the camera the players can stand
    // behind it, inside melee range and out of the shot. Otherwise stand them where it is looking.
    const float yawRadians = m_focusFacingYaw * (kPi / 180.0f);
    const Vector3 forward(std::sin(yawRadians), 0.0f, std::cos(yawRadians));
    const Vector3 right(std::cos(yawRadians), 0.0f, -std::sin(yawRadians));
    const float side = (m_closeUpFaceCamera && m_closeUpInPlace) ? -1.0f : 1.0f;
    const Vector3 standPoint = m_focusAnchor + forward * (m_closeUpBringDistance * side);

    (void)right;
    placePlayersAround(standPoint, m_focusFacingYaw);
    Debug::log("[TrailerEnemyShowcase] Players moved next to the enemy.");
}

void TrailerEnemyShowcase::updateCloseUpLight()
{
    Transform* lightTransform = m_closeUpLight.getReferencedComponent();
    if (lightTransform == nullptr)
    {
        return;
    }

    // Above the enemy's head and a little towards the camera, so the face and chest catch the light.
    const float yawRadians = m_orbitYaw * (kPi / 180.0f);
    const Vector3 towardsCamera(std::sin(yawRadians), 0.0f, std::cos(yawRadians));
    const float top = m_focusMeasured ? m_focusMaxY : 2.0f;

    const Vector3 lightPosition = m_lastFocusPosition
        + Vector3(0.0f, top + m_closeUpLightHeight, 0.0f)
        + towardsCamera * m_closeUpLightForward;

    TransformAPI::setGlobalPosition(lightTransform, lightPosition);
}

bool TrailerEnemyShowcase::getStage(Vector3& outPosition, float& outCameraYaw) const
{
    Transform* stageTransform = GameObjectAPI::getTransform(getOwner());
    if (stageTransform == nullptr)
    {
        return false;
    }

    outPosition = TransformAPI::getGlobalPosition(stageTransform);
    outCameraYaw = TransformAPI::getGlobalEulerDegrees(stageTransform).y;

    // Snap to the walkable floor so the enemy stands on the ground even if the object floats a little.
    Vector3 sampled;
    if (NavigationAPI::hasNavMesh() &&
        NavigationAPI::samplePosition(outPosition, sampled, Vector3(3.0f, 6.0f, 3.0f), kEnemyNavProfile))
    {
        outPosition = sampled;
    }

    return true;
}

void TrailerEnemyShowcase::parkPlayers(const Vector3& stagePosition)
{
    // Behind the camera, on the same line: out of frame, and the enemy chases (in place) towards the lens.
    const float yawRadians = m_orbitYaw * (kPi / 180.0f);
    const Vector3 towardsCamera(std::sin(yawRadians), 0.0f, std::cos(yawRadians));
    placePlayersAround(stagePosition + towardsCamera * m_closeUpParkDistance, m_orbitYaw);
}

void TrailerEnemyShowcase::placePlayersAround(const Vector3& point, float yawDegrees)
{
    const float yawRadians = yawDegrees * (kPi / 180.0f);
    const Vector3 right(std::cos(yawRadians), 0.0f, -std::sin(yawRadians));

    const std::vector<GameObject*> players = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER);
    float lateral = -0.7f;
    for (GameObject* player : players)
    {
        Transform* playerTransform = GameObjectAPI::getTransform(player);
        if (playerTransform == nullptr)
        {
            continue;
        }

        Vector3 target = point + right * lateral;
        lateral += 1.4f;

        Vector3 sampled;
        if (NavigationAPI::hasNavMesh() &&
            NavigationAPI::samplePosition(target, sampled, Vector3(3.0f, 6.0f, 3.0f), NavAgentProfile::PlayerNormal))
        {
            target = sampled;
        }

        TransformAPI::setGlobalPosition(playerTransform, target);
    }
}

void TrailerEnemyShowcase::tauntFocusEnemy()
{
    m_focusTaunted = true;
    tauntEnemy(m_focusEnemy, false);
}

void TrailerEnemyShowcase::tauntEnemy(GameObject* enemy, bool nearestPlayer)
{
    EnemyDetectionAggro* aggro = enemy ? GameObjectAPI::findScript<EnemyDetectionAggro>(enemy) : nullptr;
    if (aggro == nullptr)
    {
        return;
    }

    Transform* target = nullptr;
    const std::vector<GameObject*> players = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER);

    if (nearestPlayer)
    {
        // Split the ring between Death and Lyriel so both are being threatened in the shot.
        Transform* enemyTransform = GameObjectAPI::getTransform(enemy);
        const Vector3 enemyPosition = enemyTransform ? TransformAPI::getGlobalPosition(enemyTransform) : Vector3::Zero;
        float bestDistance = 1.0e9f;
        for (GameObject* player : players)
        {
            Transform* playerTransform = GameObjectAPI::getTransform(player);
            if (playerTransform == nullptr)
            {
                continue;
            }

            const float distance = Vector3::Distance(enemyPosition, TransformAPI::getGlobalPosition(playerTransform));
            if (distance < bestDistance)
            {
                bestDistance = distance;
                target = playerTransform;
            }
        }
    }
    else
    {
        // Prefer Death (the taunt character), fall back to any player.
        for (GameObject* player : players)
        {
            const char* name = GameObjectAPI::getName(player);
            if (name && std::string(name) == "Death")
            {
                target = GameObjectAPI::getTransform(player);
                break;
            }
        }
    }
    if (target == nullptr && !players.empty())
    {
        target = GameObjectAPI::getTransform(players[0]);
    }

    // A taunt ignores detection range (the Summoner only sees 8 m), so the enemy always has someone
    // to walk at, shoot at or summon against. Long duration: it lasts for the whole take.
    if (target)
    {
        aggro->applyTaunt(target, 100000.0f);
    }
}

// ---------------------------------------------------------------------------
// Shadow Execution mode
// ---------------------------------------------------------------------------

ShadowExecution* TrailerEnemyShowcase::findShadowExecution()
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

void TrailerEnemyShowcase::fillReaperGauge()
{
    const std::vector<GameObject*> holders = SceneAPI::findAllGameObjectsWithScript<ReaperGauge>();
    if (holders.empty())
    {
        return;
    }

    ReaperGauge* gauge = GameObjectAPI::findScript<ReaperGauge>(holders[0]);
    if (gauge == nullptr)
    {
        return;
    }

    // Same as the Level2Cheats fill, with a cap in case a config change makes a mark worth nothing.
    for (int i = 0; i < 200 && !gauge->isFull(); ++i)
    {
        gauge->onMarkExploited();
    }

    gauge->updateUI();
}

const char* TrailerEnemyShowcase::executionPickName() const
{
    if (m_executionPick < 0)
    {
        return "a mix of every type";
    }

    return rosterName(m_executionPick);
}

void TrailerEnemyShowcase::handleExecutionInput(float deltaTime)
{
    if (comboPressed(KeyCode::J)) triggerExecution();
    if (comboPressed(KeyCode::L)) setupExecution();

    const int typeCount = static_cast<int>(m_roster.size());
    if (typeCount > 0)
    {
        // -1 is "mix", then every roster slot in order.
        if (comboPressed(KeyCode::Right))
        {
            m_executionPick = ((m_executionPick + 1 + 1) % (typeCount + 1)) - 1;
            Debug::log("[TrailerEnemyShowcase] Execution enemies: %s", executionPickName());
            setupExecution();
        }
        if (comboPressed(KeyCode::Left))
        {
            m_executionPick = ((m_executionPick + 1 - 1 + typeCount + 1) % (typeCount + 1)) - 1;
            Debug::log("[TrailerEnemyShowcase] Execution enemies: %s", executionPickName());
            setupExecution();
        }
    }

    if (comboPressed(KeyCode::Num3))
    {
        m_executionAutoLoop = !m_executionAutoLoop;
        m_executionTimer = 0.0f;
        Debug::log("[TrailerEnemyShowcase] Execution auto loop %s", m_executionAutoLoop ? "ON" : "OFF");
    }

    if (comboPressed(KeyCode::I))
    {
        m_executionHoldEnemies = !m_executionHoldEnemies;
        Debug::log("[TrailerEnemyShowcase] Hold enemies in place %s", m_executionHoldEnemies ? "ON" : "OFF");
    }

    if (comboPressed(KeyCode::Tab)) setCloseUpCamera(!m_closeUpCameraActive);
    if (comboPressed(KeyCode::K))   killAllEnemies();
    if (comboPressed(KeyCode::O))   toggleSlowMotion();

    if (comboPressed(KeyCode::Space))
    {
        m_executionDrift = !m_executionDrift;
        Debug::log("[TrailerEnemyShowcase] Camera drift %s", m_executionDrift ? "ON" : "OFF (camera holds still)");
    }

    if (comboPressed(KeyCode::R))
    {
        removeAllEnemies();
        m_executionPhase = ExecutionPhase::Idle;
    }

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

    if (comboHeld(KeyCode::Num1)) m_orbitYaw -= m_closeUpOrbitSpeed * deltaTime;
    if (comboHeld(KeyCode::Num2)) m_orbitYaw += m_closeUpOrbitSpeed * deltaTime;
}

void TrailerEnemyShowcase::setupExecution()
{
    if (m_roster.empty())
    {
        Debug::warn("[TrailerEnemyShowcase] Nothing to spawn: no enemy prefabs assigned.");
        return;
    }

    ShadowExecution* shadowExecution = findShadowExecution();
    if (shadowExecution != nullptr && shadowExecution->isActive())
    {
        return; // let the current move finish
    }

    // Clean slate: previous ring, corpses, summoned spiders and anything still waiting to spawn.
    removeAllEnemies();

    if (m_executionSlowApplied)
    {
        Time::setTimeScale(m_slowMotionActive ? m_slowMotionScale : 1.0f);
        m_executionSlowApplied = false;
    }

    Vector3 center;
    float cameraYaw = 0.0f;
    if (!getStage(center, cameraYaw))
    {
        if (!getPlayersCenter(center))
        {
            return;
        }
    }

    m_orbitYaw = cameraYaw;
    m_executionCenter = center;
    m_lastFocusPosition = center; // the key light follows this
    m_focusMeasured = false;

    // Death and Lyriel side by side in the middle, facing the camera. The move is centred between them.
    placePlayersAround(center, m_orbitYaw);
    const std::vector<GameObject*> players = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER);
    for (GameObject* player : players)
    {
        if (Transform* playerTransform = GameObjectAPI::getTransform(player))
        {
            TransformAPI::setGlobalRotationEuler(playerTransform, Vector3(0.0f, m_orbitYaw, 0.0f));
        }
    }

    // Keep the ring inside the move's reach, or the outer enemies never get hit.
    float radius = m_executionRingRadius;
    if (shadowExecution != nullptr)
    {
        const float maxRadius = shadowExecution->getMaxRadius();
        if (maxRadius > 0.5f && radius > maxRadius * 0.85f)
        {
            radius = maxRadius * 0.85f;
            Debug::warn("[TrailerEnemyShowcase] Ring radius reduced to %.1f m to stay inside Shadow Execution's reach (%.1f m).", radius, maxRadius);
        }
    }
    else
    {
        Debug::warn("[TrailerEnemyShowcase] No ShadowExecution script in this scene (it lives on GameManager). The ring will spawn but J won't fire.");
    }

    int count = m_executionEnemyCount;
    if (count < 1) count = 1;
    if (count > kMaxSpawnsPerPress) count = kMaxSpawnsPerPress;

    float gap = m_executionRingGap;
    if (gap < 0.0f) gap = 0.0f;
    if (gap > 300.0f) gap = 300.0f;
    const float arc = 360.0f - gap;

    const int typeCount = static_cast<int>(m_roster.size());
    const int mixOffset = std::rand() % typeCount;
    const Vector3 searchExtents(2.0f, 5.0f, 2.0f);

    for (int i = 0; i < count; ++i)
    {
        // Angles use the same convention as the camera, so the open part of the ring faces the lens.
        const float angleDegrees = (gap < 1.0f)
            ? m_orbitYaw + 180.0f + 360.0f * static_cast<float>(i) / static_cast<float>(count)
            : m_orbitYaw + gap * 0.5f + arc * (static_cast<float>(i) + 0.5f) / static_cast<float>(count);
        const float angle = angleDegrees * (kPi / 180.0f);

        // Alternate the depth a little so the ring reads as a crowd, not a clock face.
        const float distance = radius + ((i % 2) == 0 ? 0.0f : 0.6f);
        Vector3 position = center + Vector3(std::sin(angle) * distance, 0.0f, std::cos(angle) * distance);

        Vector3 sampled;
        if (NavigationAPI::hasNavMesh() && NavigationAPI::samplePosition(position, sampled, searchExtents, kEnemyNavProfile))
        {
            position = sampled;
        }

        const int pick = m_executionPick >= 0 ? m_executionPick : (mixOffset + i) % typeCount;

        PendingSpawn spawn;
        spawn.isRing = true;
        spawn.prefab = m_roster[pick]->m_id;
        spawn.position = position;
        const Vector3 toCenter = center - position;
        spawn.yawDegrees = std::atan2(toCenter.x, toCenter.z) * (180.0f / kPi);
        spawn.timer = -m_groupStagger * static_cast<float>(i);
        m_pending.push_back(spawn);
    }

    // Full gauge straight away, so the "ready" glow and colour grade are already on when the take starts.
    fillReaperGauge();

    m_executionPhase = ExecutionPhase::Gathering;
    m_executionTimer = 0.0f;
    m_executionPush = 0.0f;
    m_zoomScale = 1.0f;
    m_cameraSnapNext = true;

    Debug::log("[TrailerEnemyShowcase] Shadow Execution take: %d enemies (%s), ring %.1f m.", count, executionPickName(), radius);
}

void TrailerEnemyShowcase::triggerExecution()
{
    ShadowExecution* shadowExecution = findShadowExecution();
    if (shadowExecution == nullptr)
    {
        Debug::warn("[TrailerEnemyShowcase] Can't fire: no ShadowExecution script in this scene.");
        return;
    }

    if (shadowExecution->isActive())
    {
        return;
    }

    // The move spends the gauge, so top it up first (it also decays if a take runs long).
    fillReaperGauge();

    if (!shadowExecution->triggerNow())
    {
        Debug::warn("[TrailerEnemyShowcase] Shadow Execution didn't start (check Death and Lyriel are in the scene).");
        return;
    }

    if (m_executionSlowMotion < 0.999f)
    {
        Time::setTimeScale(m_executionSlowMotion);
        m_executionSlowApplied = true;
    }

    m_executionPhase = ExecutionPhase::Executing;
    m_executionTimer = 0.0f;
    m_executionPush = 0.0f;

    Debug::log("[TrailerEnemyShowcase] Shadow Execution!");
}

void TrailerEnemyShowcase::updateExecution(float deltaTime)
{
    updateRingEnemies();

    ShadowExecution* shadowExecution = findShadowExecution();

    switch (m_executionPhase)
    {
    case ExecutionPhase::Gathering:
        // Count from the moment the last enemy has arrived, so every take gets the same build-up.
        if (!m_pending.empty())
        {
            m_executionTimer = 0.0f;
            break;
        }

        m_executionTimer += deltaTime;
        if (m_executionAutoLoop && m_executionTimer >= m_executionTriggerDelay)
        {
            triggerExecution();
        }
        break;

    case ExecutionPhase::Executing:
    {
        m_executionTimer += deltaTime;
        const float duration = shadowExecution ? shadowExecution->getExecutionDuration() : 1.0f;
        m_executionPush = duration > 0.0f ? (m_executionTimer / duration) : 1.0f;
        if (m_executionPush > 1.0f) m_executionPush = 1.0f;

        if (shadowExecution == nullptr || !shadowExecution->isActive())
        {
            m_executionPhase = ExecutionPhase::Aftermath;
            m_executionTimer = 0.0f;
            m_executionPush = 1.0f;
        }
        break;
    }

    case ExecutionPhase::Aftermath:
        m_executionTimer += deltaTime;

        if (m_executionSlowApplied && m_executionTimer >= m_executionSlowTail)
        {
            Time::setTimeScale(m_slowMotionActive ? m_slowMotionScale : 1.0f);
            m_executionSlowApplied = false;
        }

        if (m_executionAutoLoop && m_executionTimer >= m_executionLoopDelay)
        {
            setupExecution();
        }
        break;

    case ExecutionPhase::Idle:
    default:
        break;
    }
}

void TrailerEnemyShowcase::updateRingEnemies()
{
    for (size_t i = m_ringEnemies.size(); i-- > 0;)
    {
        RingEnemy& ringEnemy = m_ringEnemies[i];
        ++ringEnemy.frames;

        // New objects only join the scene list a frame later; after that, gone means it died and was cleaned up.
        if (!SceneAPI::containsGameObject(ringEnemy.object))
        {
            if (ringEnemy.seenInScene || ringEnemy.frames > 10)
            {
                m_ringEnemies.erase(m_ringEnemies.begin() + static_cast<std::ptrdiff_t>(i));
            }
            continue;
        }
        ringEnemy.seenInScene = true;

        if (!ringEnemy.prepared && ringEnemy.frames > 2)
        {
            prepareRingEnemy(ringEnemy);
        }

        if (!ringEnemy.prepared)
        {
            continue;
        }

        EnemyBaseController* controller = GameObjectAPI::findScript<EnemyBaseController>(ringEnemy.object);
        const Damageable* damageable = GameObjectAPI::findScript<Damageable>(ringEnemy.object);
        const bool dead = damageable != nullptr && damageable->isDead();
        if (controller == nullptr || dead)
        {
            continue;
        }

        if (m_executionHoldEnemies)
        {
            // Walk cycle and attacks play, but nobody leaves the ring or walks in front of the players.
            if (!ringEnemy.speedLocked)
            {
                ringEnemy.originalSpeed = controller->m_moveSpeed;
                controller->m_moveSpeed = 0.0f;
                ringEnemy.speedLocked = true;
            }

            if (Transform* enemyTransform = GameObjectAPI::getTransform(ringEnemy.object))
            {
                TransformAPI::setGlobalPosition(enemyTransform, ringEnemy.anchor);
            }
        }
        else if (ringEnemy.speedLocked)
        {
            controller->m_moveSpeed = ringEnemy.originalSpeed;
            ringEnemy.speedLocked = false;
        }
    }
}

void TrailerEnemyShowcase::prepareRingEnemy(RingEnemy& ringEnemy)
{
    ringEnemy.prepared = true;

    // Always has a target (whatever its detection radius), so it menaces the players during the build-up.
    tauntEnemy(ringEnemy.object, true);

    if (!m_executionLethal)
    {
        return;
    }

    EnemyDamageable* damageable = GameObjectAPI::findScript<EnemyDamageable>(ringEnemy.object);
    if (damageable == nullptr || damageable->isDead() || damageable->getMaxHp() <= 0.0f)
    {
        return;
    }

    // Below its own execution threshold (some enemies have a multiplier), so the move finishes it.
    float threshold = 0.4f;
    if (ShadowExecution* shadowExecution = findShadowExecution())
    {
        threshold = shadowExecution->getExecutionThresholdPercent(damageable);
    }
    if (threshold <= 0.0f)
    {
        return;
    }

    const float targetHp = damageable->getMaxHp() * threshold * 0.75f;
    const float currentHp = damageable->getCurrentHp();
    if (currentHp > targetHp)
    {
        damageable->takeDamage(currentHp - targetHp);
    }
}

void TrailerEnemyShowcase::updateExecutionCamera(float deltaTime)
{
    updateCloseUpLight();

    if (!m_closeUpCameraActive)
    {
        return;
    }

    GameObject* camera = SceneAPI::getDefaultCameraGameObject();
    Transform* cameraTransform = camera ? GameObjectAPI::getTransform(camera) : nullptr;
    if (cameraTransform == nullptr)
    {
        return;
    }

    // Slow drift keeps the wide shot alive; it slows down with the slow motion too.
    if (m_executionDrift)
    {
        m_orbitYaw += m_executionOrbitSpeed * deltaTime;
    }

    // Smoothstep push-in over the move, held afterwards for the aftermath.
    const float push = m_executionPush;
    const float eased = push * push * (3.0f - 2.0f * push);
    const float scale = m_zoomScale * (1.0f + (m_executionPushIn - 1.0f) * eased);

    const float distance = m_executionCamDistance * scale;
    const float height = m_executionCamHeight * scale;

    const float yawRadians = m_orbitYaw * (kPi / 180.0f);
    const Vector3 lookTarget = m_executionCenter + Vector3(0.0f, m_executionLookHeight, 0.0f);
    const Vector3 desiredPosition = m_executionCenter + Vector3(
        std::sin(yawRadians) * distance,
        height,
        std::cos(yawRadians) * distance);

    Vector3 newPosition = desiredPosition;
    if (!m_cameraSnapNext)
    {
        const float dt = deltaTime > 0.05f ? 0.05f : deltaTime;
        const float blend = 1.0f - std::exp(-m_closeUpSharpness * dt);
        const Vector3 current = TransformAPI::getGlobalPosition(cameraTransform);
        newPosition = current + (desiredPosition - current) * blend;
    }
    m_cameraSnapNext = false;

    TransformAPI::setGlobalPosition(cameraTransform, newPosition);
    TransformAPI::lookAt(cameraTransform, lookTarget);
}

IMPLEMENT_SCRIPT(TrailerEnemyShowcase)
