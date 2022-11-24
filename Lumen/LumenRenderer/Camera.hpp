//
// Created by enzoc on 30/09/2022.
//

#ifndef LUMEN_CAMERA_HPP
#define LUMEN_CAMERA_HPP

#include <glm/glm.hpp>
#include <vector>

namespace LumenRender {
class Camera
{
  public:
    Camera(float verticalFOV, float nearClip, float farClip);

    auto OnUpdate(float ts) -> bool;

    void OnResize(uint32_t width, uint32_t height);

    [[nodiscard]] __forceinline auto GetProjection() const -> const glm::mat4 & { return m_Projection; }

    [[nodiscard]] __forceinline auto GetInverseProjection() const -> const glm::mat4 & { return m_InverseProjection; }

    [[nodiscard]] __forceinline auto GetView() const -> const glm::mat4 & { return m_View; }

    [[nodiscard]] __forceinline auto GetInverseView() const -> const glm::mat4 & { return m_InverseView; }

    [[nodiscard]] __forceinline auto GetPosition() const -> const glm::vec3 & { return m_Position; }

    [[nodiscard]] __forceinline auto GetDirection() const -> const glm::vec3 & { return m_ForwardDirection; }

    [[nodiscard]] __forceinline auto GetRayDirections() const -> const glm::vec3 * { return m_RayDirections; }

    static auto GetRotationSpeed() -> float;

  private:
    void RecalculateProjection();

    void RecalculateView();

    void RecalculateRayDirections();


    glm::mat4 m_Projection{ 1.0F };
    glm::mat4 m_View{ 1.0F };
    glm::mat4 m_InverseProjection{ 1.0F };
    glm::mat4 m_InverseView{ 1.0F };

    float m_VerticalFOV{ 45.0F };
    float m_NearClip{ 0.1F };
    float m_FarClip{ 100.0F };

    glm::vec3 m_Position{ 0.0F, 0.0F, 0.0F };
    glm::vec3 m_ForwardDirection{ 0.0F, 0.0F, 0.0F };

    // Cached ray directions
    glm::vec3 *m_RayDirections{ nullptr };

    glm::vec2 m_LastMousePosition{ 0.0F, 0.0F };

    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
};

}// namespace LumenRender

#endif// LUMEN_CAMERA_HPP