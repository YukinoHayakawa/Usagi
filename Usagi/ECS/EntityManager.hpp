#pragma once

#include <vector>
#include <typeindex>
#include <map>
#include <stdexcept>
#include <any>

#include <boost/iterator/filter_iterator.hpp>

#include <Usagi/Core/Exception.hpp>

#include "Entity.hpp"
#include "ComponentStorage.hpp"
#include "ComponentStream.hpp"

namespace usagi::ecs
{
class EntityManager
{
    std::size_t mEntityId = 0;
    SortedArray<Entity> mEntities;
    std::map<std::type_index, std::any> mComponentStorage;

    template <typename Component>
    auto registerComponentType()
    {
        if(mComponentStorage.size() >= MAX_COMPONENTS)
        {
            USAGI_THROW(std::logic_error(
                "Component number exceeds the maximum allowed."));
        }
        auto it = mComponentStorage.insert({
            typeid(Component),
            ComponentStorage<Component> { mComponentStorage.size() }
        });
        assert(it.second);
        return it.first;
    }

    template <typename... Components>
    struct EntityComponentMaskFilter
    {
        EntityManager *manager = nullptr;
        Entity::ComponentMask mask;

        EntityComponentMaskFilter() = default;

        EntityComponentMaskFilter(
            EntityManager *manager,
            Entity::ComponentMask mask)
            : manager(manager)
            , mask(std::move(mask))
        {
        }

        bool operator()(const Entity &e) const
        {
            assert(manager);
            return e.matchMask(mask);
        }
    };

    template <typename... Components>
    using EntityFilter = boost::filter_iterator<
        EntityComponentMaskFilter<Components...>,
        std::vector<Entity>::iterator
    >;

public:
    /*
     * Component Storage
     */

    template <typename Component>
    ComponentStorage<Component> & componentStorage()
    {
        // find existing component array
        auto it = mComponentStorage.find(typeid(Component));
        if(it == mComponentStorage.end())
        {
            it = registerComponentType<Component>();
        }
        return std::any_cast<ComponentStorage<Component>&>(it->second);
    }

    /*
     * Component Mask Bits
     */

    template <typename Component>
    Entity::ComponentMask componentMaskBit()
    {
        Entity::ComponentMask mask;
        mask[componentStorage<Component>().mask_bit].flip();
        return mask;
    }

    template <typename... Components>
    Entity::ComponentMask buildComponentMask()
    {
        Entity::ComponentMask mask;
        Entity::ComponentMask bits[] = { componentMaskBit<Components>()... };
        for(auto &&bit : bits)
            mask |= bit;
        return mask;
    }

    /*
     * Component Stream
     */

    template <typename Component>
    auto componentStream()
    {
        const auto it = mComponentStorage.find(typeid(Component));
        if(it == mComponentStorage.end())
        {
            USAGI_THROW(std::runtime_error("Component type not registered."));
        }
        ComponentStorage<Component> &component_array =
            std::any_cast<ComponentStorage<Component>&>(it->second);
        return ComponentStream<Component>(component_array);
    }

    template <typename... Components>
    auto componentStreams()
    {
        return std::make_tuple(componentStream<Components>()...);
    }

    /*
     * Entity Access
     */

    auto entityBegin()
    {
        return mEntities.begin();
    }

    auto entityEnd()
    {
        return mEntities.end();
    }

    template <typename... Components>
    auto entityFilteredBegin()
    {
        EntityFilter<Components...> f {
            { this, buildComponentMask<Components...>() },
            mEntities.begin(), mEntities.end()
        };
        return f;
    }

    template <typename... Components>
    auto entityFilteredEnd()
    {
        return EntityFilter<Components...>(mEntities.end(), mEntities.end());
    }

    struct EntityProxy
    {
        Entity &entity;
        EntityManager *manager = nullptr;

        EntityProxy(Entity &entity, EntityManager *manager)
            : entity(entity)
            , manager(manager)
        {
        }

        template <typename Component, typename... Args>
        EntityProxy & addComponent(Args &&... args)
        {
            manager->addComponent<Component>(
                entity, std::forward<Args>(args)...
            );
            return *this;
        }
    };

    auto addEntity()
    {
        return EntityProxy {
            mEntities.emplace_back(mEntityId++),
            this
        };
    }

private:
    Entity & findEntity(EntityId id)
    {
        const auto iter = mEntities.find(id);
        assert(iter != mEntities.end());
        return *iter;
    }

    template <typename Component, typename... Args>
    EntityManager & addComponent(Entity &entity, Args &&... args)
    {
        auto &storage = componentStorage<Component>();
        if(entity.components[storage.mask_bit])
            USAGI_THROW(std::runtime_error(
                "Entity already has the specified component."));
        entity.components[storage.mask_bit].flip();
        storage.insert(entity.id, { std::forward<Args>(args)... });
        return *this;
    }

    template <typename Component>
    EntityManager & removeComponent(Entity &entity)
    {
        auto &storage = componentStorage<Component>();
        if(!entity.components[storage.mask_bit])
            USAGI_THROW(std::runtime_error(
                "Entity does not have the specified component."));
        entity.components[storage.mask_bit].flip();
        storage.erase(entity.id);
        return *this;
    }

public:
    template <typename Component, typename... Args>
    EntityManager & addComponent(const EntityId id, Args &&... args)
    {
        auto &entity = findEntity(id);
        return addComponent<Component>(entity, std::forward<Args>(args)...);
    }

    template <typename Component>
    EntityManager & removeComponent(const EntityId id)
    {
        auto &entity = findEntity(id);
        return removeComponent<Component>(entity);
    }
};
}
