#pragma once

#include "EngineAPI.h"
#include "DestroyParticles.h"

namespace ParticleLifecycle
{
    inline void visitParticleSystems(GameObject* gameObject, void (*fn)(ParticleSystemComponent*))
    {
        if (gameObject == nullptr || fn == nullptr)
        {
            return;
        }

        ParticleSystemComponent* particleSystem = ParticleSystemAPI::getParticleSystemComponent(gameObject);
        if (particleSystem != nullptr)
        {
            fn(particleSystem);
        }

        Transform* transform = GameObjectAPI::getTransform(gameObject);
        if (transform == nullptr)
        {
            return;
        }

        const int childCount = TransformAPI::getChildCount(transform);
        for (int i = 0; i < childCount; ++i)
        {
            Transform* child = TransformAPI::getChild(transform, i);
            if (child == nullptr)
            {
                continue;
            }

            visitParticleSystems(ComponentAPI::getOwner(child), fn);
        }
    }

    inline void disableSelfDestruct(GameObject* gameObject)
    {
        if (gameObject == nullptr)
        {
            return;
        }

        DestroyParticles* destroyParticles = GameObjectAPI::findScript<DestroyParticles>(gameObject);
        if (destroyParticles != nullptr)
        {
            destroyParticles->setAutoDestroy(false);
        }

        Transform* transform = GameObjectAPI::getTransform(gameObject);
        if (transform == nullptr)
        {
            return;
        }

        const int childCount = TransformAPI::getChildCount(transform);
        for (int i = 0; i < childCount; ++i)
        {
            Transform* child = TransformAPI::getChild(transform, i);
            if (child == nullptr)
            {
                continue;
            }

            disableSelfDestruct(ComponentAPI::getOwner(child));
        }
    }

    inline void restart(GameObject* gameObject)
    {
        visitParticleSystems(gameObject, [](ParticleSystemComponent* particleSystem)
        {
            ParticleSystemAPI::reset(particleSystem);
            ParticleSystemAPI::play(particleSystem);
        });
    }

    inline void stop(GameObject* gameObject)
    {
        visitParticleSystems(gameObject, [](ParticleSystemComponent* particleSystem)
        {
            ParticleSystemAPI::stop(particleSystem);
        });
    }

    inline void activate(GameObject* gameObject)
    {
        if (gameObject == nullptr)
        {
            return;
        }

        GameObjectAPI::setActive(gameObject, true);
        restart(gameObject);
    }

    inline void deactivate(GameObject* gameObject)
    {
        if (gameObject == nullptr)
        {
            return;
        }

        stop(gameObject);
        GameObjectAPI::setActive(gameObject, false);
    }

    inline GameObject* instantiatePersistent(const AssetId& prefabId, const Vector3& position, const Vector3& rotation, GameObject* parent = nullptr)
    {
        if (!prefabId.isValid())
        {
            return nullptr;
        }

        GameObject* instance = GameObjectAPI::instantiatePrefab(prefabId, position, rotation, parent);
        if (instance == nullptr)
        {
            return nullptr;
        }

        disableSelfDestruct(instance);
        GameObjectAPI::setActive(instance, false);
        return instance;
    }

    inline GameObject* ensurePersistent(GameObject*& storage, const AssetId& prefabId, const Vector3& position, const Vector3& rotation, GameObject* parent = nullptr)
    {
        if (storage == nullptr)
        {
            storage = instantiatePersistent(prefabId, position, rotation, parent);
        }

        return storage;
    }

    inline void destroy(GameObject*& gameObject)
    {
        if (gameObject == nullptr)
        {
            return;
        }

        GameObjectAPI::removeGameObject(gameObject);
        gameObject = nullptr;
    }
}
