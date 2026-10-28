#include "pch.h"
#include "AelorinSound.h"

namespace
{
    const char* const k_banks[] = { "LevelBoss.bnk" };

    constexpr const char* k_event_hurt = "Play_Aelorin_Hurt";

    constexpr const char* k_event_idle_loop = "Play_Aelorin_Idle_Loop";
    constexpr const char* k_event_idle_loop_stop = "Stop_Aelorin_Idle_Loop";
    constexpr const char* k_event_phase_transition = "Play_Aelorin_PhaseTransition";
    constexpr const char* k_event_death = "Play_Aelorin_Death";

    constexpr const char* k_event_sigils_cast = "Play_Aelorin_SeekerSigils_Cast";
    constexpr const char* k_event_sigils_fall = "Play_Aelorin_SeekerSigils_Fall";
    constexpr const char* k_event_sigils_impact = "Play_Aelorin_SeekerSigils_Impact";
    constexpr const char* k_event_sigils_impact_large = "Play_Aelorin_SeekerSigils_Impact_Large";

    constexpr const char* k_event_spires_windup = "Play_Aelorin_RisenSpires_Windup";
    constexpr const char* k_event_spires_erupt = "Play_Aelorin_RisenSpires_Erupt";
    constexpr const char* k_event_spires_erupt_p2 = "Play_Aelorin_RisenSpires_Erupt_Phase2";

    constexpr const char* k_event_nova_charge = "Play_Aelorin_Nova_Charge";
    constexpr const char* k_event_nova_blast = "Play_Aelorin_Nova_blast";

    constexpr const char* k_event_cannon_charge = "Play_Aelorin_SpiritCannon_Charge";
    constexpr const char* k_event_cannon_fire = "Play_Aelorin_SpiritCannon_Fire";
    constexpr const char* k_event_cannon_fire_p2 = "Play_Aelorin_SpiritCannon_Fire_Phase2";

    constexpr const char* k_event_grasp_cast = "Play_Aelorin_Grasp_Cast";
    constexpr const char* k_event_grasp_loop = "Play_Aelorin_Grasp_Loop";
    constexpr const char* k_event_grasp_loop_stop = "Stop_Aelorin_Grasp_Loop";
    constexpr const char* k_event_grasp_release = "Play_Aelorin_Grasp_Release";

    constexpr const char* k_event_summon_cast = "Play_Aelorin_Summon_Cast";
    constexpr const char* k_event_summon_spawn = "Play_Aelorin_Summon_Spawn";

    constexpr const char* k_event_teleport_out = "Play_Aelorin_Teleport_Out";
    constexpr const char* k_event_teleport_in = "Play_Aelorin_Teleport_In";
    constexpr const char* k_event_teleport_burst = "Play_Aelorin_Teleport_Burst";

    constexpr const char* k_event_cataclysm_channel = "Play_Aelorin_SoulCataclysm_Channel";
    constexpr const char* k_event_cataclysm_channel_stop = "Stop_Aelorin_SoulCataclysm_Channel";
    constexpr const char* k_event_cataclysm_detonate = "Play_Aelorin_SoulCataclysm_Detonate";

    constexpr const char* k_event_fury_enter = "Play_Aelorin_Fury_Enter";
    constexpr const char* k_event_fury_loop = "Play_Aelorin_Fury_Loop";
    constexpr const char* k_event_fury_loop_stop = "Stop_Aelorin_Fury_Loop";
    constexpr const char* k_event_exhaustion_enter = "Play_Aelorin_Exhaustion_Enter";
    constexpr const char* k_event_exhaustion_loop = "Play_Aelorin_Exhaustion_Loop";
    constexpr const char* k_event_exhaustion_loop_stop = "Stop_Aelorin_Exhaustion_Loop";

    // Varias espiras entran en el mismo frame y varios sigilos impactan casi a la vez:
    // sin ventana de agrupado se empastan y se comen el margen de la mezcla.
    constexpr uint32_t k_group_cooldown_ms = 90;
}

IMPLEMENT_SCRIPT(AelorinSound)

AelorinSound::AelorinSound(GameObject* owner)
    : EnemySound(owner)
{
}

void AelorinSound::getCandidateBanks(const char* const*& outBanks, int& outCount) const
{
    outBanks = k_banks;
    outCount = 1;
}

const char* AelorinSound::evHurt() const
{
    return k_event_hurt;
}

void AelorinSound::startIdleLoop()
{
    if (m_idleLoopID != 0)
    {
        return;
    }

    m_idleLoopID = postEvent(k_event_idle_loop);
}

