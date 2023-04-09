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
#include "Structure/Mesh.hpp"



namespace LumenRender {

    class Renderer {
    public:
        Renderer() = default;

        void Render(const Camera &camera, const Mesh &objects);

        void OnResize(uint32_t width, uint32_t height);

        __forceinline std::shared_ptr<Lumen::Image> GetFinalImage() const { return m_Image; }

    private:

        static HitRecords ClosestHit(const Ray &ray, const float &dist, uint32_t ObjectID);

        static HitRecords Miss(const Ray &ray);

        HitRecords TraceRay(const Ray &ray);

        glm::vec4 PerPixel(int x, int y);


    private:
        std::shared_ptr<Lumen::Image> m_Image;

        const Mesh *m_Objects{nullptr};
        const Camera *m_ActiveCamera{nullptr};

        uint32_t *m_ImageData{nullptr};


    };

} // LumenRender

#endif //LUMEN_RENDERER_HPP