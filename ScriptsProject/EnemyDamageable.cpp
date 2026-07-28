#include "pch.h"
#include "EnemyDamageable.h"

#include "EnemyDetectionAggro.h"
#include "EnemySound.h"
#include "EnemyBaseController.h"
#include "EnemyBaseDataConfig.h"
#include "EnemyShadowMark.h"
#include "Transform2D.h"
#include "ReaperGauge.h"
#include "ShadowExecution.h"

IMPLEMENT_SCRIPT_FIELDS_INHERITED(EnemyDamageable, Damageable,
	SERIALIZED_COMPONENT_REF(m_healthBarContainer, "Health Bar Container", ComponentType::TRANSFORM2D),
	SERIALIZED_COMPONENT_REF(m_shadowExecutionPreview, "Shadow Execution Preview", ComponentType::TRANSFORM2D),
	SERIALIZED_FLOAT(m_healthBarFadeTime, "Health Bar Fade Time", 0.0f, 5.0f, 0.05f)
)

EnemyDamageable::EnemyDamageable(GameObject* owner)
	: Damageable(owner)
{
}

void EnemyDamageable::Start()
{
	resolveHealthBarReferences();
	resolveReaperGauge();
	resolveShadowExecution();

	if (m_shadowExecutionPreviewTransform)
	{
		if (m_shadowExecutionPreviewTransform)
		{
			m_shadowExecutionPreviewBaseSize = Transform2DAPI::getBaseSize(m_shadowExecutionPreviewTransform);
			m_shadowExecutionPreviewBasePosition = Transform2DAPI::getPosition(m_shadowExecutionPreviewTransform);

			Transform2DAPI::setPivot(m_shadowExecutionPreviewTransform, Vector2(0.0f, 0.5f));
			Transform2DAPI::setAnchorMin(m_shadowExecutionPreviewTransform, Vector2(0.0f, 0.5f));
			Transform2DAPI::setAnchorMax(m_shadowExecutionPreviewTransform, Vector2(0.0f, 0.5f));
			Transform2DAPI::setAlpha(m_shadowExecutionPreviewTransform, 0.0f);
		}
	}

	// Override HP from controller's attack config (inherits from EnemyBaseDataConfig)
	EnemyBaseController* controller = GameObjectAPI::findScript<EnemyBaseController>(m_owner);
	if (controller)
	{
		m_baseDataConfig = controller->getBaseDataConfig();

		if (m_baseDataConfig && m_baseDataConfig->m_maxHp > 0.0f)
		{
			m_maxHp = m_baseDataConfig->m_maxHp;
		}
	}

	Damageable::Start();

	m_enemyDetectionAggro = GameObjectAPI::findScript<EnemyDetectionAggro>(m_owner);

	if (!m_enemyDetectionAggro)
	{
		Debug::warn("EnemyDetectionAggro Script is missing from %s", GameObjectAPI::getName(m_owner));
	}

	m_shadowMark = GameObjectAPI::findScript<EnemyShadowMark>(m_owner);

	m_enemySound = GameObjectAPI::findScript<EnemySound>(m_owner);

	if (!m_healthBarContainerTransform)
	{
		Debug::warn("Health Bar Container Transform2D is missing from %s", GameObjectAPI::getName(m_owner));
		return;
	}

	setHealthBarAlpha(0.0f);
}

void EnemyDamageable::Update()
{
	Damageable::Update();
	updateHealthBarFade();
	updateShadowExecutionPreviewAvailability();
}

void EnemyDamageable::takeDamage(const HitContext& ctx)
{
	const EnemyHitContext& enemyCtx = static_cast<const EnemyHitContext&>(ctx);

	resetLastShadowMarkResult();

	if (m_isDead || m_invulnerable)
	{
		return;
	}

	processShadowMarkHit(enemyCtx.attackType);
	applyDamageWithoutShadowMark(enemyCtx);
}

float EnemyDamageable::getShadowExecutionThresholdMultiplier() const
{
	return m_baseDataConfig ? m_baseDataConfig->m_shadowExecutionThresholdMultiplier : 1.0f;
}

void EnemyDamageable::onDamaged(float amount)
{
	Damageable::onDamaged(amount);

	if (!m_healthBarFadeActive && m_healthBarFadeTimer < m_healthBarFadeTime)
	{
		m_healthBarFadeActive = true;
	}

	if (m_enemySound)
	{
		m_enemySound->playHurt();
	}

	if (m_shadowExecutionPreviewActive)
	{
		updateShadowExecutionPreview();
	}

	if (!m_enemyDetectionAggro)
	{
		return;
	}

	if (!m_damageSource)
	{
		return;
	}

	m_enemyDetectionAggro->notifyPlayerAttackedEnemy(m_damageSource);

}

