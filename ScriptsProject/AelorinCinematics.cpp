#include "pch.h"
#include "AelorinCinematics.h"

#include "AelorinBossController.h"
#include "AelorinVFX.h"

#include "CameraShake.h"
#include "CameraTransitionEvent.h"
#include "CameraTransitionController.h"
#include "CameraFollow.h"
#include "MusicManager.h"

#include <algorithm>

IMPLEMENT_SCRIPT_FIELDS(AelorinCinematics,
    FIELD_GROUP_COLLAPSE("Encounter",
        SERIALIZED_COMPONENT_REF(m_encounterCinematic, "Encounter Cinematic", ComponentType::TRANSFORM),
        SERIALIZED_COMPONENT_REF(m_healthBarRoot, "Health Bar Root", ComponentType::TRANSFORM),
        SERIALIZED_COMPONENT_REF(m_arenaCameraAnchor, "Arena Camera Anchor", ComponentType::TRANSFORM),
        SERIALIZED_COMPONENT_REF_VECTOR(m_hideDuringCinematics, "Hide During Cinematics", ComponentType::TRANSFORM),
        SERIALIZED_VEC3(m_arenaCameraOffset, "Arena Camera Offset"),
        SERIALIZED_COMPONENT_REF(m_combatCameraAnchor, "Combat Camera Anchor", ComponentType::TRANSFORM),
        SERIALIZED_VEC3(m_combatCameraCloseOffset, "Combat Camera Close Offset"),
        SERIALIZED_VEC3(m_combatCameraFarOffset, "Combat Camera Far Offset"),
        SERIALIZED_FLOAT(m_combatCameraZoomStartDistance, "Combat Zoom Start Distance", 0.0f, 100.0f, 0.5f),
        SERIALIZED_FLOAT(m_combatCameraZoomMaxDistance, "Combat Zoom Max Distance", 0.0f, 100.0f, 0.5f),
        SERIALIZED_FLOAT(m_combatFocusSmoothSpeed, "Combat Focus Smooth Speed", 0.1f, 20.0f, 0.1f),
        SERIALIZED_FLOAT(m_combatZoomSmoothSpeed, "Combat Zoom Smooth Speed", 0.1f, 20.0f, 0.1f),
        SERIALIZED_VEC3(m_playersShotOffset, "Players Shot Offset"),
        SERIALIZED_FLOAT(m_encounterRoarDelay, "Encounter Roar Delay", 0.0f, 30.0f, 0.05f),
        SERIALIZED_BOOL(m_setMusicOnEncounter, "Set Boss Music On Encounter"),
        SERIALIZED_BOOL(m_debugReplayEncounter, "DEBUG - Replay Encounter")
    ),

    FIELD_GROUP_COLLAPSE("Phase Transition",
        SERIALIZED_COMPONENT_REF(m_phaseTransitionCinematic, "Phase Transition Cinematic", ComponentType::TRANSFORM),
        SERIALIZED_FLOAT(m_phaseSwapDelay, "Phase Swap Delay", 0.0f, 10.0f, 0.05f),
        SERIALIZED_VEC3(m_phaseShotOffset, "Phase Shot Offset"),
        SERIALIZED_VEC3(m_phaseWideShotOffset, "Phase Wide Shot Direction"),
        SERIALIZED_FLOAT(m_phaseWideBaseDistance, "Phase Wide Base Distance", 0.0f, 200.0f, 0.1f),
        SERIALIZED_FLOAT(m_phaseWideDistancePerUnit, "Phase Wide Distance Per Unit", 0.0f, 10.0f, 0.05f),
        SERIALIZED_FLOAT(m_phaseWideMaxDistance, "Phase Wide Max Distance", 0.0f, 200.0f, 0.1f)
    ),

    FIELD_GROUP_COLLAPSE("Defeat",
        SERIALIZED_COMPONENT_REF(m_defeatCinematic, "Defeat Cinematic", ComponentType::TRANSFORM),
        SERIALIZED_FLOAT(m_defeatDelay, "Defeat Shot Delay", 0.0f, 10.0f, 0.05f),
        SERIALIZED_VEC3(m_defeatCamOffset, "Defeat Cam Offset"),
        SERIALIZED_STRING(m_sceneAfterDefeat, "Scene After Defeat")
    )
)

