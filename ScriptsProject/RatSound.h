#pragma once

#include "ScriptAPI.h"

// SFX de las ratas que corretean por los niveles. No hereda de EnemySound a proposito: una
// rata no recibe dano, no se aturde, no muere en combate y no tiene pasos que marcar, asi
// que toda esa maquinaria sobraria.
//
// Sus eventos viven en BoundByDeath.bnk, el bank comun que esta siempre cargado, porque
// aparecen tanto en el Nivel 1 como en el Nivel 2. El audio es 3D con un radio de 12
// metros, bastante mas corto que el de los enemigos: son ambiente, no una amenaza, y a 25
// metros se oirian por media sala. Hace falta un SOUND_SOURCE en el GameObject para que
// Wwise tenga un emisor con el que medir la distancia.
class RatSound : public Script
{
    DECLARE_SCRIPT(RatSound)

public:
    explicit RatSound(GameObject* owner);

    void Start() override;
    void Update() override;

    // El chillido, para cuando la rata se asusta y echa a correr.
    void playSqueak();

    // El correteo. Se corta solo si la rata desaparece, ver stop().
    void playScurry();

    // Las ratas se destruyen al terminar de huir, y un one-shot posteado en su propio
    // emisor se cortaria a medias. Quien la destruya llama aqui primero.
    void stop();

    FieldList getExposedFields() const override;

    // Cada cuanto puede repetir cada sonido, para que un grupo de ratas no sature.
    float m_squeakCooldown = 0.4f;
    float m_scurryCooldown = 0.3f;

private:
    ComponentSoundSource* m_source = nullptr;

    uint32_t m_scurryID = 0;

    float m_squeakTimer = 0.0f;
    float m_scurryTimer = 0.0f;
};