void EnemyDamageable::onDeath()
{
	Damageable::onDeath();

	if (m_shadowMark)
	{
		m_shadowMark->clearMark();
	}
}

bool EnemyDamageable::processShadowMarkHit(PlayerAttackType attackType)
{
	if (!m_shadowMark)
	{
		return false;
	}

	m_lastHitExploitedShadowMark = m_shadowMark->processAttack(attackType);
	return m_lastHitExploitedShadowMark;
}

void EnemyDamageable::applyDamageWithoutShadowMark(const EnemyHitContext& hit)
{
	if (hit.attacker)
	{
		m_damageSource = hit.attacker;
	}

	Damageable::takeDamage(hit);

	m_damageSource = nullptr;
}

void EnemyDamageable::resolveHealthBarReferences() 
{
	Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());

	Transform* healthBarTransform = TransformAPI::findChildByName(ownerTransform, "Health Bar");
	if (healthBarTransform)
	{
		Transform* backgroundTransform = TransformAPI::findChildByName(healthBarTransform, "Background");
		if (backgroundTransform)
		{
			GameObject* backgroundObject = ComponentAPI::getOwner(backgroundTransform);

			if (!m_healthBarContainerTransform)
			{
				m_healthBarContainerTransform = static_cast<Transform2D*>(GameObjectAPI::getComponent(backgroundObject, ComponentType::TRANSFORM2D));
			}

			if (!m_healthBarSlider)
			{
				Transform* slider1Transform = TransformAPI::findChildByName(backgroundTransform, "Slider 1");
				if (slider1Transform)
				{
					GameObject* slider1Object = ComponentAPI::getOwner(slider1Transform);
					m_healthBarSlider = static_cast<UISlider*>(GameObjectAPI::getComponent(slider1Object, ComponentType::UISLIDER));
				}
			}

			if (!m_healthBar2Slider)
			{
				Transform* slider2Transform = TransformAPI::findChildByName(backgroundTransform, "Slider 2");
				if (slider2Transform)
				{
					GameObject* slider2Object = ComponentAPI::getOwner(slider2Transform);
					m_healthBar2Slider = static_cast<UISlider*>(GameObjectAPI::getComponent(slider2Object, ComponentType::UISLIDER));
				}
			}

			if (!m_shadowExecutionPreviewTransform)
			{
				Transform* previewTransform = TransformAPI::findChildByName(backgroundTransform, "Shadow Execution Preview");
				if (previewTransform)
				{
					GameObject* previewObject = ComponentAPI::getOwner(previewTransform);
					m_shadowExecutionPreviewTransform = static_cast<Transform2D*>(GameObjectAPI::getComponent(previewObject, ComponentType::TRANSFORM2D));
				}
			}
		}
	}

	if (!m_healthBarContainerTransform)
	{
		m_healthBarContainerTransform = m_healthBarContainer.getReferencedComponent();
	}

	if (!m_shadowExecutionPreviewTransform)
	{
		m_shadowExecutionPreviewTransform = m_shadowExecutionPreview.getReferencedComponent();
	}
}

void EnemyDamageable::updateHealthBarFade()
{
	if (!m_healthBarFadeActive)
	{
		return;
	}

	if (!m_healthBarContainerTransform)
	{
		m_healthBarFadeActive = false;
		return;
	}

	if (m_healthBarFadeTime <= 0.0f)
	{
		setHealthBarAlpha(1.0f);
		m_healthBarFadeActive = false;
		return;
	}

	m_healthBarFadeTimer += Time::getDeltaTime();

	float t = m_healthBarFadeTimer / m_healthBarFadeTime;
	t = std::clamp(t, 0.0f, 1.0f);

	float alpha = MathAPI::evaluateEasing(MathAPI::EasingType::EaseOutCubic, t);

	setHealthBarAlpha(alpha);

	if (t >= 1.0f)
	{
		setHealthBarAlpha(1.0f);
		m_healthBarFadeActive = false;
	}
}

void EnemyDamageable::setHealthBarAlpha(float alpha)
{
	if (!m_healthBarContainerTransform)
	{
		return;
	}

	alpha = std::clamp(alpha, 0.0f, 1.0f);

	Transform2DAPI::setAlpha(m_healthBarContainerTransform, alpha);
}

