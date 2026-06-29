#pragma once
#include <Script.h>
#include "ScriptAPI.h"


class DeathParticles : public Script
{
	DECLARE_SCRIPT(DeathParticles)
public:

	explicit DeathParticles(GameObject* owner);

	ScriptComponentRef<Transform> m_dashTrail;
	ScriptComponentRef<Transform> m_scytheTrail;
	ScriptComponentRef<Transform> m_tauntParticle;

	Transform* m_dashTrailController = nullptr;
	Transform* m_scytheTrailController = nullptr;
	Transform* m_tauntParticle = nullptr;

	ScriptFieldList getExposedFields() const override;

	void SetDashActive();
	void SetDashInactive();

	void SetScytheActive();
	void SetScytheInactive();

	void SetTauntActive();
	void SetTauntInactive();

private:

	Transform* getTransform(ScriptComponentRef<Transform> controller);
};

