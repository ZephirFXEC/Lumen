//
// Created by enzoc on 30/09/2022.
//

#ifndef LUMEN_RENDERER_HPP
#define LUMEN_RENDERER_HPP

#include "../../LumenCore/Image.hpp"
#include "../../LumenCore/Random.hpp"

#include "Camera.hpp"
#include "Ray.hpp"
#include "Scene/Scene.hpp"

#include <memory>
#include <glm/glm.hpp>


namespace LumenRender {

    class Renderer {
    public:
        Renderer() = default;

        void Render(const LumenRender::Camera& camera, const LumenRender::Scene& scene);

        void OnResize(uint32_t width, uint32_t height);

        std::shared_ptr<Lumen::Image> GetFinalImage() const { return m_Image; }

    private:
        HitRecords TraceRay(const LumenRender::Ray& ray);
        HitRecords ClosestHit(const LumenRender::Ray& ray, float dist, uint32_t ObjectID);
        static HitRecords Miss(const LumenRender::Ray& ray);

        glm::vec4 PerPixel(uint32_t x, uint32_t y);

    private:
        std::shared_ptr<Lumen::Image> m_Image;

        const Scene* m_ActiveScene;
        const Camera* m_ActiveCamera;

        uint32_t *m_ImageData = nullptr;

    };

} // LumenRender

#endif //LUMEN_RENDERER_HPP