AelorinCinematics::AelorinCinematics(GameObject* owner)
    : Script(owner)
{
}

void AelorinCinematics::Start()
{
    m_controller = GameObjectAPI::findScript<AelorinBossController>(getOwner());
    m_vfx = GameObjectAPI::findScript<AelorinVFX>(getOwner());

    GameObject* cameraObject = SceneAPI::getDefaultCameraGameObject();
    m_cameraShake = cameraObject ? GameObjectAPI::findScript<CameraShake>(cameraObject) : nullptr;
    m_cameraTransition = cameraObject ? GameObjectAPI::findScript<CameraTransitionController>(cameraObject) : nullptr;
    m_cameraFollow = cameraObject ? GameObjectAPI::findScript<CameraFollow>(cameraObject) : nullptr;

    if (!m_controller)
    {
        Debug::error("[AelorinCinematics] AelorinBossController not found.");
    }

}

void AelorinCinematics::Update()
{
    // La barra se esconde en el primer frame, no en Start, para no depender del orden de Start
    // con AelorinUI, que resuelve ahí sus refs a los markers.
    if (!m_initialHideDone)
    {
        m_initialHideDone = true;

        if (m_encounterCinematic.getReferencedComponent() != nullptr)
        {
            showHealthBar(false);
        }
    }

    const float dt = Time::getDeltaTime();

    updateEncounter(dt);
    updatePhaseTransition(dt);
    updateDefeat(dt);

    if (m_encounterFinished && !isPlaying() && !isTransitionRunning())
    {
        updateCombatCamera();
    }
}

void AelorinCinematics::requestEncounterStart()
{
    if (m_encounterFinished || m_encounterRequested)
    {
        return;
    }

    m_encounterRequested = true;
}

void AelorinCinematics::updateEncounter(float dt)
{
    if (m_debugReplayEncounter)
    {
        m_debugReplayEncounter = false;
        playCinematic(m_encounterCinematic);
    }

    if (m_encounterFinished)
    {
        return;
    }

    if (m_encounterRequested && !m_encounterPlaying)
    {
        // La cámara deja de seguir a los jugadores justo al empezar el encuentro, para que la
        // transición vuelva ya a la vista fija de la arena.
        applyArenaCamera();
        setCinematicUIHidden(true);

        if (m_controller)
        {
            const Vector3 playersMidpoint =
                (m_controller->getLyrielPosition() + m_controller->getDeathPosition()) * 0.5f;

            positionShot(m_encounterCinematic, "Point3", playersMidpoint, m_playersShotOffset);
        }

        // El plano de apertura y el de cierre son la propia vista fija de arena, asi que se
        // derivan del mismo offset: un solo mando para toda la altura de camara.
        Transform* anchor = m_arenaCameraAnchor.getReferencedComponent();
        if (anchor != nullptr)
        {
            const Vector3 arenaCenter = TransformAPI::getGlobalPosition(anchor);
            positionShot(m_encounterCinematic, "Point1", arenaCenter, m_arenaCameraOffset);
            positionShot(m_encounterCinematic, "Point5", arenaCenter, m_arenaCameraOffset);
        }

        if (!playCinematic(m_encounterCinematic))
        {
            onEncounterFinished();
            return;
        }

        m_encounterPlaying = true;
        m_encounterTimer = 0.0f;
        m_encounterRoarPlayed = false;
    }

    if (!m_encounterPlaying)
    {
        return;
    }

    m_encounterTimer += dt;

    if (!m_encounterRoarPlayed && m_encounterTimer >= m_encounterRoarDelay)
    {
        if (m_cameraShake)
        {
            m_cameraShake->shakeRoar();
        }

        m_encounterRoarPlayed = true;
    }

    // El guard de 0.1s existe porque la transicion no esta activa todavia el primer frame.
    if (m_encounterTimer > 0.1f && !isTransitionRunning())
    {
        m_encounterPlaying = false;
        onEncounterFinished();
    }
}

