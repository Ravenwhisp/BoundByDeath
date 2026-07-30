#pragma once

#include "EnemyDamageable.h"

class AelorinBossController;

class AelorinDamageable : public EnemyDamageable
{
	DECLARE_SCRIPT(AelorinDamageable)

public:
	explicit AelorinDamageable(GameObject* owner);

	void Start() override;

	void takeDamage(const HitContext& ctx) override;
	void changePhase();
	bool canChangePhases() const;

protected:
	void onHpDepleted() override;

private:
	AelorinBossController* m_controller = nullptr;
};