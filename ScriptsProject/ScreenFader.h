#pragma once

#include "ScriptAPI.h"

class Transform2D;

// Fundido a pantalla completa. Va en un GameObject con una UIImage negra que cubra toda la
// pantalla, al final de la jerarquía del canvas para que quede por encima del HUD.
//
// No confundir con HUDFader, que solo baja el alfa de los tres contenedores del HUD.
class ScreenFader : public Script
{
    DECLARE_SCRIPT(ScreenFader)

public:
    explicit ScreenFader(GameObject* owner);

    void Start() override;
    void Update() override;

    FieldList getExposedFields() const override;

    void fadeTo(float targetAlpha, float duration);
    void setAlpha(float alpha);

    float getAlpha() const { return m_currentAlpha; }
    bool isFading() const { return m_isFading; }

public:
    ComponentRef<Transform2D> m_fadeImage;

    float m_initialAlpha = 0.0f;

private:
    void applyAlpha(float alpha);

    Transform2D* m_fadeImageTransform2D = nullptr;

    bool m_isFading = false;

    float m_currentAlpha = 0.0f;
    float m_startAlpha = 0.0f;
    float m_targetAlpha = 0.0f;

    float m_timer = 0.0f;
    float m_duration = 0.0f;
};