void AelorinCinematics::onEncounterFinished()
{
    m_encounterFinished = true;

    applyArenaCamera();
    setCinematicUIHidden(false);
    showHealthBar(true);

    if (m_setMusicOnEncounter)
    {
        if (MusicManager* music = MusicManager::Get())
        {
            music->SetState_FinalBoss();
        }
    }
}

void AelorinCinematics::updateCombatCamera()
{
    if (!m_cameraFollow || !m_controller)
    {
        return;
    }

    Transform* anchor = m_combatCameraAnchor.getReferencedComponent();
    if (!anchor)
    {
        return;
    }

    Transform* lyriel = m_controller->getLyrielTransform();
    Transform* death = m_controller->getDeathTransform();
    Transform* boss = GameObjectAPI::getTransform(getOwner());

    if (!lyriel || !death || !boss)
    {
        return;
    }

    const Vector3 lyrielPosition = TransformAPI::getGlobalPosition(lyriel);
    const Vector3 deathPosition = TransformAPI::getGlobalPosition(death);
    const Vector3 bossPosition = TransformAPI::getGlobalPosition(boss);

    // Center between both players and Aelorin
    const Vector3 desiredFocus = (lyrielPosition + deathPosition + bossPosition) / 3.0f;

    // Calculate how spread out the fight is

    const float lyrielDistance = (lyrielPosition - desiredFocus).Length();
    const float deathDistance = (deathPosition - desiredFocus).Length();
    const float bossDistance = (bossPosition - desiredFocus).Length();

    float maxDistance = lyrielDistance;

    if (deathDistance > maxDistance)
    {
        maxDistance = deathDistance;
    }

    if (bossDistance > maxDistance)
    {
        maxDistance = bossDistance;
    }

    // Calculate zoom

    const float zoomRange = m_combatCameraZoomMaxDistance - m_combatCameraZoomStartDistance;
    float zoomT = 0.0f;

    if (zoomRange > 0.001f)
    {
        zoomT = (maxDistance - m_combatCameraZoomStartDistance) / zoomRange;
    }

    zoomT = std::clamp(zoomT, 0.0f, 1.0f);

    // Smoothstep zoom
    zoomT = zoomT * zoomT * (3.0f - 2.0f * zoomT);

    const Vector3 desiredOffset(
        m_combatCameraCloseOffset.x + (m_combatCameraFarOffset.x - m_combatCameraCloseOffset.x) * zoomT,
        m_combatCameraCloseOffset.y + (m_combatCameraFarOffset.y - m_combatCameraCloseOffset.y) * zoomT,
        m_combatCameraCloseOffset.z + (m_combatCameraFarOffset.z - m_combatCameraCloseOffset.z) * zoomT
    );

    // First frame after a cinematic

    if (!m_combatCameraInitialized)
    {
        GameObject* cameraObject = SceneAPI::getDefaultCameraGameObject();

        Transform* cameraTransform = cameraObject ? GameObjectAPI::getTransform(cameraObject) : nullptr;
        if (!cameraTransform)
        {
            return;
        }

        const Vector3 currentCameraPosition = TransformAPI::getGlobalPosition(cameraTransform);

        m_combatCurrentFocus = desiredFocus;

        // focus + offset = current camera position
        m_combatCurrentOffset = currentCameraPosition - desiredFocus;

        TransformAPI::setGlobalPosition(anchor, m_combatCurrentFocus);

        m_cameraFollow->m_firstTarget.uid = anchor->getID();
        m_cameraFollow->m_firstTarget.component = anchor;
        m_cameraFollow->m_secondTarget.uid = anchor->getID();
        m_cameraFollow->m_secondTarget.component = anchor;
        m_cameraFollow->m_transformOffset = m_combatCurrentOffset;

        m_combatCameraInitialized = true;

        return;
    }

    // Smooth combat

    const float dt = Time::getDeltaTime();
    const float focusT = std::clamp(m_combatFocusSmoothSpeed * dt, 0.0f, 1.0f);
    const float offsetT = std::clamp(m_combatZoomSmoothSpeed * dt, 0.0f, 1.0f);

    m_combatCurrentFocus = m_combatCurrentFocus + (desiredFocus - m_combatCurrentFocus) * focusT;
    m_combatCurrentOffset = m_combatCurrentOffset + (desiredOffset - m_combatCurrentOffset) * offsetT;

    TransformAPI::setGlobalPosition(anchor, m_combatCurrentFocus);

    m_cameraFollow->m_firstTarget.uid = anchor->getID();
    m_cameraFollow->m_firstTarget.component = anchor;
    m_cameraFollow->m_secondTarget.uid = anchor->getID();
    m_cameraFollow->m_secondTarget.component = anchor;
    m_cameraFollow->m_transformOffset = m_combatCurrentOffset;
}

