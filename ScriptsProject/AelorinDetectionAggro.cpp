#include "pch.h"
#include "AelorinDetectionAggro.h"

IMPLEMENT_SCRIPT_FIELDS(AelorinDetectionAggro,
	SERIALIZED_FLOAT(m_detectionRadius, "Detection Radius", 0.0f, 50.0f, 0.1f),
	SERIALIZED_BOOL(m_debugEnabled, "Debug Enabled")
)

AelorinDetectionAggro::AelorinDetectionAggro(GameObject* owner) : Script(owner) {}

void AelorinDetectionAggro::Start()
{
	findPlayerTransforms();
}

// update
// drawgizmo

void AelorinDetectionAggro::findPlayerTransforms()
{

}

// getownertransform
// getlyrielposition
// getdeathposition
// getdistancetolyriel
// getdistancetodeath