#pragma once

#include <cstdint>
#include <limits>

namespace Engine::Scene {

    /// @brief Унікальний ідентифікатор сутності.
    using EntityID = uint32_t;

    /// @brief "Порожня" сутність — аналог nullptr.
    inline constexpr EntityID NULL_ENTITY =
        std::numeric_limits<EntityID>::max();

    /// @brief Легка обгортка над EntityID.
    /// @details Дозволяє писати:
    /// @code
    ///   Entity player = registry.Create();
    ///   if (player.IsValid()) { ... }
    /// @endcode
    struct Entity {
        EntityID ID = NULL_ENTITY;

        [[nodiscard]] bool IsValid() const { return ID != NULL_ENTITY; }

        operator EntityID() const { return ID; }

        bool operator==(const Entity& o) const { return ID == o.ID; }
        bool operator!=(const Entity& o) const { return ID != o.ID; }
        bool operator< (const Entity& o) const { return ID <  o.ID; }
    };

} // namespace Engine::Scene