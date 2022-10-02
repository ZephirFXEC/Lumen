//
// Created by enzoc on 30/09/2022.
//

#ifndef LUMEN_RENDERER_HPP
#define LUMEN_RENDERER_HPP

#include "../../LumenCore/Image.hpp"
#include "../../LumenCore/Random.hpp"
#include "Camera.hpp"
#include "Ray.hpp"

#include <memory>

namespace LumenRender {

    class Renderer {
    public:
        Renderer() = default;

        void Render(const LumenRender::Camera& camera);

        void OnResize(uint32_t width, uint32_t height);

        std::shared_ptr<Lumen::Image> GetFinalImage() const { return m_Image; }

    private:
        static glm::vec4 TraceRay(const LumenRender::RayDesc& ray);

    private:
        std::shared_ptr<Lumen::Image> m_Image;
        uint32_t *m_ImageData = nullptr;
    };

} // LumenRender

#endif //LUMEN_RENDERER_HPP