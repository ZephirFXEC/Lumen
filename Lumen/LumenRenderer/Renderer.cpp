//
// Created by enzoc on 30/09/2022.
//
#define NOMINMAX
#include "Renderer.hpp"
#include <tbb/tbb.h>
#include <glm/glm.hpp>


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
    memset(
      m_AccumulationBuffer, 0, static_cast<uint64_t>(m_Image->GetWidth() * m_Image->GetHeight()) * sizeof(glm::vec4));
  }

#if 0
        tbb::parallel_for(tbb::blocked_range2d<uint32_t>(0, m_Image->GetHeight(), 0, m_Image->GetWidth()),
                          [&](const tbb::blocked_range2d<uint32_t> &range) {
                              for (uint32_t y = range.rows().begin(); y != range.rows().end(); y++) {
                                  for (uint32_t x = range.cols().begin(); x != range.cols().end(); x++) {

                                      glm::vec4 const color = PerPixel(x, y);
                                      m_AccumulationBuffer[y * m_Image->GetWidth() + x] += color;

                                      glm::vec4 accumulatedColor = m_AccumulationBuffer[y * m_Image->GetWidth() +
                                                                                        x];
                                      accumulatedColor /= static_cast<float>(m_FrameSample);

                                      accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0F), glm::vec4(1.0F));
                                      m_ImageData[y * m_Image->GetWidth() + x] = Utils::ConvertToRGBA(
                                              accumulatedColor);

                                  }
                              }
                          });
#else// Tiled Rendering


#define TILES 32

  const uint32_t tileWidth = m_Image->GetWidth() / TILES;
  const uint32_t tileHeight = m_Image->GetHeight() / TILES;
  const uint32_t numTile = tileWidth * tileHeight;

  tbb::parallel_for(
    tbb::blocked_range3d<uint32_t>(0, TILES, 0, TILES, 0, numTile), [&](const tbb::blocked_range3d<uint32_t> &range) {
      for (uint32_t y = range.pages().begin(); y != range.pages().end(); y++) {
        for (uint32_t x = range.rows().begin(); x != range.rows().end(); x++) {
          for (uint32_t i = range.cols().begin(); i != range.cols().end(); i++) {
            uint32_t const tileX = i % tileWidth;
            uint32_t const tileY = i / tileWidth;

            uint32_t const idx = m_Image->GetWidth() * (y * tileHeight + tileY) + (x * tileWidth + tileX);


            glm::vec4 const color = PerPixel(x * tileWidth + tileX, y * tileHeight + tileY);
            m_AccumulationBuffer[idx] += color;

            glm::vec4 accumColor = m_AccumulationBuffer[idx];
            accumColor /= static_cast<float>(m_FrameSample);

            accumColor = glm::clamp(accumColor, glm::vec4(0.0F), glm::vec4(1.0F));
            m_ImageData[idx] = Utils::ConvertToRGBA(accumColor);
          }
        }
      }
    });
#endif

  m_Image->SetData(m_ImageData);


  if (m_Settings.Accumulate) {
    m_FrameSample++;
  } else {
    m_FrameSample = 1;
  }
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

  delete[] m_ImageData;
  delete[] m_AccumulationBuffer;

  m_ImageData = new uint32_t[static_cast<uint64_t>(width * height)];
  m_AccumulationBuffer = new glm::vec4[static_cast<uint64_t>(width * height)];
}

auto Renderer::TraceRay(LumenRender::Ray &ray) -> HitRecords
{

  float const tmax = std::numeric_limits<float>::max();

  if (!m_ActiveScene->Hit(ray, tmax)) { return Miss(ray); }

  return ray.m_Record;
}

auto Renderer::PerPixel(uint32_t x, uint32_t y) -> glm::vec4
{

  Ray ray(m_ActiveCamera->GetPosition(), m_ActiveCamera->GetRayDirections()[y * m_Image->GetWidth() + x]);

  glm::vec3 color(0.0F);

  TraceRay(ray);

  // if we miss the scene, return the background color
  if (ray.m_Record.m_T < 0.0F) { color = Utils::BackgroundColor(ray); }

  glm::vec3 const lightDir = glm::normalize(glm::vec3(-1));
  float const lightIntensity = glm::max(glm::dot(ray.m_Record.m_Normal, -lightDir), 0.0F);// == cos(angle)
  glm::vec3 sphereColor = glm::vec3(1.0F, 0.4F, 0.3F);
  sphereColor *= lightIntensity;
  color += sphereColor;

  return { color, 1.0F };
}


auto Renderer::Miss(Ray &ray) -> HitRecords
{
  ray.m_Record.m_T = -1.0F;
  return ray.m_Record;
}


}// namespace LumenRender