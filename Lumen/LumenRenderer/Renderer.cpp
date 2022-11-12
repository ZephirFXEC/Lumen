//
// Created by enzoc on 30/09/2022.
//

#include <tbb/tbb.h>
#include <numeric>
#include "Renderer.hpp"

namespace LumenRender {

    namespace Utils {

        glm::vec3 BackgroundColor(const Ray &ray) {
            auto t = 0.5f * (ray.Direction.y + 1.0f);
            return (1.0f - t) * glm::vec3(1.0, 1.0, 1.0) + t * glm::vec3(0.5, 0.7, 1.0);
        }

        uint32_t ConvertToRGBA(glm::vec4 color) {
            auto r = static_cast<uint8_t>(color.r * 255);
            auto g = static_cast<uint8_t>(color.g * 255);
            auto b = static_cast<uint8_t>(color.b * 255);
            auto a = static_cast<uint8_t>(color.a * 255);
            return a << 24 | b << 16 | g << 8 | r;
        }

        uint32_t ConvertToRGBA_SPP(glm::vec4 color, uint32_t spp) {
            auto scale = 1.0f / (float) spp;

            glm::vec4 scaledColor = color * scale;

            auto r = static_cast<uint8_t>(glm::clamp(scaledColor.r, 0.0f, 0.999f) * 256);
            auto g = static_cast<uint8_t>(glm::clamp(scaledColor.g, 0.0f, 0.999f) * 256);
            auto b = static_cast<uint8_t>(glm::clamp(scaledColor.b, 0.0f, 0.999f) * 256);
            auto a = static_cast<uint8_t>(glm::clamp(scaledColor.a, 0.0f, 0.999f) * 256);

            return a << 24 | b << 16 | g << 8 | r;
        }

    }

    //TODO: Accumulator to let the image converge

    void Renderer::Render(const LumenRender::Camera &camera, const LumenRender::Scene &scene) {
        m_ActiveCamera = &camera;
        m_ActiveScene = &scene;

        if (m_FrameSample == 1)
            memset(m_AccumulationBuffer, 0, m_Image->GetWidth() * m_Image->GetHeight() * sizeof(glm::vec4));

        tbb::parallel_for(tbb::blocked_range2d<uint32_t>(0, m_Image->GetHeight(), 0, m_Image->GetWidth()),
                          [&](const tbb::blocked_range2d<uint32_t> &range) {
                              for (uint32_t y = range.rows().begin(); y != range.rows().end(); y++) {
                                  for (uint32_t x = range.cols().begin(); x != range.cols().end(); x++) {

                                      glm::vec4 color = PerPixel(x, y);
                                      m_AccumulationBuffer[y * m_Image->GetWidth() + x] += color;

                                      glm::vec4 accumulatedColor = m_AccumulationBuffer[y * m_Image->GetWidth() +
                                                                                        x];
                                      accumulatedColor /= (float) m_FrameSample;

                                      accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f),
                                                                    glm::vec4(1.0f));
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

        m_ImageData = new uint32_t[width * height];
        m_AccumulationBuffer = new glm::vec4[width * height];
    }

    HitRecords Renderer::TraceRay(LumenRender::Ray &ray) {

        float tmax = std::numeric_limits<float>::max();

        if (!m_ActiveScene->Hit(ray, tmax)) {
            return Miss(ray);
        }

        return ray.m_Record;
    }

    glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y) {
        Ray ray;
        ray.Origin = m_ActiveCamera->GetPosition();
        ray.Direction = m_ActiveCamera->GetRayDirections()[y * m_Image->GetWidth() + x];

        glm::vec3 color(0.0f);

        TraceRay(ray);

        // if we miss the scene, return the background color
        if (ray.m_Record.m_T < 0.0f) {
            color = Utils::BackgroundColor(ray);
        }

        glm::vec3 lightDir = glm::normalize(glm::vec3(-1));
        float lightIntensity = glm::max(glm::dot(ray.m_Record.m_Normal, -lightDir), 0.0f); // == cos(angle)
        glm::vec3 sphereColor = { 1.0f, 0.2f, 0.3f };
        sphereColor *= lightIntensity;
        color += sphereColor;

        return { color, 1.0f };
    }


    HitRecords Renderer::Miss(Ray &ray) {
        ray.m_Record.m_T = -1.0f;
        return ray.m_Record;
    }


} // LumenRender