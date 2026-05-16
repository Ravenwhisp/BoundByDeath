#pragma once

#include "ScriptAPI.h"

class ArthurDetectionAggro;

class ArthurBossController : public Script
{
	DECLARE_SCRIPT(ArthurBossController)

public:
	explicit ArthurBossController(GameObject* owner);

	void Start() override;
	void drawGizmo() override;

	ScriptFieldList getExposedFields() const override;
};