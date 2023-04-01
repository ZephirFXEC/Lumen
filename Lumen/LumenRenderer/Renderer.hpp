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

        void Render(const Camera &camera, const Object &objects);

        void OnResize(uint32_t width, uint32_t height);

        [[nodiscard]] std::shared_ptr<Lumen::Image> GetFinalImage() const { return m_Image; }

    private:
        HitRecords TraceRay(const Ray &ray);

        static HitRecords ClosestHit(const Ray &ray, float dist, uint32_t ObjectID);

        static HitRecords Miss(const Ray &ray);

        glm::vec4 PerPixel(uint32_t x, uint32_t y);

    private:
        std::shared_ptr<Lumen::Image> m_Image;

        const Object *m_Objects = {};
        const Camera *m_ActiveCamera = nullptr;

        uint32_t *m_ImageData = nullptr;

    };

} // LumenRender

#endif //LUMEN_RENDERER_HPP