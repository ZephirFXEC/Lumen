// Copyright (c) 2022.
// Enzo Crema
// All rights reserved


#include "Camera.hpp"
#include "../../LumenCore/Inputs/Input.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <tbb/tbb.h>

namespace LumenRender {

Camera::Camera(float verticalFOV, float nearClip, float farClip)
  : mVerticalFOV(verticalFOV), mNearClip(nearClip), mFarClip(farClip), mPosition({ 0, 0, 5 }),
    mForwardDirection({ 0, 0, -1 })
{}

auto Camera::OnUpdate(float ts) -> std::optional<bool>
{
    const glm::vec2 mousePos = Lumen::Input::GetMousePosition();
    const glm::vec2 delta = (mousePos - mLastMousePosition) * 0.002F;
    mLastMousePosition = mousePos;

    if (!Lumen::Input::IsMouseButtonDown(Lumen::MouseButton::Right)) {
        Lumen::Input::SetCursorMode(Lumen::CursorMode::Normal);
        return {};
    }

    Lumen::Input::SetCursorMode(Lumen::CursorMode::Locked);

    bool moved = false;

    constexpr glm::vec3 upDirection(0.0F, 1.0F, 0.0F);
    const glm::vec3 rightDirection = glm::cross(mForwardDirection, upDirection);

    constexpr float speed = 5.0F;

    // Movement
    if (Lumen::Input::IsKeyDown(Lumen::KeyCode::W)) {
        mPosition += mForwardDirection * speed * ts;
        moved = true;
    } else if (Lumen::Input::IsKeyDown(Lumen::KeyCode::S)) {
        mPosition -= mForwardDirection * speed * ts;
        moved = true;
    }
    if (Lumen::Input::IsKeyDown(Lumen::KeyCode::A)) {
        mPosition -= rightDirection * speed * ts;
        moved = true;
    } else if (Lumen::Input::IsKeyDown(Lumen::KeyCode::D)) {
        mPosition += rightDirection * speed * ts;
        moved = true;
    }
    if (Lumen::Input::IsKeyDown(Lumen::KeyCode::Q)) {
        mPosition -= upDirection * speed * ts;
        moved = true;
    } else if (Lumen::Input::IsKeyDown(Lumen::KeyCode::E)) {
        mPosition += upDirection * speed * ts;
        moved = true;
    }

    // Rotation
    if (delta.x != 0.0F || delta.y != 0.0F) {
        const float pitchDelta = delta.y * mRotationSpeed;
        const float yawDelta = delta.x * mRotationSpeed;

        const glm::quat q = glm::normalize(glm::cross(
          glm::angleAxis(-pitchDelta, rightDirection), glm::angleAxis(-yawDelta, glm::vec3(0.F, 1.0F, 0.0F))));
        mForwardDirection = glm::rotate(q, mForwardDirection);

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
    if (width == mViewportWidth && height == mViewportHeight) { return; }

    mViewportWidth = width;
    mViewportHeight = height;

    mRayDirections.resize(mViewportWidth * mViewportHeight);

    RecalculateProjection();
    RecalculateRayDirections();
}

void Camera::RecalculateProjection()
{
    mProjection = glm::perspectiveFov(glm::radians(mVerticalFOV),
      static_cast<float>(mViewportWidth),
      static_cast<float>(mViewportHeight),
      mNearClip,
      mFarClip);
    mInverseProjection = glm::inverse(mProjection);
}

void Camera::RecalculateView()
{
    mView = glm::lookAt(mPosition, mPosition + mForwardDirection, { 0, 1, 0 });
    mInverseView = glm::inverse(mView);
}

void Camera::RecalculateRayDirections()
{
    mRayDirections.clear();

    // Store results of calculations in local variables to avoid repeating them in the inner loop
    const float width = static_cast<float>(mViewportWidth);
    const float height = static_cast<float>(mViewportHeight);


    for (uint32_t y = 0; y < mViewportHeight; ++y) {
        for (uint32_t x = 0; x < mViewportWidth; ++x) {

            const glm::vec2 coord =
              glm::vec2(static_cast<float>(x) / width, static_cast<float>(y) / height) * 2.0f - 1.0f;

            const glm::vec4 target = mInverseProjection * glm::vec4(coord.x, coord.y, 1, 1);

            const glm::vec3 rayDirection =
              glm::vec3(mInverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0));// World space

            mRayDirections[y * mViewportWidth + x] = rayDirection;
        }
    }
}
}// namespace LumenRender