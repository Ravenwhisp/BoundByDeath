#pragma once

#include "ScriptAPI.h"

#include "Pickup.h"

class HealthPickup : public Pickup
{
    DECLARE_SCRIPT(HealthPickup)

public:
    explicit HealthPickup(GameObject* owner);

    void Start() override;
    void Update() override;
	void OnTriggerEnter(GameObject* player) override; //esto es cuando el pickup llega al jugador. debera haber otro trigger para xuclar el pickup

	void drawGizmo() override;

    ScriptFieldList getExposedFields() const override;
    
public:
	float m_healAmount = 25.0f;
	float m_detectionRadius = 8.0f;
	float m_maxFlySpeed = 30.0f;
	float m_slowDetectionRadius = 20.0f; //radio para que se acerque muuuuy lentamente al player mas cercano. a partir de m_detectionRadius, ira acelerando
	float m_slowSpeed = 0.5f; //velocidad a la que se mueve hacia el player cuando esta dentro de m_slowDetectionRadius pero fuera de m_detectionRadius

private:
	void moveTowardsClosestPlayer(const GameObject* closestPlayer, const float dt); //funcion para detectar al player mas cercano y moverse hacia el. se llama cada update, pero solo hace algo si el player esta dentro de m_slowDetectionRadius
	float getDistanceToClosestPlayer(const Vector3& position, GameObject*& closestPlayer) const;

private:
	std::vector<GameObject*> playerObjects;

	float m_startHeight = 0.0f;
	float m_lockOnTimer = 0.0f;
	bool m_isLockedOnPlayer = false;

};