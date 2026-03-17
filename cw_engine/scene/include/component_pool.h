//
// Created by onigirya on 17.03.26.
//

#ifndef COURSE_WORK_DARYEV_COMPONENT_POOL_H
#define COURSE_WORK_DARYEV_COMPONENT_POOL_H
#include "entity.h"
#include <vector>
#include <cassert>

namespace Engine::Scene {

    /// @brief Sparse Set — зберігає компоненти одного типу.
    ///
    /// Дві структури:
    ///   sparse[entityID] → індекс у dense/components (або INVALID)
    ///   dense[i]         → entityID
    ///   components[i]    → сам компонент
    ///
    /// Завдяки цьому:
    ///   Has(entity) — O(1)
    ///   Get(entity) — O(1)
    ///   Add/Remove  — O(1) amortized
    ///   Ітерація    — cache-friendly (щільний масив)
    class IComponentPool {
    public:
        virtual ~IComponentPool() = default;
        virtual void Remove(EntityID entity) = 0;
        virtual bool Has(EntityID entity) const = 0;
    };

    template<typename T>
    class ComponentPool : public IComponentPool {
    public:
        static constexpr uint32_t INVALID = std::numeric_limits<uint32_t>::max();

        void Add(EntityID entity, T component) {
            if (Has(entity)) {
                // Якщо вже є — просто оновлюємо
                components[sparse[entity]] = std::move(component);
                return;
            }

            // Розширюємо sparse якщо треба
            if (entity >= sparse.size())
                sparse.resize(entity + 1, INVALID);

            sparse[entity] = static_cast<uint32_t>(dense.size());
            dense.push_back(entity);
            components.push_back(std::move(component));
        }

        void Remove(EntityID entity) override {
            if (!Has(entity)) return;

            // Swap з останнім елементом → O(1) видалення
            uint32_t idx     = sparse[entity];
            EntityID last    = dense.back();

            dense[idx]      = last;
            components[idx] = std::move(components.back());
            sparse[last]    = idx;

            dense.pop_back();
            components.pop_back();
            sparse[entity] = INVALID;
        }

        bool Has(EntityID entity) const override {
            return entity < sparse.size() && sparse[entity] != INVALID;
        }

        T& Get(EntityID entity) {
            assert(Has(entity) && "Entity does not have this component!");
            return components[sparse[entity]];
        }

        const T& Get(EntityID entity) const {
            assert(Has(entity) && "Entity does not have this component!");
            return components[sparse[entity]];
        }

        // Для ітерації у System-ах
        std::vector<T>&        GetAll()       { return components; }
        std::vector<EntityID>& GetEntities()  { return dense; }
        size_t                 Size() const   { return dense.size(); }

    private:
        std::vector<uint32_t> sparse;     // entityID → index
        std::vector<EntityID> dense;      // index → entityID
        std::vector<T>        components; // index → component
    };
}
#endif //COURSE_WORK_DARYEV_COMPONENT_POOL_H