void EnemyDamageable::resolveReaperGauge()
{
	const std::vector<GameObject*> holders = SceneAPI::findAllGameObjectsWithScript<ReaperGauge>();

	if (!holders.empty())
	{
		m_reaperGauge = GameObjectAPI::findScript<ReaperGauge>(holders[0]);
	}

	if (!m_reaperGauge)
	{
		Debug::warn("[EnemyDamageable] ReaperGauge not found for '%s'.", GameObjectAPI::getName(m_owner));
	}
}

void EnemyDamageable::updateShadowExecutionPreviewAvailability()
{
	if (!m_reaperGauge)
	{
		return;
	}

	const bool shouldBeActive = m_reaperGauge->isFull() && !m_isDead;

	if (shouldBeActive != m_shadowExecutionPreviewActive)
	{
		setShadowExecutionPreviewActive(shouldBeActive);
	}
}

void EnemyDamageable::setShadowExecutionPreviewActive(bool active)
{
	m_shadowExecutionPreviewActive = active;

	if (!m_shadowExecutionPreviewTransform)
	{
		return;
	}

	if (active)
	{
		updateShadowExecutionPreview();
	}
	else
	{
		Transform2DAPI::setAlpha(m_shadowExecutionPreviewTransform, 0.0f);
		Transform2DAPI::setBaseSize(m_shadowExecutionPreviewTransform, m_shadowExecutionPreviewBaseSize);
		Transform2DAPI::setPosition(m_shadowExecutionPreviewTransform, m_shadowExecutionPreviewBasePosition);
	}
}

void EnemyDamageable::resolveShadowExecution()
{
	const std::vector<GameObject*> holders = SceneAPI::findAllGameObjectsWithScript<ShadowExecution>();

	if (!holders.empty())
	{
		m_shadowExecution = GameObjectAPI::findScript<ShadowExecution>(holders[0]);
	}

	if (!m_shadowExecution)
	{
		Debug::warn("[EnemyDamageable] ShadowExecution not found for '%s'.", GameObjectAPI::getName(m_owner));
	}
}

void EnemyDamageable::updateShadowExecutionPreview()
{
	if (!m_shadowExecutionPreviewActive || !m_shadowExecution || !m_shadowExecutionPreviewTransform || m_isDead)
	{
		return;
	}

	const float maxHp = getMaxHp();
	const float currentHp = getCurrentHp();

	if (maxHp <= 0.0f || currentHp <= 0.0f)
	{
		Transform2DAPI::setAlpha(m_shadowExecutionPreviewTransform, 0.0f);
		return;
	}

	const ShadowExecutionPreview preview = m_shadowExecution->calculatePreview(this);

	float currentHpPercent = currentHp / maxHp;
	float resultingHpPercent = preview.resultingHpPercent;

	currentHpPercent = std::clamp(currentHpPercent, 0.0f, 1.0f);
	resultingHpPercent = std::clamp(resultingHpPercent, 0.0f, currentHpPercent);

	const float damageSegmentPercent = currentHpPercent - resultingHpPercent;

	if (damageSegmentPercent <= 0.0f)
	{
		Transform2DAPI::setAlpha(m_shadowExecutionPreviewTransform, 0.0f);
		return;
	}

	Vector2 previewSize = m_shadowExecutionPreviewBaseSize;
	previewSize.x = m_shadowExecutionPreviewBaseSize.x * damageSegmentPercent;

	Vector2 previewPosition = m_shadowExecutionPreviewBasePosition;
	previewPosition.x = m_shadowExecutionPreviewBasePosition.x + m_shadowExecutionPreviewBaseSize.x * resultingHpPercent;

	Transform2DAPI::setBaseSize(m_shadowExecutionPreviewTransform, previewSize);
	Transform2DAPI::setPosition(m_shadowExecutionPreviewTransform, previewPosition);
	Transform2DAPI::setAlpha(m_shadowExecutionPreviewTransform, 1.0f);

	Debug::log("[ShadowExecutionPreview] Enemy '%s': current %.1f%%, resulting %.1f%%, segment %.1f%%, damage %.1f, lethal %s.", GameObjectAPI::getName(m_owner), currentHpPercent * 100.0f, resultingHpPercent * 100.0f, damageSegmentPercent * 100.0f, preview.damage, preview.willDie ? "true" : "false");
}

IMPLEMENT_SCRIPT(EnemyDamageable)
