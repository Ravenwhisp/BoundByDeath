#pragma once

#include "ScriptAPI.h"

class PlayerMovement;
class PlayerRotation;
class PlayerState;

class PlayerController : public Script
{
    DECLARE_SCRIPT(PlayerController)

public:
    explicit PlayerController(GameObject* owner);

    void Start() override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

    bool getGodMode() const { return m_godMode; }

	Vector2 getMoveAxis() const { return m_moveAxis; } //esto se quitara cuando se haga todo desde playercontroller, lo he añadido para poder hacer el readmovedirection desde el dash

    Vector3 readMoveDirection(const Vector2& moveAxis) const; //esto se quitara cuando se haga todo desde playercontroller, lo he añadido para poder hacer el readmovedirection desde el dash

public:
	int m_playerIndex = 0;

	bool m_godMode = false;

private:
	PlayerMovement* m_playerMovement = nullptr;
	PlayerRotation* m_playerRotation = nullptr;
    PlayerState* m_playerState = nullptr;

	Transform* m_cameraTransform = nullptr;

	Vector2 m_moveAxis = { 0.0f, 0.0f }; //esto se quitara cuando se haga todo desde playercontroller, lo he añadido para poder hacer el readmovedirection desde el dash

private:
    //Vector3 readMoveDirection(const Vector2& moveAxis) const;

    PlayerMovement* findMovementScript(GameObject* owner);
    PlayerRotation* findRotationScript(GameObject* owner);
	PlayerState* findStateScript(GameObject* owner);
};