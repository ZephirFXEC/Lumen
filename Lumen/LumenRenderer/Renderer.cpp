// Copyright (c) 2022.
// Enzo Crema
// All rights reserved


#include "Ray.hpp"
#define NOMINMAX

#include "Renderer.hpp"
#include <tbb/tbb.h>
#include <bitset>


constexpr glm::vec3 lightDir = glm::vec3(-0.57735026918962584, -0.57735026918962584, -0.57735026918962584);
constexpr float tmax = std::numeric_limits<float>::max();

namespace LumenRender {

namespace {

    auto BackgroundColor(const Ray &ray) -> glm::vec3
    {
        const float t = 0.5F * (ray.mDirection.y + 1.0F);
        return (1.0F - t) * glm::vec3(1.0F, 1.0F, 1.0F) + t * glm::vec3(0.5F, 0.7F, 1.0F);
    }

    auto ConvertToRGBA(const glm::vec4 *color) -> uint32_t
    {
        uint8_t out[4] = {};
        const __m128 x = _mm_mul_ps(_mm_load_ps((float *)color), _mm_set_ps1(255));
        __m128i y = _mm_cvtps_epi32(x);
        y = _mm_packus_epi32(y, y);
        y = _mm_packus_epi16(y, y);
        *(int *)out = _mm_cvtsi128_si32(y);

        const std::bitset<32> bits = (*reinterpret_cast<uint32_t *>(out));
        return bits.to_ulong();
    }

}// namespace


void Renderer::Render(const LumenRender::Camera &camera, const LumenRender::Scene &scene)
{
    pActiveCamera = &camera;
    pActiveScene = &scene;

    if (mFrameSample == 1) {
        memset(pAccumulationBuffer, 0, static_cast<uint64_t>(pImage->m_Width * pImage->m_Height) * sizeof(glm::vec4));
    }

    if (pImageData[0] == NULL) {
        // Create a task group for the parallel tasks
        tbb::task_group group;

        constexpr uint8_t TASK_COUNT = 8;

        // Calculate the size of the range for each task
        const uint16_t rangeSize = pImage->m_Height / TASK_COUNT;

        // Create and launch TASK_COUNT tasks
        for (uint8_t i = 0; i < TASK_COUNT; ++i) {
            // Calculate the starting and ending rows for this task
            const uint16_t startRow = i * rangeSize;
            const uint16_t endRow = startRow + rangeSize;

            // Add the task to the group
            group.run([this, startRow, endRow]() {
                // Use a parallel_for loop to process the rows for this task
                tbb::parallel_for(
                  tbb::blocked_range<uint32_t>(startRow, endRow), [this](const tbb::blocked_range<uint32_t> &range) {
                      for (uint32_t y = range.begin(); y != range.end(); ++y) {
                          for (uint32_t x = 0; x < pImage->m_Width; ++x) {
                              const glm::vec4 color = glm::clamp(PerPixel(x, y), 0.0F, 1.0F);

                              if (mSettings.Accumulate) {
                                  pAccumulationBuffer[y * pImage->m_Width + x] += color;

                                  glm::vec4 accumulatedColor = pAccumulationBuffer[y * pImage->m_Width + x];
                                  accumulatedColor /= static_cast<float>(mFrameSample);

                                  accumulatedColor = glm::clamp(accumulatedColor, 0.0F, 1.0F);
                                  pImageData[y * pImage->m_Width + x] = ConvertToRGBA(&accumulatedColor);
                              } else {
                                  pImageData[y * pImage->m_Width + x] = ConvertToRGBA(&color);
                              }
                          }
                      }
                  });
            });
        }

        // Wait for all tasks to complete
        group.wait();
    }

    pImage->SetData(pImageData);

    if (mSettings.Accumulate) { mFrameSample++; }
}


void Renderer::MemAlloc()// TODO: Use TBB Malloc
{
    if (mSettings.Accumulate) {
        memset(pAccumulationBuffer, 0, static_cast<uint64_t>(pImage->m_Width * pImage->m_Height) * sizeof(glm::vec4));
    }

    memset(pImageData, NULL, static_cast<uint64_t>(pImage->m_Width * pImage->m_Height) * sizeof(uint32_t));
}


void Renderer::OnResize(uint32_t width, uint32_t height)
{
    if (pImage) {
        // no resize needed
        if (pImage->m_Width == width && pImage->m_Height == height) { return; }

        pImage->Resize(width, height);
    } else {
        pImage = std::make_shared<Lumen::Image>(width, height, Lumen::ImageFormat::RGBA);
    }

    delete[] pImageData;
    delete[] pAccumulationBuffer;

    pImageData = new uint32_t[static_cast<uint64_t>(pImage->m_Width * pImage->m_Height)];

    pAccumulationBuffer = new glm::vec4[static_cast<uint64_t>(pImage->m_Width * pImage->m_Height)];
}


HitRecords &Renderer::TraceRay(LumenRender::Ray &ray)
{
    HitRecords hitRecords{};
    ray.pRecord = &hitRecords;

    pActiveScene->Hit(ray, tmax);
    return *ray.pRecord;
}

glm::vec4 Renderer::PerPixel(const uint32_t &x, const uint32_t &y)
{

    Ray ray = {};
    ray.mOrigin = pActiveCamera->mPosition;
    ray.mDirection = pActiveCamera->mRayDirections[y * pImage->m_Width + x];

    TraceRay(ray);

    switch (ray.pRecord->mType) {
    case Type::MISS:
        return { BackgroundColor(ray), 1.0F };


    case Type::HIT:
        // Calculate the light direction and intensity once for all pixels
        float const lightIntensity = glm::max(glm::dot(ray.pRecord->mNormal, -lightDir), 0.0F);// == cos(angle)

        // Pre-calculate the sphere color
        glm::vec3 sphereColor = glm::vec3(1.0F, 0.4F, 0.3F);
        sphereColor *= lightIntensity;

        glm::vec3 color(0.0F);
        color += sphereColor;
        return { color, 1.0F };
    }

    return { 0.0F, 0.0F, 0.0F, 1.0F };
}
}// namespace LumenRender