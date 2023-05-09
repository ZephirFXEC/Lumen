// Copyright (c) 2022.
// Enzo Crema
// All rights reserved


#ifndef LUMEN_RENDERER_HPP
#define LUMEN_RENDERER_HPP

#include "../../LumenCore/Image.hpp"
#include "../../LumenCore/Random.hpp"

#include "Camera.hpp"
#include "Ray.hpp"
#include "Scene/Scene.hpp"

#include "Utility/Utility.hpp"
#include <glm/glm.hpp>

namespace LumenRender {

struct Settings
{
    bool Accumulate = false;
};

class Renderer
{
  public:
    Renderer() = default;

    void Render(const LumenRender::Camera &camera, const LumenRender::Scene &scene);
    void OnResize(uint32_t width, uint32_t height);
    void MemAlloc();
    void ResetFrame() { mFrameSample = 1; }


    __forceinline const std::shared_ptr<Lumen::Image> GetFinalImage() const { return pImage; }
    __forceinline Settings &GetSettings() { return mSettings; }


  private:
    HitRecords &TraceRay(LumenRender::Ray &ray);
    glm::vec4 PerPixel(const uint32_t &x, const uint32_t &y);


    std::shared_ptr<Lumen::Image> pImage = nullptr;
    Settings mSettings = {};

    const Camera *pActiveCamera = nullptr;
    const Scene *pActiveScene = nullptr;

    uint32_t *pImageData = nullptr;
    glm::vec4 *pAccumulationBuffer = nullptr;

    uint32_t mFrameSample = 1;
    HitRecords mHitRecords = {};
};

}// namespace LumenRender

#endif// LUMEN_RENDERER_HPP