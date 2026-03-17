//
// Created by onigirya on 17.03.26.
//

#ifndef COURSE_WORK_DARYEV_ENTITY_H
#define COURSE_WORK_DARYEV_ENTITY_H

#include <cstdint>
#include <limits>

namespace Engine::Scene {

    using EntityID = uint32_t;

    inline constexpr EntityID NULL_ENTITY = std::numeric_limits<EntityID>::max();

    /// @brief Легка обгортка над EntityID.
    /// Дозволяє писати: Entity e = registry.Create();
    struct Entity {
        EntityID ID = NULL_ENTITY;

        bool IsValid() const { return ID != NULL_ENTITY; }
        operator EntityID() const { return ID; }
        bool operator==(const Entity& o) const { return ID == o.ID; }
    };
}
#endif //COURSE_WORK_DARYEV_ENTITY_H