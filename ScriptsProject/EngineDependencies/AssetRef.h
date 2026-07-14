#pragma once

class DataContainer;

template<typename T = void>
struct AssetRef
{
    AssetReference m_ref;
    std::shared_ptr<T> m_data;

    T* get() const
    {
        return m_data.get();
    }

    operator bool() const { return m_data != nullptr; }
};

using PrefabRef = AssetRef<struct Prefab>;
using SceneRef = AssetRef<struct Scene>;
using MaterialRef = AssetRef<struct BasicMaterial>;
using DataContainerRef = AssetRef<DataContainer>;
