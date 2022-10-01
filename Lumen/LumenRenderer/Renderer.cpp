//
// Created by enzoc on 30/09/2022.
//

#include "Renderer.hpp"

namespace LumenRender {

    namespace Utils {
        uint32_t ConvertToRGBA(glm::vec4 color) {
            auto r = (uint32_t) (color.r * 255.0f);
            auto g = (uint32_t) (color.g * 255.0f);
            auto b = (uint32_t) (color.b * 255.0f);
            auto a = (uint32_t) (color.a * 255.0f);
            return a << 24 | b << 16 | g << 8 | r;
        }
    }

    void Renderer::Render(const LumenRender::Camera& camera) {

        RayDesc ray;
        ray.Origin = camera.GetPosition();

        for(int y = 0; y < m_Image->GetHeight(); y++) {
            for(int x = 0; x < m_Image->GetWidth(); x++) {

                ray.Direction = camera.GetRayDirections()[y * m_Image->GetWidth() + x];
                glm::vec4 color = glm::clamp(TraceRay(ray), 0.0f, 1.0f);
                m_ImageData[y * m_Image->GetWidth() + x] = Utils::ConvertToRGBA(color);

            }
        }
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

    glm::vec4 Renderer::TraceRay(const LumenRender::RayDesc& ray) {

            float radius = 0.5f;

            float a = glm::dot(ray.Direction, ray.Direction);
            float b = 2.0f * glm::dot(ray.Origin, ray.Direction);
            float c = glm::dot(ray.Origin, ray.Origin) - radius * radius;

            float discriminant = b * b - 4 * a * c;
            if(discriminant <= 0.0f) {
                return { 0.0f, 0.0f, 0.0f, 1.0f };
            }

            float t1 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
            float t2 = (-b - glm::sqrt(discriminant)) / (2.0f * a);

            glm::vec3 HitPoint = ray.Origin + ray.Direction * glm::min(t1, t2);
            glm::vec3 Normal = glm::normalize(HitPoint);

            glm::vec3 LightDir = glm::normalize(glm::vec3(-1.0f));
            float Diffuse = glm::max(glm::dot(Normal, -LightDir), 0.0f);

            glm::vec3 Color = { 1.0f, 0.0f, 1.0f };
            return {Color * Diffuse, 1.0f};
    }
} // LumenRender