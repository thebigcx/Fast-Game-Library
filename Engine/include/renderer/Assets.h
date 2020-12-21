#pragma once

#include <string>
#include <unordered_map>
#include <typeindex>
#include <vector>

#include <renderer/Texture2D.h>
#include <core/Logger.h>

class IAssetList
{
    friend class Assets;

protected:
    IAssetList() {}
    virtual ~IAssetList() = default;

    virtual int getAssetCount() = 0;
};

template <typename T>
class AssetList : public IAssetList
{
    friend class Assets;

public:
    void add(const std::string& key, Shared<T> asset)
    {
        m_assets.insert(std::make_pair(key, asset));
    }

    const Shared<T>& get(const std::string& key)
    {
        if (!exists(key))
        {
            Logger::getCoreLogger()->error("Asset does not exist: %s", key);
        }

        return m_assets.at(key);
    }

    void remove(const std::string& key)
    {
        m_assets.erase(key);
    }

    bool exists(const std::string& key)
    {
        return m_assets.find(key) != m_assets.end();
    }

    int getAssetCount() override
    {
        return m_assets.size();
    }

    typename std::unordered_map<std::string, Shared<T>>::iterator begin() { return m_assets.begin(); }
    typename std::unordered_map<std::string, Shared<T>>::iterator end()   { return m_assets.end(); }
    typename std::unordered_map<std::string, Shared<T>>::const_iterator begin() const { return m_assets.begin(); }
    typename std::unordered_map<std::string, Shared<T>>::const_iterator end()   const { return m_assets.end(); }

    std::unordered_map<std::string, Shared<T>>& getInternalList()
    {
        return m_assets;
    }

private:
    std::unordered_map<std::string, Shared<T>> m_assets;
};

class Assets
{
public:
    Assets() {}

    ~Assets()
    {
        flush();
    }

    template<typename T>
    static void add(const std::string& key, Shared<T> asset)
    {
        if (m_instance.m_lists.find(typeid(T)) == m_instance.m_lists.end())
        {
            m_instance.m_lists.insert(std::pair<std::type_index, IAssetList*>(typeid(T), new AssetList<T>()));
        }

        getList<T>()->add(key, asset);
    }

    template<typename T>
    static const Shared<T>& get(const std::string& key)
    {
        if (!listExists<T>())
        {
            Logger::getCoreLogger()->error("Asset List does not exist.");
        }

        return getList<T>()->get(key);
    }

    template<typename T>
    static bool exists(const std::string& key)
    {
        if (!listExists<T>())
        {
            return false;
        }

        return getList<T>()->exists(key);
    }

    template<typename T>
    static void remove(const std::string& key)
    {
        if (!listExists<T>())
        {
            return;
        }

        getList<T>()->remove();
    }

    static void flush()
    {
        for (auto list : m_instance.m_lists)
        {
            delete list.second;
        }
        m_instance.m_lists.clear();
    }

    template<typename T>
    static int getAssetCount()
    {
        return getList<T>()->getAssetCount();
    }

    template<typename T>
    static AssetList<T>* getList()
    {
        return (AssetList<T>*)m_instance.m_lists[typeid(T)];
    }

    template<typename T>
    static bool listExists()
    {
        return m_instance.m_lists.find(typeid(T)) != m_instance.m_lists.end();
    }

private:
    std::unordered_map<std::type_index, IAssetList*> m_lists;

    static Assets m_instance;
};