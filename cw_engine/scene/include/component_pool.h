#pragma once

#include "entity.h"
#include <vector>
#include <cassert>
#include <limits>

namespace Engine::Scene {

    // ── Базовий інтерфейс ─────────────────────────────────────────────────────

    /// @brief Нетипізований базовий клас для зберігання в одному контейнері.
    class IComponentPool {
    public:
        virtual ~IComponentPool() = default;

        virtual void Remove(EntityID entity)          = 0;
        virtual bool Has   (EntityID entity) const    = 0;
        virtual void Clear ()                         = 0;
        virtual size_t Size() const                   = 0;
    };

    // ── Sparse Set ────────────────────────────────────────────────────────────

    /// @brief Типізований пул компонентів одного типу (Sparse Set).
    ///
    /// Складається з трьох паралельних масивів:
    ///   sparse[entityID]  → індекс у dense / components, або INVALID
    ///   dense[i]          → entityID
    ///   components[i]     → компонент
    ///
    /// Складність операцій:
    ///   Has / Get        — O(1)
    ///   Add / Remove     — O(1) amortized
    ///   Ітерація         — cache-friendly (щільний масив)
    template<typename T>
    class ComponentPool final : public IComponentPool {
    public:
        static constexpr uint32_t INVALID =
            std::numeric_limits<uint32_t>::max();

        // ── Основні операції ──────────────────────────────────────────────

        /// @brief Додати або оновити компонент для entity.
        template<typename... Args>
        T& Emplace(EntityID entity, Args&&... args) {
            if (Has(entity)) {
                // Оновлюємо існуючий
                components[sparse[entity]] = T{ std::forward<Args>(args)... };
                return components[sparse[entity]];
            }

            if (entity >= sparse.size())
                sparse.resize(entity + 1, INVALID);

            sparse[entity] = static_cast<uint32_t>(dense.size());
            dense.push_back(entity);
            components.emplace_back(std::forward<Args>(args)...);
            return components.back();
        }

        /// @brief Видалити компонент. O(1) через swap з останнім.
        void Remove(EntityID entity) override {
            if (!Has(entity)) return;

            uint32_t idx  = sparse[entity];
            EntityID last = dense.back();

            // Переносимо останній елемент на місце видаленого
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

        void Clear() override {
            sparse.clear();
            dense.clear();
            components.clear();
        }

        size_t Size() const override { return dense.size(); }

        // ── Ітерація ──────────────────────────────────────────────────────

        /// @brief Щільний масив компонентів — cache-friendly ітерація.
        std::vector<T>&        GetAll()      { return components; }
        const std::vector<T>&  GetAll() const{ return components; }

        /// @brief Паралельний масив EntityID.
        std::vector<EntityID>& GetEntities() { return dense; }
        const std::vector<EntityID>& GetEntities() const { return dense; }

    private:
        std::vector<uint32_t> sparse;      // entityID  → index
        std::vector<EntityID> dense;       // index     → entityID
        std::vector<T>        components;  // index     → component
    };

} // namespace Engine::Scene