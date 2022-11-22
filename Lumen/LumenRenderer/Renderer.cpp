//
// Created by enzoc on 30/09/2022.
//
#define NOMINMAX
#define MT

#include "Renderer.hpp"
#include <glm/glm.hpp>
#include <tbb/tbb.h>


namespace LumenRender {

namespace Utils {

    static auto BackgroundColor(const Ray &ray) -> glm::vec3
    {
        auto t = 0.5F * (ray.Direction.y + 1.0F);
        return (1.0F - t) * glm::vec3(1.0F, 1.0F, 1.0F) + t * glm::vec3(0.5F, 0.7F, 1.0F);
    }

    static auto ConvertToRGBA(glm::vec4 color) -> uint32_t
    {
        auto r = static_cast<uint8_t>(color.r * 255U);
        auto g = static_cast<uint8_t>(color.g * 255U);
        auto b = static_cast<uint8_t>(color.b * 255U);
        auto a = static_cast<uint8_t>(color.a * 255U);
        return static_cast<uint32_t>(a << 24U | b << 16U | g << 8U | r);
    }

}// namespace Utils


void Renderer::Render(const LumenRender::Camera &camera, const LumenRender::Scene &scene)
{
    m_ActiveCamera = &camera;
    m_ActiveScene = &scene;

    if (m_FrameSample == 1) {
        memset(m_AccumulationBuffer,
          0,
          static_cast<uint64_t>(m_Image->GetWidth() * m_Image->GetHeight()) * sizeof(glm::vec4));
    }

#ifdef MT
#if 1
    if (m_ImageData[0] == 0) {
        tbb::parallel_for(tbb::blocked_range2d<uint32_t>(0, m_Image->GetHeight(), 0, m_Image->GetWidth()),
          [&](const tbb::blocked_range2d<uint32_t> &range) {
              for (uint32_t y = range.rows().begin(); y != range.rows().end(); ++y) {
                  for (uint32_t x = range.cols().begin(); x != range.cols().end(); ++x) {

                      glm::vec4 color = PerPixel(x, y);

                      if (m_Settings.Accumulate) {
                          m_AccumulationBuffer[y * m_Image->GetWidth() + x] += color;

                          glm::vec4 accumulatedColor = m_AccumulationBuffer[y * m_Image->GetWidth() + x];
                          accumulatedColor /= static_cast<float>(m_FrameSample);

                          accumulatedColor = glm::clamp(accumulatedColor, 0.0F, 1.0F);
                          m_ImageData[y * m_Image->GetWidth() + x] = Utils::ConvertToRGBA(accumulatedColor);
                      } else {
                          color = glm::clamp(color, 0.0F, 1.0F);
                          m_ImageData[y * m_Image->GetWidth() + x] = Utils::ConvertToRGBA(color);
                      }
                  }
              }
          });
    }
#else// Tiled Rendering


    constexpr uint32_t TILES = 32;

    const uint32_t tileWidth = glm::ceil(m_Image->GetWidth() / TILES);
    const uint32_t tileHeight = glm::ceil(m_Image->GetHeight() / TILES);
    const uint32_t numTile = tileWidth * tileHeight;

    tbb::parallel_for(
      tbb::blocked_range3d<uint32_t>(0, TILES, 0, TILES, 0, numTile), [&](const tbb::blocked_range3d<uint32_t> &range) {
          for (uint32_t y = range.pages().begin(); y != range.pages().end(); y++) {
              for (uint32_t x = range.rows().begin(); x != range.rows().end(); x++) {
                  for (uint32_t i = range.cols().begin(); i != range.cols().end(); i++) {

                      uint32_t const tileX = i % tileWidth;
                      uint32_t const tileY = i / tileWidth;

                      uint32_t const idx = m_Image->GetWidth() * (y * tileHeight + tileY) + (x * tileWidth + tileX);

                      glm::vec4 color = PerPixel(x, y);

                      if (m_Settings.Accumulate) {
                          m_AccumulationBuffer[idx] += color;

                          glm::vec4 accumulatedColor = m_AccumulationBuffer[idx];
                          accumulatedColor /= static_cast<float>(m_FrameSample);

                          accumulatedColor = glm::clamp(accumulatedColor, 0.0F, 1.0F);
                          m_ImageData[idx] = Utils::ConvertToRGBA(accumulatedColor);
                      } else {
                          color = glm::clamp(color, 0.0F, 1.0F);
                          m_ImageData[idx] = Utils::ConvertToRGBA(color);
                      }
                  }
              }
          }
      });
#endif
#else
    for (uint32_t y = 0; y < m_Image->GetHeight(); y++) {
        for (uint32_t x = 0; x < m_Image->GetWidth(); x++) {

            glm::vec4 color = PerPixel(x, y);

            if (m_Settings.Accumulate) {
                m_AccumulationBuffer[y * m_Image->GetWidth() + x] += color;

                glm::vec4 accumulatedColor = m_AccumulationBuffer[y * m_Image->GetWidth() + x];
                accumulatedColor /= static_cast<float>(m_FrameSample);

                accumulatedColor = glm::clamp(accumulatedColor, 0.0F, 1.0F);
                m_ImageData[y * m_Image->GetWidth() + x] = Utils::ConvertToRGBA(accumulatedColor);
            } else {
                color = glm::clamp(color, 0.0F, 1.0F);
                m_ImageData[y * m_Image->GetWidth() + x] = Utils::ConvertToRGBA(color);
            }
        }
    }
#endif

    m_Image->SetData(m_ImageData);


    if (m_Settings.Accumulate) {
        m_FrameSample++;
    } else {
        m_FrameSample = 1;
    }
}


void Renderer::MemAlloc()
{
    if (m_Settings.Accumulate) {
        memset(m_AccumulationBuffer,
          0,
          static_cast<uint64_t>(m_Image->GetWidth() * m_Image->GetHeight()) * sizeof(glm::vec4));
    }

    memset(m_ImageData, 0, static_cast<uint64_t>(m_Image->GetWidth() * m_Image->GetHeight()) * sizeof(uint32_t));
}


void Renderer::OnResize(uint32_t width, uint32_t height)
{
    if (m_Image) {
        // no resize needed
        if (m_Image->GetWidth() == width && m_Image->GetHeight() == height) { return; }

        m_Image->Resize(width, height);
    } else {
        m_Image = std::make_shared<Lumen::Image>(width, height, Lumen::ImageFormat::RGBA);
    }

    _aligned_free(m_ImageData);
    _aligned_free(m_AccumulationBuffer);

    m_ImageData = static_cast<uint32_t *>(
      _aligned_malloc(m_Image->GetWidth() * m_Image->GetHeight() * sizeof(uint32_t), sizeof(uint32_t)));

    m_AccumulationBuffer = static_cast<glm::vec4 *>(
      _aligned_malloc(m_Image->GetWidth() * m_Image->GetHeight() * sizeof(glm::vec4), sizeof(glm::vec4)));
}


auto Renderer::TraceRay(LumenRender::Ray &ray) -> HitRecords
{

    constexpr float tmax = std::numeric_limits<float>::max();

    m_ActiveScene->Hit(ray, tmax);

    return ray.m_Record;
}

auto Renderer::PerPixel(const uint32_t &x, const uint32_t &y) -> glm::vec4
{

    Ray ray(m_ActiveCamera->GetPosition(), m_ActiveCamera->GetRayDirections()[y * m_Image->GetWidth() + x]);
    TraceRay(ray);


    // if we miss the scene, return the background color
    if (ray.m_Record.m_T < 0.0F) {
        return { Utils::BackgroundColor(ray), 1.0F };

    } else {

        glm::vec3 color(0.0F);
        glm::vec3 const lightDir = glm::normalize(glm::vec3(-1));
        float const lightIntensity = glm::max(glm::dot(ray.m_Record.m_Normal, -lightDir), 0.0F);// == cos(angle)
        glm::vec3 sphereColor = glm::vec3(1.0F, 0.4F, 0.3F);
        sphereColor *= lightIntensity;
        color += sphereColor;

        return { color, 1.0F };
    }
}


}// namespace LumenRender