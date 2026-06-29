#pragma once
#include <Script.h>
#include "ScriptAPI.h"


class LyrielParticles : public Script
{
	DECLARE_SCRIPT(LyrielParticles)
public:

	explicit LyrielParticles(GameObject* owner);

	ScriptComponentRef<Transform> m_dashTrail;

	GameObject* dashTrailController = nullptr;

	ScriptFieldList getExposedFields() const override;

	void SetDashActive();
	void SetDashInactive();

private:

	GameObject* getObject(ScriptComponentRef<Transform> controller);

};