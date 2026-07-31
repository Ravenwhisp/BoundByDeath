#pragma once

#include "EnemyDamageable.h"

#include <vector>

class AelorinBossController;

class AelorinDamageable : public EnemyDamageable
{
	DECLARE_SCRIPT(AelorinDamageable)

public:
	explicit AelorinDamageable(GameObject* owner);

	void Start() override;

	void takeDamage(float amount) override;
	void takeDamage(const HitContext& ctx) override;

	bool isThresholdLocked() const { return m_thresholdLocked; }
	bool isPhaseTransitionPending() const { return m_phaseTransitionPending; }

	void beginPhase2();

protected:
	void onHpDepleted() override;

private:
	const std::vector<float>& getActiveThresholds() const;

	bool hasCurrentThreshold() const;
	bool isCurrentThresholdFinal() const;
	bool isShadowExecution(const EnemyHitContext& ctx) const;

	float getCurrentThresholdPercent() const;
	float getCurrentThresholdHp() const;

	void processNormalDamage(const EnemyHitContext& ctx);
	void processShadowExecution(const EnemyHitContext& ctx);

	void lockCurrentThreshold();
	void advanceThreshold();
	void requestPhaseTransition();
	void handleFinalDeath(const EnemyHitContext& ctx);

private:
	AelorinBossController* m_controller = nullptr;

	std::vector<float> m_phase1Thresholds
	{
		0.50f,
		0.00f
	};

	std::vector<float> m_phase2Thresholds
	{
		0.70f,
		0.45f,
		0.25f,
		0.10f,
		0.00f
	};

	std::size_t m_currentThresholdIndex = 0;

	bool m_thresholdLocked = false;
	bool m_phaseTransitionPending = false;
	bool m_allowFinalDeath = false;
};