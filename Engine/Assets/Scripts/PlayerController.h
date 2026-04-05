#pragma once

#include "ScriptAPI.h"

class PlayerMovement;
class PlayerRotation; 

class PlayerController : public Script
{
    DECLARE_SCRIPT(PlayerController)

public:
    explicit PlayerController(GameObject* owner);

    void Start() override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

	bool getGodMode() const { return m_godMode; }

public:
    int m_playerIndex = 0;

    bool m_godMode = false;

private:
    PlayerMovement* m_movement = nullptr;
    PlayerRotation* m_rotation = nullptr;

    Transform* m_cameraTransform = nullptr;

private:
    Vector3 readMoveDirection() const;

};