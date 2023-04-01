//
// Created by enzoc on 30/09/2022.
//

#include <tbb/tbb.h>
#include "Renderer.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

namespace LumenRender {

    namespace Utils {
        static uint32_t ConvertToRGBA(glm::vec4 color) {
            return uint32_t(color.a * 255.99f) << 24 | uint32_t(color.b * 255.99f) << 16 |
                   uint32_t(color.g * 255.99f) << 8 | uint32_t(color.r * 255.99f);
        }
    }

    void Renderer::Render(const LumenRender::Camera &camera, const LumenRender::Object &objects) {

        m_ActiveCamera = &camera;
        m_Objects = &objects;


#if 0

        tbb::parallel_for(tbb::blocked_range2d<uint32_t>(0, m_Image->GetHeight(),0, m_Image->GetWidth()),
                          [&](const tbb::blocked_range2d<uint32_t>& range) {

                              for (uint32_t y = range.rows().begin(); y != range.rows().end(); y++) {
                                  for (uint32_t x = range.cols().begin(); x != range.cols().end(); x++) {
                                      m_ImageData[y * m_Image->GetWidth() + x] = Utils::ConvertToRGBA(PerPixel(x, y));
                                  }
                              }
                          });


#else

        for (uint32_t y = 0; y < m_Image->GetHeight(); y++) {
            for (uint32_t x = 0; x < m_Image->GetWidth(); x++) {
                m_ImageData[y * m_Image->GetWidth() + x] = Utils::ConvertToRGBA(PerPixel(x, y));
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

        return records;
    }



    glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y) {

        Ray ray;
        ray.Origin = m_ActiveCamera->GetPosition();
        ray.Direction = m_ActiveCamera->GetRayDirections()[y * m_Image->GetWidth() + x];

        HitRecords payload = TraceRay(ray);

        glm::vec3 col;

        if(payload.m_T < 0.0f) {
            col = glm::vec3(0.0f);
        } else {
            col = glm::vec3(1.0f, 0.0f, 0.0f);
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