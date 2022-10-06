//
// Created by enzoc on 30/09/2022.
//

#include <tbb/tbb.h>
#include "Renderer.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/norm.hpp>

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

    void Renderer::Render(const LumenRender::Camera &camera, const std::unordered_map<uint32_t, Object *> &objects) {

        m_ActiveCamera = &camera;
        m_Objects = &objects;


#if 0

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

                glm::vec4 color = PerPixel(x, y);
                color = glm::clamp(color, 0.0f, 1.0f);
                m_ImageData[y * m_Image->GetWidth() + x] = Utils::ConvertToRGBA(color);

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
        HitRecords records{};
        for (auto& [id, object] : *m_Objects) {
            HitRecords temp{};
            if (object->Intersect(ray, temp)) {
                records.m_Index = id;
                records = temp;
            }
        }

        return records;
    }


    glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y) {

        Ray ray;
        ray.Origin = m_ActiveCamera->GetPosition();
        ray.Direction = m_ActiveCamera->GetRayDirections()[y * m_Image->GetWidth() + x];

        HitRecords payload = TraceRay(ray);

        glm::vec3 col = glm::vec3(0.0f);

        // color red if hit
        if (payload.m_T < 1000.0f && payload.m_T > 0.0f) {
            col = {1.0f, 0.0f, 0.0f};
        }

        return {col, 1.0f};
    }

    HitRecords Renderer::ClosestHit(const Ray &ray, float dist, uint32_t ObjectID) {

        return {};
    }


    HitRecords Renderer::Miss(const Ray &ray) {
        HitRecords payload{};
        payload.m_T = -1.0f;
        return payload;
    }
} // LumenRender