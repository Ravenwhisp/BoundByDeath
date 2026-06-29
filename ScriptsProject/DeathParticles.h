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

	Transform* m_dashTrailController = nullptr;
	Transform* m_scytheTrailController = nullptr;

	ScriptFieldList getExposedFields() const override;

	void SetDashActive();
	void SetDashInactive();

	void SetScytheActive();
	void SetScytheInactive();

private:

	Transform* getTransform(ScriptComponentRef<Transform> controller);
};

