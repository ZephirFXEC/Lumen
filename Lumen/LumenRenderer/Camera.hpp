// Copyright (c) 2022.
// Enzo Crema
// All rights reserved


#ifndef LUMEN_CAMERA_HPP
#define LUMEN_CAMERA_HPP

#include <glm/glm.hpp>
#include <optional>
#include <vector>

namespace LumenRender {
struct Camera
{
    Camera(float verticalFOV, float nearClip, float farClip);

    auto OnUpdate(float ts) -> std::optional<bool>;

    void OnResize(uint32_t width, uint32_t height);

    void RecalculateProjection();

    void RecalculateView();

    void RecalculateRayDirections();


    glm::mat4 mProjection = glm::mat4(1.0F);
    glm::mat4 mView = glm::mat4(1.0F);
    glm::mat4 mInverseProjection = glm::mat4(1.0F);
    glm::mat4 mInverseView = glm::mat4(1.0F);

    float mVerticalFOV = 45.0F;
    float mNearClip = 0.1F;
    float mFarClip = 100.0F;
    float mRotationSpeed = 0.3F;

    glm::vec3 mPosition = {};
    glm::vec3 mForwardDirection = {};

    // Cached ray directions
    std::vector<glm::vec3> mRayDirections = {};

    glm::vec2 mLastMousePosition = {};

    uint32_t mViewportWidth = 0, mViewportHeight = 0;
};

}// namespace LumenRender

#endif// LUMEN_CAMERA_HPP