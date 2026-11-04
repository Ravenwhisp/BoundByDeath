#pragma once

#include "ScriptAPI.h"

class PlayerMovement;
class PlayerRotation;
class PlayerState;
class AbilityBase;

class PlayerController : public Script
{
    DECLARE_SCRIPT(PlayerController)

public:
    explicit PlayerController(GameObject* owner);

    void Start() override;
    void Update() override;

    FieldList getExposedFields() const override;

    void setGameplayInputLocked(bool locked) { m_gameplayInputLocked = locked; }
    bool isGameplayInputLocked() const { return m_gameplayInputLocked; }

    // Walks the character in this world direction instead of reading the stick (trailer/cutscene tools).
    // Pass Vector3::Zero to hand control back to the player's input.
    void setScriptedMoveDirection(const Vector3& direction) { m_scriptedMoveDirection = direction; }

    bool getGodMode() const { return m_godMode; }
    int getPlayerIndex() const { return m_playerIndex; }
    Vector3 getMoveDirection() const;

public:
    int m_playerIndex = 0;
    bool m_godMode = false;

private:
    PlayerMovement* m_playerMovement = nullptr;
    PlayerRotation* m_playerRotation = nullptr;
    PlayerState* m_playerState = nullptr;

	AbilityBase* m_basicAttack = nullptr;
    AbilityBase* m_chargedAttack = nullptr;
	AbilityBase* m_dash = nullptr;
	AbilityBase* m_specialAbility = nullptr; //Taunt or Arrow Volley

    Transform* m_cameraTransform = nullptr;

    bool m_gameplayInputLocked = false;
    Vector3 m_scriptedMoveDirection = Vector3(0.0f, 0.0f, 0.0f);

private:
    Vector3 readMoveDirection(const Vector2& moveAxis) const;

	AbilityBase* findBasicAttackScript(GameObject* owner);
	AbilityBase* findChargedAttackScript(GameObject* owner);
	AbilityBase* findDashScript(GameObject* owner);
	AbilityBase* findSpecialAbilityScript(GameObject* owner);
};