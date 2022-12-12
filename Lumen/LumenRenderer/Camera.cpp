//
// Created by enzoc on 30/09/2022.
//

#include "Camera.hpp"
#include "../../LumenCore/Inputs/Input.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <tbb/tbb.h>

namespace LumenRender {

Camera::Camera(float verticalFOV, float nearClip, float farClip)
  : m_VerticalFOV(verticalFOV), m_NearClip(nearClip), m_FarClip(farClip), m_Position({ 0, 0, 5 }),
    m_ForwardDirection({ 0, 0, -1 })
{}

auto Camera::OnUpdate(float ts) -> std::optional<bool>
{
    const auto mousePos = Lumen::Input::GetMousePosition();
    const auto delta = (mousePos - m_LastMousePosition) * 0.002F;
    m_LastMousePosition = mousePos;

    if (!Lumen::Input::IsMouseButtonDown(Lumen::MouseButton::Right)) {
        Lumen::Input::SetCursorMode(Lumen::CursorMode::Normal);
        return {};
    }

    Lumen::Input::SetCursorMode(Lumen::CursorMode::Locked);

    bool moved = false;

    constexpr glm::vec3 upDirection(0.0F, 1.0F, 0.0F);
    const auto rightDirection = glm::cross(m_ForwardDirection, upDirection);

    constexpr auto speed = 5.0F;

    // Movement
    if (Lumen::Input::IsKeyDown(Lumen::KeyCode::W)) {
        m_Position += m_ForwardDirection * speed * ts;
        moved = true;
    } else if (Lumen::Input::IsKeyDown(Lumen::KeyCode::S)) {
        m_Position -= m_ForwardDirection * speed * ts;
        moved = true;
    }
    if (Lumen::Input::IsKeyDown(Lumen::KeyCode::A)) {
        m_Position -= rightDirection * speed * ts;
        moved = true;
    } else if (Lumen::Input::IsKeyDown(Lumen::KeyCode::D)) {
        m_Position += rightDirection * speed * ts;
        moved = true;
    }
    if (Lumen::Input::IsKeyDown(Lumen::KeyCode::Q)) {
        m_Position -= upDirection * speed * ts;
        moved = true;
    } else if (Lumen::Input::IsKeyDown(Lumen::KeyCode::E)) {
        m_Position += upDirection * speed * ts;
        moved = true;
    }

    // Rotation
    if (delta.x != 0.0F || delta.y != 0.0F) {
        float const pitchDelta = delta.y * GetRotationSpeed();
        float const yawDelta = delta.x * GetRotationSpeed();

        glm::quat const q = glm::normalize(glm::cross(
          glm::angleAxis(-pitchDelta, rightDirection), glm::angleAxis(-yawDelta, glm::vec3(0.F, 1.0F, 0.0F))));
        m_ForwardDirection = glm::rotate(q, m_ForwardDirection);

        moved = true;
    }

    if (moved) {
        RecalculateView();
        RecalculateRayDirections();
    }

    return moved;
}

void Camera::OnResize(uint32_t width, uint32_t height)
{
    if (width == m_ViewportWidth && height == m_ViewportHeight) { return; }

    m_ViewportWidth = width;
    m_ViewportHeight = height;

    RecalculateProjection();
    RecalculateRayDirections();
}

void Camera::RecalculateProjection()
{
    m_Projection = glm::perspectiveFov(glm::radians(m_VerticalFOV),
      static_cast<float>(m_ViewportWidth),
      static_cast<float>(m_ViewportHeight),
      m_NearClip,
      m_FarClip);
    m_InverseProjection = glm::inverse(m_Projection);
}

void Camera::RecalculateView()
{
    m_View = glm::lookAt(m_Position, m_Position + m_ForwardDirection, { 0, 1, 0 });
    m_InverseView = glm::inverse(m_View);
}

void Camera::RecalculateRayDirections()
{
    m_RayDirections.clear();
    m_RayDirections.resize(m_ViewportWidth * m_ViewportHeight);

    // Store results of calculations in local variables to avoid repeating them in the inner loop
    auto const width = static_cast<float>(m_ViewportWidth);
    auto const height = static_cast<float>(m_ViewportHeight);


    tbb::parallel_for(tbb::blocked_range2d<uint32_t>(0, m_ViewportHeight, 0, m_ViewportWidth),
      [&](const tbb::blocked_range2d<uint32_t> &range) {
          for (uint32_t y = range.rows().begin(); y != range.rows().end(); ++y) {
              for (uint32_t x = range.cols().begin(); x != range.cols().end(); ++x) {

                  glm::vec2 coord = { static_cast<float>(x) / width, static_cast<float>(y) / height };

                  coord = coord * 2.0F - 1.0F;// -1 -> 1

                  glm::vec4 const target = m_InverseProjection * glm::vec4(coord.x, coord.y, 1, 1);

                  glm::vec3 const rayDirection =
                    glm::vec3(m_InverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0));// World space

                  m_RayDirections[y * m_ViewportWidth + x] = rayDirection;
              }
          }
      });
}
}// namespace LumenRender