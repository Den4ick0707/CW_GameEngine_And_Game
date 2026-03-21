#pragma once

#include "entity.h"
#include "component_pool.h"

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <functional>
#include <vector>
#include <algorithm>
#include <string>
#include <cassert>

namespace Engine::Scene {

    // Forward declaration щоб не включати весь components.h тут
    struct TagComponent;

    class Registry {
    public:
        Registry()  = default;
        ~Registry() = default;

        Registry(const Registry&)            = delete;
        Registry& operator=(const Registry&) = delete;

        // ── Entity Lifecycle ──────────────────────────────────────────────

        /// @brief Створити нову сутність.
        Entity Create() {
            EntityID id = AllocateID();
            m_Alive.push_back(id);
            return Entity{ id };
        }

        /// @brief Знищити сутність і всі її компоненти.
        void Destroy(EntityID entity) {
            if (!IsAlive(entity)) return;

            for (auto& [type, pool] : m_Pools)
                pool->Remove(entity);

            m_Alive.erase(
                std::remove(m_Alive.begin(), m_Alive.end(), entity),
                m_Alive.end());

            m_FreeList.push_back(entity);
        }

        [[nodiscard]] bool IsAlive(EntityID entity) const {
            return std::find(m_Alive.begin(), m_Alive.end(), entity)
                   != m_Alive.end();
        }

        [[nodiscard]] size_t EntityCount() const { return m_Alive.size(); }

        [[nodiscard]] const std::vector<EntityID>& GetAllEntities() const {
            return m_Alive;
        }

        // ── Component API ─────────────────────────────────────────────────

        /// @brief Додати або оновити компонент. Повертає посилання.
        template<typename T, typename... Args>
        T& Add(EntityID entity, Args&&... args) {
            // FIX: викликаємо Emplace а не Add
            return Pool<T>().Emplace(entity, std::forward<Args>(args)...);
        }

        template<typename T>
        void Remove(EntityID entity) {
            Pool<T>().Remove(entity);
        }

        template<typename T>
        [[nodiscard]] bool Has(EntityID entity) const {
            auto it = m_Pools.find(std::type_index(typeid(T)));
            if (it == m_Pools.end()) return false;
            return it->second->Has(entity);
        }

        template<typename T>
        [[nodiscard]] T& Get(EntityID entity) {
            return Pool<T>().Get(entity);
        }

        template<typename T>
        [[nodiscard]] const T& Get(EntityID entity) const {
            return ConstPool<T>().Get(entity);
        }

        /// @brief Повертає nullptr якщо компонента немає.
        template<typename T>
        [[nodiscard]] T* TryGet(EntityID entity) {
            if (!Has<T>(entity)) return nullptr;
            return &Pool<T>().Get(entity);
        }

        // ── View ──────────────────────────────────────────────────────────

        template<typename... Components, typename Func>
        void View(Func&& func) {
            auto& primary = Pool<
                std::tuple_element_t<0, std::tuple<Components...>>
            >();

            for (EntityID id : primary.GetEntities()) {
                if ((Has<Components>(id) && ...))
                    func(id, Get<Components>(id)...);
            }
        }

        template<typename... Components, typename Func>
        void View(Func&& func) const {
            const auto& primary = ConstPool<
                std::tuple_element_t<0, std::tuple<Components...>>
            >();

            for (EntityID id : primary.GetEntities()) {
                if ((Has<Components>(id) && ...))
                    func(id, Get<Components>(id)...);
            }
        }

        // ── Утиліти ───────────────────────────────────────────────────────

        void Clear() {
            for (auto& [type, pool] : m_Pools)
                pool->Clear();
            m_Alive.clear();
            m_FreeList.clear();
            m_NextID = 0;
        }

    private:
        EntityID AllocateID() {
            if (!m_FreeList.empty()) {
                EntityID id = m_FreeList.back();
                m_FreeList.pop_back();
                return id;
            }
            return m_NextID++;
        }

        template<typename T>
        ComponentPool<T>& Pool() {
            auto key = std::type_index(typeid(T));
            auto it  = m_Pools.find(key);
            if (it == m_Pools.end())
                m_Pools[key] = std::make_unique<ComponentPool<T>>();
            return *static_cast<ComponentPool<T>*>(m_Pools[key].get());
        }

        template<typename T>
        const ComponentPool<T>& ConstPool() const {
            auto it = m_Pools.find(std::type_index(typeid(T)));
            assert(it != m_Pools.end() && "Component pool not found!");
            return *static_cast<const ComponentPool<T>*>(it->second.get());
        }

        EntityID m_NextID = 0;
        std::vector<EntityID> m_Alive;
        std::vector<EntityID> m_FreeList;
        std::unordered_map<
            std::type_index,
            std::unique_ptr<IComponentPool>
        > m_Pools;
    };

}