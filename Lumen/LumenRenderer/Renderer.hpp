//
// Created by enzoc on 30/09/2022.
//

#ifndef LUMEN_RENDERER_HPP
#define LUMEN_RENDERER_HPP

#include <memory>
#include <glm/glm.hpp>

#include "../../LumenCore/Image.hpp"
#include "../../LumenCore/Random.hpp"

#include "Camera.hpp"
#include "Ray.hpp"
#include "Structure/Object.hpp"



namespace LumenRender {

    class Renderer {
    public:
        Renderer() = default;

        void Render(const LumenRender::Camera& camera,  const Triangle &objects);

        void OnResize(uint32_t width, uint32_t height);

        Lumen::Image* GetFinalImage() const { return m_Image; }

    private:
        HitRecords TraceRay(const LumenRender::Ray& ray);
        HitRecords ClosestHit(const LumenRender::Ray& ray, float dist, uint32_t ObjectID);
        static HitRecords Miss(const LumenRender::Ray& ray);

        glm::vec4 PerPixel(uint32_t x, uint32_t y);

    private:
        Lumen::Image* m_Image;

        const Triangle* m_Objects = {};
        const Camera* m_ActiveCamera = nullptr;

        uint32_t *m_ImageData = nullptr;




    };

} // LumenRender

#endif //LUMEN_RENDERER_HPP