//
// Created by enzoc on 30/09/2022.
//

#include <tbb/tbb.h>
#include "Renderer.hpp"
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

    void Renderer::Render(const LumenRender::Camera &camera) {

        m_ActiveCamera = &camera;


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

        glm::vec3 oc = ray.Origin;
        auto half_b = glm::dot(oc, ray.Direction);
        auto c = glm::length2(oc) - 1;

        auto discriminant = half_b*half_b - c;
        if (discriminant < 0) return {};
        auto sqrtd = glm::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-half_b - sqrtd);
        if (root < 0.001 || std::numeric_limits<float>::max() < root) {
            root = (-half_b + sqrtd);
            if (root < 0.001 || std::numeric_limits<float>::max() < root)
                return {};
        }

        HitRecords rec{};
        rec.m_T = root;
        rec.m_Position = ray.At(rec.m_T);
        rec.m_Normal = glm::normalize(rec.m_Position);

        return rec;

    }


    glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y) {

        Ray ray;
        ray.Origin = m_ActiveCamera->GetPosition();
        ray.Direction = m_ActiveCamera->GetRayDirections()[y * m_Image->GetWidth() + x];

        HitRecords payload = TraceRay(ray);

        glm::vec3 lightDir = glm::normalize(glm::vec3(-1));

        float lightIntensity = glm::max(glm::dot(payload.m_Normal, -lightDir), 0.0f); // == cos(angle)

        glm::vec3 sphereColor = { 1.0f, 0.2f, 0.3f };

        sphereColor *= lightIntensity;

        return { sphereColor, 1.0f };
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