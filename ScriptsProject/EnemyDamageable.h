#pragma once

#include "Damageable.h"
#include "PlayerAttackType.h"

class EnemyDetectionAggro;
class EnemySound;
class EnemyShadowMark;
class Transform2D;
class EnemyBaseController;
class EnemyBaseDataConfig;
class ReaperGauge;
class ShadowExecution;

struct EnemyHitContext : public HitContext
{
	Transform* attacker = nullptr;
	PlayerAttackType attackType = PlayerAttackType::None;
};

class EnemyDamageable : public Damageable
{
	DECLARE_SCRIPT(EnemyDamageable)

public:
	explicit EnemyDamageable(GameObject* owner);

	void Start() override;
	void Update() override;

	FieldList getExposedFields() const override;
	
    void takeDamage(const HitContext& ctx) override;
	bool lastHitExploitShadowMark() const { return m_lastHitExploitedShadowMark; }
	float getShadowExecutionThresholdMultiplier() const;

protected:
	void onDamaged(float amount) override;
	void onDeath() override;

	void resetLastShadowMarkResult() { m_lastHitExploitedShadowMark = false; }
	bool processShadowMarkHit(PlayerAttackType attackType);
	void applyDamageWithoutShadowMark(const EnemyHitContext& hit);

	virtual void setHealthBarAlpha(float alpha);

private: 
	void resolveHealthBarReferences();
	void updateHealthBarFade();

	void resolveReaperGauge();
	void updateShadowExecutionPreviewAvailability();
	void setShadowExecutionPreviewActive(bool active);

	void resolveShadowExecution();
	void updateShadowExecutionPreview();

private:
	const EnemyBaseDataConfig* m_baseDataConfig = nullptr;
	EnemyDetectionAggro* m_enemyDetectionAggro = nullptr;
	EnemySound* m_enemySound = nullptr;
	EnemyShadowMark* m_shadowMark = nullptr;
	Transform* m_damageSource = nullptr;
	ReaperGauge* m_reaperGauge = nullptr;
	ShadowExecution* m_shadowExecution = nullptr;
	
	bool m_lastHitExploitedShadowMark = false;

	bool m_shadowExecutionPreviewActive = false;

	ComponentRef<Transform2D> m_healthBarContainer;
	Transform2D* m_healthBarContainerTransform = nullptr;
	ComponentRef<Transform2D> m_shadowExecutionPreview;
	Transform2D* m_shadowExecutionPreviewTransform = nullptr;

	float m_healthBarFadeTime = 0.25f;
	float m_healthBarFadeTimer = 0.0f;
	bool m_healthBarFadeActive = false;
};
