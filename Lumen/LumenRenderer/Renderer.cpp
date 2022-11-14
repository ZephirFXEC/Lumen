//
// Created by enzoc on 30/09/2022.
//

#include <tbb/tbb.h>
#include <numeric>
#include "Renderer.hpp"

namespace LumenRender {

    namespace Utils {

        auto BackgroundColor(const Ray &ray) -> glm::vec3 {
            auto t = 0.5F * (ray.Direction.y + 1.0F);
            return (1.0F - t) * glm::vec3(1.0, 1.0, 1.0) + t * glm::vec3(0.5F, 0.7F, 1.0);
        }

        auto ConvertToRGBA(glm::vec4 color) -> uint32_t {
            auto r = static_cast<uint8_t>(color.r * 255);
            auto g = static_cast<uint8_t>(color.g * 255);
            auto b = static_cast<uint8_t>(color.b * 255);
            auto a = static_cast<uint8_t>(color.a * 255);
            return a << 24 | b << 16 | g << 8 | r;
        }

        auto ConvertToRGBA_SPP(glm::vec4 color, uint32_t spp) -> uint32_t {
            auto scale = 1.0F / static_cast<float>(spp);

            glm::vec4 scaledColor = color * scale;

            auto r = static_cast<uint8_t>(glm::clamp(scaledColor.r, 0.0F, 0.999F) * 256);
            auto g = static_cast<uint8_t>(glm::clamp(scaledColor.g, 0.0F, 0.999F) * 256);
            auto b = static_cast<uint8_t>(glm::clamp(scaledColor.b, 0.0F, 0.999F) * 256);
            auto a = static_cast<uint8_t>(glm::clamp(scaledColor.a, 0.0F, 0.999F) * 256);

            return a << 24 | b << 16 | g << 8 | r;
        }

    }


    void Renderer::Render(const LumenRender::Camera &camera, const LumenRender::Scene &scene) {
        m_ActiveCamera = &camera;
        m_ActiveScene = &scene;

        if (m_FrameSample == 1) {
            memset(m_AccumulationBuffer, 0, static_cast<uint64_t>(m_Image->GetWidth() * m_Image->GetHeight())
            * sizeof(glm::vec4));
        }

        tbb::parallel_for(tbb::blocked_range2d<uint32_t>(0, m_Image->GetHeight(), 0, m_Image->GetWidth()),
                          [&](const tbb::blocked_range2d<uint32_t> &range) {
                              for (uint32_t y = range.rows().begin(); y != range.rows().end(); y++) {
                                  for (uint32_t x = range.cols().begin(); x != range.cols().end(); x++) {

                                      glm::vec4 color = PerPixel(x, y);
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


        m_Image->SetData(m_ImageData);


        if (m_Settings.Accumulate) {
            m_FrameSample++;
        } else {
            m_FrameSample = 1;
        }
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
        delete[] m_AccumulationBuffer;

        m_ImageData = new uint32_t[static_cast<uint64_t>(width * height)];
        m_AccumulationBuffer = new glm::vec4[static_cast<uint64_t>(width * height)];
    }

    auto Renderer::TraceRay(LumenRender::Ray &ray) -> HitRecords {

        float tmax = std::numeric_limits<float>::max();

        if (!m_ActiveScene->Hit(ray, tmax)) {
            return Miss(ray);
        }

        return ray.m_Record;
    }

    auto Renderer::PerPixel(uint32_t x, uint32_t y) -> glm::vec4 {
        Ray ray;
        ray.Origin = m_ActiveCamera->GetPosition();
        ray.Direction = m_ActiveCamera->GetRayDirections()[y * m_Image->GetWidth() + x];

        glm::vec3 color(0.0F);

        TraceRay(ray);

        // if we miss the scene, return the background color
        if (ray.m_Record.m_T < 0.0F) {
            color = Utils::BackgroundColor(ray);
        }

        glm::vec3 lightDir = glm::normalize(glm::vec3(-1));
        float lightIntensity = glm::max(glm::dot(ray.m_Record.m_Normal, -lightDir), 0.0F); // == cos(angle)
        glm::vec3 sphereColor = { 1.0F, 0.2F, 0.3F };
        sphereColor *= lightIntensity;
        color += sphereColor;

        return { color, 1.0F };
    }


    auto Renderer::Miss(Ray &ray) -> HitRecords {
        ray.m_Record.m_T = -1.0F;
        return ray.m_Record;
    }


} // LumenRender