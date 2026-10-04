#pragma once

#include "ScriptAPI.h"

#include <string>
#include <vector>

class SummonerTeleportVFX final : public Script
{
    DECLARE_SCRIPT(SummonerTeleportVFX)

public:
    explicit SummonerTeleportVFX(GameObject* owner);

    void Start() override;
    void Update() override;

    FieldList getExposedFields() const override;

    static Vector3 helixPoint(float t, float baseAngle, float radius, float coreRadius, float height, float turns);

private:
    void findParts();

public:
    std::string m_streamPrefix = "Spiral Stream";
    std::string m_convergeBurstName = "Converge Burst";

    float m_duration = 0.7f;
    float m_radius = 0.85f;
    float m_coreRadius = 0.15f;
    float m_height = 2.4f;
    float m_turns = 1.1f;

private:
    std::vector<Transform*> m_streams;
    std::vector<float> m_baseAngles;
    GameObject* m_convergeBurst = nullptr;

    float m_timer = 0.0f;
    bool m_converged = false;
};
