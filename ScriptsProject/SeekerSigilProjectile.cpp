#include "pch.h"
#include "SeekerSigilProjectile.h"

#include "AelorinAttackExecutor.h"
#include "CameraShake.h"
#include "AelorinVFX.h"

namespace
{
	// El sigilo sale del pool sin padre, asi que no puede subir a buscar el AelorinSound
	// del jefe. Postea desde su propio emisor, que ademas deja la explosion donde cae y no
	// en el centro de la arena. Mismo patron que EnergyBallProjectile.
	void postSigilImpact(GameObject* owner, bool heavy)
	{
		ComponentSoundSource* source = AudioAPI::getSoundSourceComponent(owner);
		if (source == nullptr)
		{
			return;
		}

		AudioAPI::postEvent(source, "LevelBoss.bnk", heavy
			? "Play_Aelorin_SeekerSigils_Impact_Large"
			: "Play_Aelorin_SeekerSigils_Impact");
	}
}

SeekerSigilProjectile::SeekerSigilProjectile(GameObject* owner)
	: ProjectileBase(owner)
{
}

void SeekerSigilProjectile::launch(
	const Vector3& startPosition,
	const Vector3& targetPosition,
	float fallSpeed,
	float lifetime,
	float impactRadius,
	float damage,
	AelorinAttackExecutor* attackExecutor,
	CameraShake* cameraShake,
	AelorinVFX* vfx,
	bool heavyImpact
)
{
	m_cameraShake = cameraShake;
	m_vfx = vfx;
	m_heavyImpact = heavyImpact;

	m_targetPosition = targetPosition;

	m_fallSpeed = fallSpeed;
	m_lifetime = lifetime;
	m_aliveTimer = 0.0f;

	m_impactRadius = impactRadius;
	m_damage = damage;

	m_attackExecutor = attackExecutor;

	m_isLaunched = true;
	m_inUse = true;

	GameObjectAPI::setActive(getOwner(), true);

	Transform* projectileTransform = GameObjectAPI::getTransform(getOwner());

	if (!projectileTransform)
	{
		returnToPool();
		return;
	}

	TransformAPI::setGlobalPosition(projectileTransform, startPosition);
}

void SeekerSigilProjectile::Update()
{
	if (!m_isLaunched)
	{
		return;
	}

	m_aliveTimer += Time::getDeltaTime();

	if (m_aliveTimer >= m_lifetime)
	{
		returnToPool();
		return;
	}

	Transform* projectileTransform = GameObjectAPI::getTransform(getOwner());

	if (!projectileTransform)
	{
		returnToPool();
		return;
	}

	const Vector3 currentPosition = TransformAPI::getGlobalPosition(projectileTransform);

	Vector3 toTarget = m_targetPosition - currentPosition;

	const float distanceSquared = toTarget.LengthSquared();

	const float movementThisFrame =	m_fallSpeed * Time::getDeltaTime();

	if (distanceSquared <= movementThisFrame * movementThisFrame)
	{
		TransformAPI::setGlobalPosition(projectileTransform, m_targetPosition);

		playImpactFeedback();
		applyImpactDamage();
		returnToPool();
		return;
	}

	if (distanceSquared <= 0.00001f)
	{
		playImpactFeedback();
		applyImpactDamage();
		returnToPool();
		return;
	}

	toTarget.Normalize();

	TransformAPI::translateGlobal(projectileTransform, toTarget * movementThisFrame);
}

void SeekerSigilProjectile::applyImpactDamage()
{
	if (!m_attackExecutor)
	{
		Debug::warn("[SeekerSigilProjectile] AelorinAttackExecutor not found.");
		return;
	}

	m_attackExecutor->applyDamageInRadius(m_targetPosition, m_impactRadius, m_damage, "Aelorin Seeker Sigils");
}

void SeekerSigilProjectile::playImpactFeedback()
{
	postSigilImpact(getOwner(), m_heavyImpact);

	if (m_cameraShake)
	{
		if (m_heavyImpact)
		{
			m_cameraShake->shake(0.04f, 0.25f);
		}
		else
		{
			m_cameraShake->shake(0.02f, 0.25f);
		}
	}

	if (m_vfx)
	{
		if (m_heavyImpact)
		{
			m_vfx->playSeekerSigilImpactDustBig(m_targetPosition);
		}
		else
		{
			m_vfx->playSeekerSigilImpactDust(m_targetPosition);
		}
	}
}

void SeekerSigilProjectile::resetProjectile()
{
	m_cameraShake = nullptr;
	m_vfx = nullptr;
	m_heavyImpact = false;

	m_targetPosition = Vector3::Zero;

	m_fallSpeed = 0.0f;
	m_lifetime = 0.0f;
	m_aliveTimer = 0.0f;

	m_impactRadius = 0.0f;
	m_damage = 0.0f;

	m_attackExecutor = nullptr;

	m_isLaunched = false;

	ProjectileBase::resetProjectile();
}

IMPLEMENT_SCRIPT(SeekerSigilProjectile)