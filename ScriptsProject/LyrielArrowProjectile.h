#pragma once

#include "ScriptAPI.h"
#include "ProjectileBase.h"

class LyrielCharacter;
class LyrielSound;

class LyrielArrowProjectile : public ProjectileBase
{
    DECLARE_SCRIPT(LyrielArrowProjectile)

public:
    explicit LyrielArrowProjectile(GameObject* owner);

    void Update() override;
    FieldList getExposedFields() const override;

    enum class VisualModel
    {
        Basic = 0,
        Charged = 1,
        Volley = 2
    };

    void launch(const Vector3& startPosition, const Vector3& direction, float speed, float lifetime, GameObject* target, float damage, VisualModel visual = VisualModel::Basic);

    void resetProjectile() override;

private:
    void applyImpactDamage();
    void prepareVisuals();
    void cacheShooterScripts();
    void activateVisual(VisualModel visual);
    void deactivateVisuals();
    void setExternalVisualActive(GameObject* visualObject, TrailComponent* trail, bool active);
    void setBasicVisualActive(bool active);

public:
    PrefabRef m_visualChargedPrefab;
    PrefabRef m_visualVolleyPrefab;

private:
    Vector3 m_direction = Vector3::Zero;

    float m_speed = 0.0f;
    float m_currentLifetime = 0.0f;
    float m_lifeTimer = 0.0f;

    GameObject* m_target = nullptr;
    float m_damage = 0.0f;

    Transform* m_transform = nullptr;
    Component* m_basicModel = nullptr;
    TrailComponent* m_basicTrail = nullptr;
    GameObject* m_basicAdornment = nullptr;
    GameObject* m_chargedVisual = nullptr;
    TrailComponent* m_chargedTrail = nullptr;
    GameObject* m_volleyVisual = nullptr;
    TrailComponent* m_volleyTrail = nullptr;
    LyrielCharacter* m_lyrielCharacter = nullptr;
    LyrielSound* m_sound = nullptr;
    bool m_visualsPrepared = false;
};
