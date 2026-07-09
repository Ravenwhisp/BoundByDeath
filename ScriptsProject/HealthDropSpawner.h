#pragma once

#include "ScriptAPI.h"
struct AssetReference;

class HealthDropSpawner
{
public:
    static GameObject* drop(const AssetReference& prefabRef, const Vector3& originPosition, float healAmount, float dropRadius, float dropHeight);
};