void AelorinSound::stopIdleLoop()
{
    if (m_idleLoopID == 0)
    {
        return;
    }

    postEvent(k_event_idle_loop_stop);
    m_idleLoopID = 0;
}

void AelorinSound::playPhaseTransition()
{
    postEvent(k_event_phase_transition);
}

void AelorinSound::playDeathRoar()
{
    postEvent(k_event_death);
}

void AelorinSound::playSeekerSigilsCast()
{
    postEvent(k_event_sigils_cast);
}

void AelorinSound::playSeekerSigilsFall()
{
    postEventGrouped(k_event_sigils_fall, "AelorinSigilFall", k_group_cooldown_ms);
}

void AelorinSound::playSeekerSigilsImpact(bool large)
{
    postEventGrouped(large ? k_event_sigils_impact_large : k_event_sigils_impact,
        "AelorinSigilImpact", k_group_cooldown_ms);
}

void AelorinSound::playRisenSpiresWindup()
{
    postEvent(k_event_spires_windup);
}

void AelorinSound::playRisenSpiresErupt(bool phase2)
{
    postEventGrouped(phase2 ? k_event_spires_erupt_p2 : k_event_spires_erupt,
        "AelorinSpireErupt", k_group_cooldown_ms);
}

void AelorinSound::playNovaCharge()
{
    postEvent(k_event_nova_charge);
}

void AelorinSound::playNovaBlast()
{
    postEvent(k_event_nova_blast);
}

void AelorinSound::playSpiritCannonCharge()
{
    postEvent(k_event_cannon_charge);
}

void AelorinSound::playSpiritCannonFire(bool phase2)
{
    postEvent(phase2 ? k_event_cannon_fire_p2 : k_event_cannon_fire);
}

void AelorinSound::playGraspCast()
{
    postEvent(k_event_grasp_cast);
}

void AelorinSound::startGraspLoop()
{
    if (m_graspLoopID != 0)
    {
        return;
    }

    m_graspLoopID = postEvent(k_event_grasp_loop);
}

void AelorinSound::stopGraspLoop()
{
    if (m_graspLoopID == 0)
    {
        return;
    }

    postEvent(k_event_grasp_loop_stop);
    m_graspLoopID = 0;
}

void AelorinSound::playGraspRelease()
{
    postEvent(k_event_grasp_release);
}

void AelorinSound::playSummonCast()
{
    postEvent(k_event_summon_cast);
}

void AelorinSound::playSummonSpawn()
{
    postEventGrouped(k_event_summon_spawn, "AelorinSummonSpawn", k_group_cooldown_ms);
}

void AelorinSound::playTeleportOut()
{
    postEvent(k_event_teleport_out);
}

void AelorinSound::playTeleportIn()
{
    postEvent(k_event_teleport_in);
}

void AelorinSound::playTeleportBurst()
{
    postEvent(k_event_teleport_burst);
}

void AelorinSound::startCataclysmChannel()
{
    if (m_cataclysmLoopID != 0)
    {
        return;
    }

    m_cataclysmLoopID = postEvent(k_event_cataclysm_channel);
}

void AelorinSound::stopCataclysmChannel()
{
    if (m_cataclysmLoopID == 0)
    {
        return;
    }

    postEvent(k_event_cataclysm_channel_stop);
    m_cataclysmLoopID = 0;
}

void AelorinSound::playCataclysmDetonate()
{
    postEvent(k_event_cataclysm_detonate);
}

void AelorinSound::playFuryEnter()
{
    postEvent(k_event_fury_enter);
}

void AelorinSound::startFuryLoop()
{
    if (m_furyLoopID != 0)
    {
        return;
    }

    m_furyLoopID = postEvent(k_event_fury_loop);
}

void AelorinSound::stopFuryLoop()
{
    if (m_furyLoopID == 0)
    {
        return;
    }

    postEvent(k_event_fury_loop_stop);
    m_furyLoopID = 0;
}

void AelorinSound::playExhaustionEnter()
{
    postEvent(k_event_exhaustion_enter);
}

void AelorinSound::startExhaustionLoop()
{
    if (m_exhaustionLoopID != 0)
    {
        return;
    }

    m_exhaustionLoopID = postEvent(k_event_exhaustion_loop);
}

void AelorinSound::stopExhaustionLoop()
{
    if (m_exhaustionLoopID == 0)
    {
        return;
    }

    postEvent(k_event_exhaustion_loop_stop);
    m_exhaustionLoopID = 0;
}

void AelorinSound::stopAllLoops()
{
    EnemySound::stopAllLoops();

    stopIdleLoop();
    stopGraspLoop();
    stopCataclysmChannel();
    stopFuryLoop();
    stopExhaustionLoop();
}
