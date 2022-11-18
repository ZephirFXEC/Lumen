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

#include "Utility/Utility.hpp"


namespace LumenRender {

    struct Settings {
        bool Accumulate = true;
    };

    class Renderer {
    public:
        Renderer() = default;

        void Render(const LumenRender::Camera &camera, const LumenRender::Scene &scene);

        void OnResize(uint32_t width, uint32_t height);

        [[nodiscard]] auto GetFinalImage() const -> std::shared_ptr<Lumen::Image> { return m_Image; }

        auto GetSettings() -> Settings & { return m_Settings; }

        void ResetFrame() { m_FrameSample = 1; }

    private:

        auto TraceRay(LumenRender::Ray &ray) -> HitRecords;

        static auto Miss(LumenRender::Ray &ray) -> HitRecords;

        auto PerPixel(uint32_t x, uint32_t y) -> glm::vec4;


        std::shared_ptr<Lumen::Image> m_Image;
        Settings m_Settings;

        const Camera *m_ActiveCamera{};
        const Scene *m_ActiveScene{};

        uint32_t *m_ImageData = nullptr;
        glm::vec4 *m_AccumulationBuffer = nullptr;

        uint32_t m_FrameSample = 1;
    };

} // LumenRender

#endif //LUMEN_RENDERER_HPP