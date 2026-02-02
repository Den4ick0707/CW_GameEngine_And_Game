#ifndef COURSEWORK_GAMEENGINE_SPRITERENDENER_H
#define COURSEWORK_GAMEENGINE_SPRITERENDENER_H

#include "Component.h"
#include "GameObject.h"
#include "../../Graphics/Renderer2D.h"
#include "../../Graphics/Textures.h"

namespace Engine {
    namespace Scene {
        class SpriteRenderer : public Component {
        public:
            glm::vec4 Color;
            std::shared_ptr<Graphics::Textures> Texture;

            SpriteRenderer(const glm::vec4 &color)
                : Color(color), Texture(nullptr) {
            }

            SpriteRenderer(std::shared_ptr<Graphics::Textures> texture)
                : Color(1.0f), Texture(texture) {
            }

            void OnRender() override {
                if (Texture) {
                   Renderer2D::DrawQuad(m_Owner->GetTransform(), Texture, Color);
                } else {
                   Renderer2D::DrawQuad(m_Owner->GetTransform(), Color);
                }
            }
        };
    }
}

#endif //COURSEWORK_GAMEENGINE_SPRITERENDENER_H
