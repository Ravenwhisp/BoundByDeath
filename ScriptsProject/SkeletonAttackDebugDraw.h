#pragma once

#include "ScriptAPI.h"

class SkeletonAttackConfig;

class SkeletonAttackDebugDraw : public Script
{
	DECLARE_SCRIPT(SkeletonAttackDebugDraw)

public:
	explicit SkeletonAttackDebugDraw(GameObject* owner);

	void Start() override;
	void drawGizmo() override;

	ScriptFieldList getExposedFields() const override;

public:
	bool m_debugEnabled = true;

	bool m_drawScimitarStartRange = true;
	bool m_drawDashStopRange = true;
	bool m_drawBasicAttackRange = true;
	bool m_drawFinalHitRange = true;

	float m_heightOffset = 0.15f;

private:
	SkeletonAttackConfig* m_attackConfig = nullptr;
};