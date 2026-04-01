#pragma once

#include "ScriptAPI.h"

class EnemyDetectionAggro : public Script
{
	DECLARE_SCRIPT(EnemyDetectionAggro)

public:
	explicit EnemyDetectionAggro(GameObject* owner);

	void Start() override;
	void Update() override;

	ScriptFieldList getExposedFields() const override;

public:
	// Exposed
	float m_detectionRadius = 10.0f;
	float m_loseAggroDelay = 2.0f;
	bool m_debugEnabled = true;
};