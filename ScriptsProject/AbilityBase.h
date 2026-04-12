#pragma once

#include "ScriptAPI.h"

class CharacterBase;

// Abstract base class for all character abilities.
// Never instantiated directly — each concrete ability inherits from this.
// NO DECLARE_SCRIPT / IMPLEMENT_SCRIPT macros.
//
// Each ability lives as a sibling Script on the same GameObject as the character.
// It communicates with the character via m_character, assigned in the child's Start().
//
// --- Concrete ability pattern ---
//
//   Start():
//     m_character = static_cast<CharacterBase*>(
//         GameObjectAPI::getScript(m_owner, "DeathCharacter"));  // or "LyrielCharacter"
//     m_cooldown = 1.5f;  // optional, 0 = no cooldown
//
//   Update():
//     AbilityBase::Update();          // MUST be called first — handles dead guard + cooldown tick
//     if (!canActivate()) return;
//     if (!Input::isXxxJustPressed(getPlayerIndex())) return;
//     onActivate();                   // blocks other abilities, sets isActive = true
//     ... ability logic this frame ...
//     onDeactivate();                 // or later when the ability naturally ends
//
class AbilityBase : public Script
{
public:
    explicit AbilityBase(GameObject* owner);

    // Handles the dead guard, mid-execution cancellation on death, and cooldown tick.
    // Concrete abilities MUST call AbilityBase::Update() at the top of their override.
    void Start()  override;
    void Update() override;

    int  getPlayerIndex() const;
    bool isActive()       const { return m_isActive; }
    bool isEnabled()      const { return m_isEnabled; }

    // Disable or re-enable this ability externally (e.g. stun, death lock, cutscene).
    void setEnabled(bool enabled) { m_isEnabled = enabled; }

    void setActive(bool active) { m_isActive = active; }

protected:
    // Returns true when all pre-conditions for activation are met:
    // isEnabled, !isActive, character exists, character alive, canAct, cooldown ready.
    // Concrete abilities can override to add extra checks (e.g. needs a target).
    virtual bool canActivate() const;

    // Marks the ability as running and blocks all other abilities via canAct.
    // Concrete abilities can override for extra on-start behaviour — must call base.
    virtual void onActivate();

    // Marks the ability as finished, starts the cooldown, and unblocks other abilities.
    // Concrete abilities can override for extra on-end behaviour — must call base.
    virtual void onDeactivate();

    // Convenience: deals damage to the character's current target.
    // Finds Damageable on the target GameObject and calls takeDamage(damage).
    // Safe to call even if there is no current target.
    void dealDamageToCurrentTarget(float damage) const;

    // Sibling CharacterBase (or subclass) on the same GameObject.
    // Assigned by each concrete ability in its own Start().
    CharacterBase* m_character = nullptr;

    // Seconds before this ability can activate again after deactivating.
    // Set in the concrete ability's constructor or Start(). 0.0f = no cooldown.
    float m_cooldown = 0.0f;

private:
    bool  m_isActive      = false;
    bool  m_isEnabled     = true;
    float m_cooldownTimer = 0.0f;
};
