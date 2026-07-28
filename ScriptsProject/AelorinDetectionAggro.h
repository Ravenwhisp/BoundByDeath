#pragma once

#include "ScriptAPI.h"

class AelorinDetectionAggro : public Script
{
	DECLARE_SCRIPT(AelorinDetectionAggro)

public:
	explicit AelorinDetectionAggro(GameObject* owner);

	void Start() override;
	void Update() override;
	void drawGizmo() override;

	FieldList getExposedFields() const override;

public:
	float m_detectionRadius = 20.0f;
	bool m_debugEnabled = true;

private:
	Transform* m_lyrielTransform = nullptr;
	Transform* m_deathTransform = nullptr;
	// add boss controller to check phases

private:
	void findPlayerTransforms();
	Transform* getOwnerTransform() const;

public:
	Vector3 getLyrielPosition() const;
	Vector3 getDeathPosition() const;

	float getDistanceToLyriel() const;
	float getDistanceToDeath() const;
};