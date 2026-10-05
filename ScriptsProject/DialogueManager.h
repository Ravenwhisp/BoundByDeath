#pragma once

#include "ScriptAPI.h"

#include <vector>

class Bound;
class Damageable;
class PlayerState;

// Spoken reactions between Death and Lyriel. Lives on the GameManager and polls the two
// players instead of hooking into their scripts, so nothing else had to change.
//
// Two kinds of line:
//   - Barks: one random take out of a Wwise container. Frequent situations (taking a hit)
//     speak rarely; rare ones (going down, getting up) speak every time.
//   - Dialogues: two lines in a fixed order, one character answering the other. The second
//     waits for the first to finish, using the durations the bank reports.
//
// Only one line is ever in the air: anything triggered while someone is talking is dropped.
class DialogueManager : public Script
{
    DECLARE_SCRIPT(DialogueManager)

public:
    explicit DialogueManager(GameObject* owner);

    void Start()  override;
    void Update() override;

    FieldList getExposedFields() const override;

    float m_lowHealthPercent     = 0.30f;
    float m_takesDamageChance    = 0.34f;
    float m_takesDamageCooldown  = 18.0f;
    float m_lowHealthCooldown    = 30.0f;
    float m_separationCooldown   = 35.0f;
    float m_lineGap              = 0.25f;

private:
    struct Speaker
    {
        GameObject*           object      = nullptr;
        ComponentSoundSource* source      = nullptr;
        Damageable*           damageable  = nullptr;
        PlayerState*          state       = nullptr;

        float lastHp     = -1.0f;
        bool  wasLowHp   = false;
        bool  wasDowned  = false;

        float damageCooldown    = 0.0f;
        float lowHealthCooldown = 0.0f;
    };

    struct Line
    {
        ComponentSoundSource* source   = nullptr;
        const char*           event    = nullptr;
        float                 duration = 0.0f;
    };

    void resolveSpeaker(Speaker& speaker, GameObject* object);
    void updateSpeaker(Speaker& self, Speaker& partner, bool isDeath, float dt);
    void updateSeparation(float dt);
    void updateQueue(float dt);

    void say(const Speaker& speaker, const char* eventName, float duration);
    bool busy() const;

    Speaker m_death;
    Speaker m_lyriel;
    Bound*  m_bound = nullptr;

    std::vector<Line> m_queue;
    float m_lineTimer = 0.0f;

    float m_separationCooldownTimer = 0.0f;

    bool m_wasSeparated      = false;
    bool m_firstSeparationDone = false;
};
