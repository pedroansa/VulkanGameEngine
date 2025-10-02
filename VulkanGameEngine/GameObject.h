#pragma once

#include "Model.h"

#include <memory>
#include <unordered_map>

//glm
#include "glm/gtc/matrix_transform.hpp"

namespace app {

    struct TransformComponent {
        glm::vec3 translation{};  // (position offset)
        glm::vec3 scale{ 1.f, 1.f, 1.f };
        glm::vec3 rotation;

        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
 // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
 // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 mat4();
        glm::mat3 normalMatrix();

    };

    class GameObject {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, GameObject>;

        static GameObject createGameObject() {
            static id_t currentId = 0;
            return GameObject{ currentId++ };
        }

        GameObject(const GameObject&) = delete;
        GameObject& operator=(const GameObject&) = delete;
        GameObject(GameObject&&) = default;
        GameObject& operator=(GameObject&&) = default;

        id_t getId() { return id; }

        std::shared_ptr<Model> model{};
        glm::vec3 color{};
        TransformComponent transform{};

    private:
        GameObject(id_t objId) : id{ objId } {}

        id_t id;
    };
}  // namespace lve