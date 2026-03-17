//
// Created by onigirya on 17.03.26.
//

#ifndef COURSE_WORK_DARYEV_REGISTRY_H
#define COURSE_WORK_DARYEV_REGISTRY_H

#include "entity.h"
#include "component_pool.h"
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <functional>

namespace Engine::Scene {
    class Registry {
    public:
        // ── Entity lifecycle ──────────────────────────────────────────────

        Entity Create() {
            EntityID id;
            if (!m_FreeList.empty()) {
                // Перевикористовуємо звільнені ID
                id = m_FreeList.back();
                m_FreeList.pop_back();
            } else {
                id = m_NextID++;
            }
            m_Alive.push_back(id);
            return Entity{id};
        }

        void Destroy(EntityID entity) {
            // Видаляємо всі компоненти цього entity
            for (auto &[type, pool]: m_Pools)
                pool->Remove(entity);

            // Видаляємо зі списку живих
            m_Alive.erase(
                std::remove(m_Alive.begin(), m_Alive.end(), entity),
                m_Alive.end()
            );
            m_FreeList.push_back(entity);
        }

        // ── Component API ─────────────────────────────────────────────────

        template<typename T, typename... Args>
        T &Add(EntityID entity, Args &&... args) {
            auto &pool = GetOrCreatePool<T>();
            pool.Add(entity, T{std::forward<Args>(args)...});
            return pool.Get(entity);
        }

        template<typename T>
        void Remove(EntityID entity) {
            GetOrCreatePool<T>().Remove(entity);
        }

        template<typename T>
        bool Has(EntityID entity) const {
            auto it = m_Pools.find(typeid(T));
            if (it == m_Pools.end()) return false;
            return static_cast<ComponentPool<T> *>(it->second.get())->Has(entity);
        }

        template<typename T>
        T &Get(EntityID entity) {
            return GetOrCreatePool<T>().Get(entity);
        }

        // ── View — ітерація по entities з потрібними компонентами ─────────
        //
        // Використання:
        //   registry.View<TransformComponent, SpriteRendererComponent>(
        //       [](EntityID e, TransformComponent& t, SpriteRendererComponent& s) {
        //           // ...
        //       });

        template<typename... Components, typename Func>
        void View(Func &&func) {
            // Беремо пул найменшого компонента для ітерації
            // (спрощена версія — ітеруємось по першому, фільтруємо решту)
            auto &primaryPool = GetOrCreatePool<std::tuple_element_t<0, std::tuple<Components...> > >();

            for (EntityID entity: primaryPool.GetEntities()) {
                if ((Has<Components>(entity) && ...)) {
                    func(entity, Get<Components>(entity)...);
                }
            }
        }

        const std::vector<EntityID> &GetAllEntities() const {
            return m_Alive;
        }

    private:
        template<typename T>
        ComponentPool<T> &GetOrCreatePool() {
            auto key = std::type_index(typeid(T));
            auto it = m_Pools.find(key);
            if (it == m_Pools.end()) {
                m_Pools[key] = std::make_unique<ComponentPool<T> >();
            }
            return *static_cast<ComponentPool<T> *>(m_Pools[key].get());
        }

    private:
        EntityID m_NextID = 0;
        std::vector<EntityID> m_Alive;
        std::vector<EntityID> m_FreeList; // переробка звільнених ID

        std::unordered_map<std::type_index, std::unique_ptr<IComponentPool> > m_Pools;
    };
}
#endif //COURSE_WORK_DARYEV_REGISTRY_H