bool AelorinCinematics::startPhaseTransition()
{
    if (m_phasePlaying)
    {
        return true;
    }

    // Los planos se recolocan ANTES de lanzar la cinematica: el controller lee la posicion del
    // primer punto en el mismo frame en que arranca.
    const Vector3 bossPosition = getBossPosition();
    positionShot(m_phaseTransitionCinematic, "Point1", bossPosition, m_phaseShotOffset);

    if (Transform* anchor = m_arenaCameraAnchor.getReferencedComponent())
    {
        const Vector3 arenaCenter = TransformAPI::getGlobalPosition(anchor);

        // Cuanto mas lejos este el boss del centro, mas atras hay que ponerse para que los dos
        // entren en cuadro, con un tope para no acabar en la vista de toda la arena.
        const float separation = (bossPosition - arenaCenter).Length();

        float distance = m_phaseWideBaseDistance + separation * m_phaseWideDistancePerUnit;
        if (distance < m_phaseWideBaseDistance) distance = m_phaseWideBaseDistance;
        if (distance > m_phaseWideMaxDistance) distance = m_phaseWideMaxDistance;

        Vector3 direction = m_phaseWideShotOffset;
        direction.Normalize();

        positionShot(m_phaseTransitionCinematic, "Point2", (bossPosition + arenaCenter) * 0.5f,
                     direction * distance);
    }

    if (!playCinematic(m_phaseTransitionCinematic))
    {
        return false;
    }

    setCinematicUIHidden(true);

    m_phasePlaying = true;
    m_phaseTimer = 0.0f;

    if (m_controller)
    {
        m_controller->clearPath();
        m_controller->resetRepathTimer();

        // Nada de navegación mientras las acciones de la cinemática mueven al boss a mano.
        m_controller->setForcedMovementActive(false);
        m_controller->setForcedMovementBlocked(true);
    }

    return true;
}

void AelorinCinematics::performPhaseTeleportToCenter()
{
    if (m_vfx)
    {
        m_vfx->playPhase2Transition();
    }

    if (m_cameraShake)
    {
        m_cameraShake->shakeRoar();
    }

    m_phaseSwapPending = true;
    m_phaseSwapTimer = m_phaseSwapDelay;
}

void AelorinCinematics::updatePhaseTransition(float dt)
{
    if (m_phaseSwapPending)
    {
        m_phaseSwapTimer -= dt;

        if (m_phaseSwapTimer <= 0.0f)
        {
            m_phaseSwapPending = false;

            if (m_controller)
            {
                m_controller->beginPhase2();
            }
        }
    }

    if (!m_phasePlaying)
    {
        return;
    }

    m_phaseTimer += dt;

    if (m_phaseTimer > 0.1f && !isTransitionRunning())
    {
        m_phasePlaying = false;
        setCinematicUIHidden(false);

        if (m_controller)
        {
            m_controller->setForcedMovementBlocked(false);
        }

        // Red de seguridad: si la action del swap no se disparó, no dejes al boss en fase 1.
        if (m_controller && !m_controller->isPhase2())
        {
            m_phaseSwapPending = false;
            m_controller->beginPhase2();
        }
    }
}

