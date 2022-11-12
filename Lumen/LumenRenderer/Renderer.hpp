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
#include "Scene/Object.hpp"
#include "Accelerators/Bvh.hpp"

#include <memory>
#include <glm/glm.hpp>


namespace LumenRender {

    struct Settings {
        bool Accumulate = true;
        uint32_t MaxSample = 100;
    };

    class Renderer {
    public:
        Renderer() = default;

        void Render(const LumenRender::Camera &camera, const LumenRender::Scene &scene);

        void OnResize(uint32_t width, uint32_t height);

        std::shared_ptr<Lumen::Image> GetFinalImage() const { return m_Image; }

        Settings &GetSettings() { return m_Settings; }

        void ResetFrame() { m_FrameSample = 1; }

    private:
        HitRecords TraceRay(LumenRender::Ray &ray);

        static HitRecords Miss(LumenRender::Ray &ray);

        glm::vec4 PerPixel(uint32_t x, uint32_t y);

    private:
        std::shared_ptr<Lumen::Image> m_Image;
        Settings m_Settings;

        const Camera *m_ActiveCamera{};
        const Scene *m_ActiveScene{};

        uint32_t* m_ImageData = nullptr;
        glm::vec4* m_AccumulationBuffer = nullptr;

        uint32_t m_FrameSample = 1;
    };

} // LumenRender

#endif //LUMEN_RENDERER_HPP