//
// Created by enzoc on 30/09/2022.
//


#include "Renderer.hpp"
#include <immintrin.h>
#include <tbb/tbb.h>
#include <bitset>
#include <embree4/rtcore.h>
#include "Structure/Triangle.hpp"

RTCScene g_scene = nullptr;
LumenRender::Triangle data;


namespace LumenRender {

    namespace Utils {
        static uint32_t ConvertToRGBA(const glm::vec4 *color) {

            uint8_t out[4];
#if 1

            const __m128 x = _mm_mul_ps(_mm_load_ps((float *) color), _mm_set_ps1(255));
            __m128i y = _mm_cvtps_epi32(x);
            y = _mm_packus_epi32(y, y);
            y = _mm_packus_epi16(y, y);
            *(int *) out = _mm_cvtsi128_si32(y);

            const std::bitset<32> bits = (*reinterpret_cast<uint32_t *>(out));

            return bits.to_ulong();

#else

            uint8_t r = color.r * 255U;
            uint8_t g = color.g * 255U;
            uint8_t b = color.b * 255U;
            uint8_t a = color.a * 255U;
            return (uint32_t) (a << 24U | b << 16U | g << 8U | r);

#endif
        }
    }

    void Renderer::Render(const LumenRender::Camera &camera, const LumenRender::Mesh &objects) {

        m_ActiveCamera = &camera;
        m_Objects = &objects;


#ifdef MT
        tbb::parallel_for(tbb::blocked_range<uint32_t>(0, m_Image->GetHeight()),
                [&](const tbb::blocked_range<uint32_t>& r) {

            for (uint32_t y = r.begin(); y < r.end(); y++) {
                for (uint32_t x = 0; x < m_Image->GetWidth(); x++) {
                const glm::vec4 cd = PerPixel(x, y);
                m_ImageData[y * m_Image->GetWidth() + x] = Utils::ConvertToRGBA(&cd);
                }
            }

        });
#else

        for (uint32_t y = 0; y < m_Image->GetHeight(); y++) {
            for (uint32_t x = 0; x < m_Image->GetWidth(); x++) {
                const glm::vec4 cd = PerPixel(x, y);
                m_ImageData[y * m_Image->GetWidth() + x] = Utils::ConvertToRGBA(&cd);
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


    glm::vec4 Renderer::PerPixel(int x, int y) {
        /*
        Ray ray;
        ray.Origin = m_ActiveCamera->GetPosition();
        ray.Direction = m_ActiveCamera->GetRayDirections()[y * m_Image->GetWidth() + x];



        HitRecords payload = TraceRay(ray);

        glm::vec3 col;

        if(payload.m_T < 0.0f) {
            col = glm::vec3(0.0f);
        } else {
            col = ray.Direction;
        }

         */

        return {m_ActiveCamera->GetRayDirections()[y * m_Image->GetWidth() + x], 1.0f};
    }

    HitRecords Renderer::ClosestHit(const Ray &ray, const float &dist, uint32_t ObjectID) {
        HitRecords payload{};
        payload.m_T = dist;
        return payload;
    }


    HitRecords Renderer::Miss(const Ray &ray) {
        HitRecords payload{};
        payload.m_T = -1.0f;
        return payload;
    }


} // LumenRender