//
// Created by enzoc on 30/09/2022.
//

#include <tbb/tbb.h>
#include "Renderer.hpp"
#include "Scene/Object.hpp"
#include <glm/gtx/intersect.hpp>

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
        //check if ray hit any object and return the closest hit
        HitRecords hitRecords{};
        /*
        hitRecords.m_T = std::numeric_limits<float>::max();
        for (const auto& object : m_ActiveScene->GetObjects()) {
            HitRecords temp{};
            if (obj->Hit(ray, temp) && temp.m_T < hitRecords.m_T) {
                    hitRecords = temp;
                    hitRecords.m_Index = i;
                } else {
                    return Miss(ray);
                }
            }
            */
        return ClosestHit(ray, hitRecords.m_T, hitRecords.m_Index);
    }

    glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y) {
        Ray ray;
        ray.Origin = m_ActiveCamera->GetPosition();
        ray.Direction = m_ActiveCamera->GetRayDirections()[y * m_Image->GetWidth() + x];

        HitRecords hitRecords{};
        if(glm::intersectRaySphere(ray.Origin, glm::normalize(ray.Direction), glm::vec3(0.0f), 1.0f, hitRecords.m_T)) {
            hitRecords.m_Position = ray.At(hitRecords.m_T);
            hitRecords.m_Normal = glm::normalize(hitRecords.m_Position);
        }

        glm::vec3 color(0.0f);
        float multiplier = 1.0f;


        glm::vec3 lightDir = glm::normalize(glm::vec3(-1));
        float lightIntensity = glm::max(glm::dot(hitRecords.m_Normal, -lightDir), 0.0f); // == cos(angle)

        glm::vec3 sphereColor = {1.0f, 0.2f, 0.3f};
        sphereColor *= lightIntensity;
        color += sphereColor * multiplier;

        multiplier *= 0.7f;

        ray.Origin = hitRecords.m_Position + hitRecords.m_Normal * 0.0001f;
        ray.Direction = glm::reflect(ray.Direction, hitRecords.m_Normal);

        /*
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

        } */



        return {color, 1.0f};
    }

    HitRecords Renderer::ClosestHit(const Ray &ray, float dist, uint32_t ObjectID) {
        HitRecords payload{};
        const auto& closestSphere = m_ActiveScene->GetObjects().at(ObjectID);

        if(closestSphere->GetType() == Object::Types::Sphere) {

            auto *sphere = dynamic_cast<Sphere*>(closestSphere);

            payload.m_T = dist;
            payload.m_Position = ray.Origin + ray.Direction * dist;
            payload.m_Normal = glm::normalize(payload.m_Position - sphere->m_Center);
            payload.m_Index = ObjectID;
        }

        return payload;
    }


    HitRecords Renderer::Miss(const Ray &ray) {
        HitRecords payload{};
        payload.m_T = -1.0f;
        return payload;
    }
} // LumenRender