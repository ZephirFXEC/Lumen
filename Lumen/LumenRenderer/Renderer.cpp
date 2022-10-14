//
// Created by enzoc on 30/09/2022.
//

#include <tbb/tbb.h>
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
    }

    void Renderer::Render(const LumenRender::Camera &camera, const LumenRender::Scene &scene) {

        m_ActiveScene = &scene;
        m_ActiveCamera = &camera;


#if 1

        tbb::parallel_for(tbb::blocked_range2d<uint32_t>(0, m_Image->GetHeight(),0, m_Image->GetWidth()),
                          [&](const tbb::blocked_range2d<uint32_t>& range) {

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

    HitRecords Renderer::TraceRay(const LumenRender::Ray &ray) {
        HitRecords hitRecords{};

        if(m_ActiveScene->Hit(ray, hitRecords)) {
            return hitRecords;
        } else {
            return Miss(ray);
        }

    }

    glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y) {
        Ray ray;
        ray.Origin = m_ActiveCamera->GetPosition();
        ray.Direction = m_ActiveCamera->GetRayDirections()[y * m_Image->GetWidth() + x];

        glm::vec3 color(0.0f);
        float multiplier = 1.0f;

        int bounces = 1;
        for (int i = 0; i < bounces; i++)
        {
            HitRecords payload = TraceRay(ray);
            if (payload.m_T < 0.0f)
            {
                glm::vec3 skyColor = {0.0f, 0.0f, 0.0f};
                color += skyColor * multiplier;
                break;
            }

            glm::vec3 lightDir = glm::normalize(glm::vec3(-1));
            float lightIntensity = glm::max(glm::dot(payload.m_Normal, -lightDir), 0.0f); // == cos(angle)

            glm::vec3 sphereColor = {1.0f, 0.2f, 0.3f};
            sphereColor *= lightIntensity;
            color += sphereColor * multiplier;

            multiplier *= 0.7f;

            ray.Origin = payload.m_Position + payload.m_Normal * 0.0001f;
            ray.Direction = glm::reflect(ray.Direction, payload.m_Normal);

        }

        return {color, 1.0f};
    }


    HitRecords Renderer::Miss(const Ray &ray) {
        HitRecords payload{};
        payload.m_T = -1.0f;
        return payload;
    }
} // LumenRender