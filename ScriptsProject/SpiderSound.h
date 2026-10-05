#pragma once

#include "EnemySound.h"

class SpiderEnemyController;

// Spider SFX. Lives on the Spider GameObject (needs a SOUND_SOURCE).
// The bite plays on the attack telegraph instead of the contact frame: the windup is
// only 0.25 s, so a sound on impact would land after the damage with nothing to react to.
class SpiderSound : public EnemySound
{
    DECLARE_SCRIPT(SpiderSound)

public:
    explicit SpiderSound(GameObject* owner);

    void Start()  override;
    void Update() override;

    FieldList getExposedFields() const override;

    // Idle chatter while it has a target. Grouped so a room of spiders does not pile up.
    float m_chitterMinInterval = 4.0f;
    float m_chitterMaxInterval = 9.0f;

protected:
    void getCandidateBanks(const char* const*& outBanks, int& outCount) const override;

    const char* evBasicTelegraph() const override;
    const char* evBasicImpact()    const override;
    const char* evHurt()           const override;
    const char* evStun()           const override;
    const char* evDeath()          const override;
    const char* evFootstep()       const override;

private:
    void scheduleNextChitter();

    SpiderEnemyController* m_controller = nullptr;
    float m_chitterTimer = 0.0f;
};