void AelorinCinematics::updateDefeat(float dt)
{
    if (!m_defeatHandled && m_controller && m_controller->isDead())
    {
        m_defeatHandled = true;
        m_defeatPending = true;
        m_defeatPendingTimer = m_defeatDelay;
    }

    if (m_defeatPending)
    {
        m_defeatPendingTimer -= dt;

        if (m_defeatPendingTimer <= 0.0f)
        {
            m_defeatPending = false;

            if (m_cameraShake)
            {
                m_cameraShake->shakeRoar();
            }

            positionShot(m_defeatCinematic, "Point1", getBossPosition(), m_defeatCamOffset);

            if (playCinematic(m_defeatCinematic))
            {
                setCinematicUIHidden(true);
                m_defeatPlaying = true;
                m_defeatTimer = 0.0f;
            }
        }
    }

    if (!m_defeatPlaying)
    {
        return;
    }

    m_defeatTimer += dt;

    if (m_defeatTimer > 0.1f && !isTransitionRunning())
    {
        m_defeatPlaying = false;
        onDefeatFinished();
    }
}

void AelorinCinematics::positionShot(const ComponentRef<Transform>& cinematicRef, const char* pointName,
                                     const Vector3& target, const Vector3& offset)
{
    Transform* cinematicRoot = cinematicRef.getReferencedComponent();
    if (!cinematicRoot)
    {
        return;
    }

    Transform* cameraPoints = TransformAPI::findChildByName(cinematicRoot, "CameraPoints");
    if (!cameraPoints)
    {
        return;
    }

    Transform* point = TransformAPI::findChildByName(cameraPoints, pointName);
    if (!point)
    {
        return;
    }

    TransformAPI::setGlobalPosition(point, target + offset);
}

Vector3 AelorinCinematics::getBossPosition() const
{
    return TransformAPI::getGlobalPosition(GameObjectAPI::getTransform(getOwner()));
}

void AelorinCinematics::onDefeatFinished()
{
    if (m_sceneAfterDefeat.empty())
    {
        return;
    }

    SceneAPI::requestSceneChange(m_sceneAfterDefeat.c_str());
}

bool AelorinCinematics::playCinematic(const ComponentRef<Transform>& cinematicRef)
{
    Transform* cinematicTransform = cinematicRef.getReferencedComponent();
    if (!cinematicTransform)
    {
        return false;
    }

    GameObject* cinematicObject = ComponentAPI::getOwner(cinematicTransform);
    if (!cinematicObject)
    {
        return false;
    }

    CameraTransitionEvent* cinematic = GameObjectAPI::findScript<CameraTransitionEvent>(cinematicObject);
    if (!cinematic)
    {
        return false;
    }

    m_combatCameraInitialized = false;

    cinematic->play();
    return true;
}

bool AelorinCinematics::isTransitionRunning() const
{
    return m_cameraTransition != nullptr && m_cameraTransition->isTransitioning();
}

void AelorinCinematics::applyArenaCamera()
{
    Transform* anchor = m_arenaCameraAnchor.getReferencedComponent();
    if (m_cameraFollow == nullptr || anchor == nullptr)
    {
        return;
    }

    m_cameraFollow->m_firstTarget.uid = anchor->getID();
    m_cameraFollow->m_firstTarget.component = anchor;
    m_cameraFollow->m_secondTarget.uid = anchor->getID();
    m_cameraFollow->m_secondTarget.component = anchor;
    m_cameraFollow->m_transformOffset = m_arenaCameraOffset;
}

void AelorinCinematics::setCinematicUIHidden(bool hidden)
{
    for (const ComponentRef<Transform>& ref : m_hideDuringCinematics)
    {
        Transform* target = ref.getReferencedComponent();
        if (target == nullptr)
        {
            continue;
        }

        GameObject* owner = ComponentAPI::getOwner(target);
        if (owner != nullptr)
        {
            GameObjectAPI::setActive(owner, !hidden);
        }
    }
}

void AelorinCinematics::showHealthBar(bool show)
{
    Transform* healthBarRoot = m_healthBarRoot.getReferencedComponent();
    if (!healthBarRoot)
    {
        return;
    }

    GameObject* healthBarObject = ComponentAPI::getOwner(healthBarRoot);
    if (!healthBarObject)
    {
        return;
    }

    GameObjectAPI::setActive(healthBarObject, show);
}

IMPLEMENT_SCRIPT(AelorinCinematics)
