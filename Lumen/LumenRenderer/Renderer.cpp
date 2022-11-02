//
// Created by enzoc on 30/09/2022.
//

#include <tbb/tbb.h>
#include <numeric>
#include "Renderer.hpp"

namespace LumenRender {

    namespace Utils {
        uint32_t ConvertToRGBA(glm::vec4 color) {
            auto r = static_cast<uint32_t>(color.r * 255.0f);
            auto g = static_cast<uint32_t>(color.g * 255.0f);
            auto b = static_cast<uint32_t>(color.b * 255.0f);
            auto a = static_cast<uint32_t>(color.a * 255.0f);
            return a << 24 | b << 16 | g << 8 | r;
        }

        uint32_t ConvertToRGBA_SPP(glm::vec4 color, uint32_t spp) {
            auto scale = 1.0f / (float)spp;

            glm::vec4 scaledColor = color * scale;

            auto r = static_cast<uint32_t>(glm::clamp(scaledColor.r, 0.0f, 0.999f) * 256);
            auto g = static_cast<uint32_t>(glm::clamp(scaledColor.g, 0.0f, 0.999f) * 256);
            auto b = static_cast<uint32_t>(glm::clamp(scaledColor.b, 0.0f, 0.999f) * 256);
            auto a = static_cast<uint32_t>(glm::clamp(scaledColor.a, 0.0f, 0.999f) * 256);

            return a << 24 | b << 16 | g << 8 | r;
        }

    }

    void Renderer::Render(const LumenRender::Camera &camera, const LumenRender::Scene &scene) {
        m_ActiveCamera = &camera;
        m_ActiveScene = &scene;

        const uint32_t depth = 10;

#if 1

        tbb::parallel_for(tbb::blocked_range2d<uint32_t>(0, m_Image->GetHeight(), 0, m_Image->GetWidth()),
                          [&](const tbb::blocked_range2d<uint32_t> &range) {

                              for (uint32_t y = range.rows().begin(); y != range.rows().end(); y++) {
                                  for (uint32_t x = range.cols().begin(); x != range.cols().end(); x++) {
                                      glm::vec4 color = PerPixel(x, y);
                                      color = glm::clamp(color, 0.0f, 1.0f);
                                      m_ImageData[y * m_Image->GetWidth() + x] = Utils::ConvertToRGBA(color);
                                  }
                              }
                          });


#else

        for (uint32_t y = 0; y < m_Image->GetHeight(); y++) {
            for (uint32_t x = 0; x < m_Image->GetWidth(); x++) {
                m_ImageData[y * m_Image->GetWidth() + x] = Utils::ConvertToRGBA(glm::clamp(PerPixel(x, y), 0.0f, 1.0f));
            }
        }

#endif
        m_Image->SetData(m_ImageData);
    }


    void Renderer::Accumulate(const Camera &camera, const Scene &scene) {
        m_ActiveCamera = &camera;
        m_ActiveScene = &scene;

        const uint32_t spp = 1;

        tbb::parallel_for(tbb::blocked_range2d<uint32_t>(0, m_Image->GetHeight(), 0, m_Image->GetWidth()),
                          [&](const tbb::blocked_range2d<uint32_t> &range) {

                              for (uint32_t y = range.rows().begin(); y != range.rows().end(); y++) {
                                  for (uint32_t x = range.cols().begin(); x != range.cols().end(); x++) {
                                      glm::vec4 color{ 0, 0, 0, 0 };
                                      for (uint32_t s = 0; s < spp; s++) {

                                          color += glm::clamp(PerPixel(x, y), 0.0f, 1.0f);

                                      }
                                      m_ImageData[y * m_Image->GetWidth() + x] = Utils::ConvertToRGBA_SPP(color, spp);
                                  }
                              }
                          });


        m_Image->SetData(m_ImageData);
    }


    void Renderer::OnResize(uint32_t width, uint32_t height) {
        if (m_Image) {
            //no resize needed
            if (m_Image->GetWidth() == width && m_Image->GetHeight() == height) {
                return;
            }

            m_Image->Resize(width, height);
        } else {
            m_Image = std::make_shared<Lumen::Image>(width, height, Lumen::ImageFormat::RGBA);
        }

        delete[] m_ImageData;
        m_ImageData = new uint32_t[width * height];
    }

    HitRecords Renderer::TraceRay(LumenRender::Ray &ray) {
        HitRecords hitRecords{};

        float tmax = std::numeric_limits<float>::max();

        if (m_ActiveScene->Hit(ray, tmax)) {
            hitRecords = ray.m_Record;
        } else {
            hitRecords = Miss(ray);
        }

        return hitRecords;
    }

    glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y) {
        Ray ray;
        ray.Origin = m_ActiveCamera->GetPosition();
        ray.Direction = m_ActiveCamera->GetRayDirections()[y * m_Image->GetWidth() + x];

        glm::vec3 color(0.0f);

        TraceRay(ray);

        if (ray.m_Record.m_T < 0.0f) {
            auto t = 0.5f*(ray.Direction.y + 1.0f);
            color = (1.0f-t)*glm::vec3(1.0, 1.0, 1.0) + t*glm::vec3(0.5, 0.7, 1.0);
        }

        glm::vec3 lightDir = glm::normalize(glm::vec3(-1));
        float lightIntensity = glm::max(glm::dot(ray.m_Record.m_Normal, -lightDir), 0.0f); // == cos(angle)
        glm::vec3 sphereColor = { 1.0f, 0.2f, 0.3f };
        sphereColor *= lightIntensity;
        color += sphereColor;

        return { color, 1.0f };
    }


    HitRecords Renderer::Miss(Ray &ray) {
        HitRecords hitRecords{};
        ray.m_Record.m_T = -1.0f;
        hitRecords = ray.m_Record;
        return hitRecords;
    }

} // LumenRender