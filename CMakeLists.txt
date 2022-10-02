cmake_minimum_required(VERSION 3.23)
project(Lumen)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(tbb CONFIG REQUIRED)

add_executable(Lumen Lumen/LumenApp.cpp
        LumenCore/Application.cpp
        LumenCore/Application.hpp
        LumenCore/EntryPoint.hpp
        LumenCore/Image.cpp
        LumenCore/Image.hpp
        LumenCore/Layer.hpp
        LumenCore/Random.cpp
        LumenCore/Random.hpp
        LumenCore/Timer.hpp
        LumenCore/Inputs/Input.cpp
        LumenCore/Inputs/Input.hpp
        LumenCore/Inputs/KeyCodes.hpp
        Lumen/LumenRenderer/Camera.cpp
        Lumen/LumenRenderer/Camera.hpp
        Lumen/LumenRenderer/Renderer.cpp
        Lumen/LumenRenderer/Renderer.hpp Lumen/LumenRenderer/Ray.hpp)

target_link_libraries(Lumen PRIVATE glfw Vulkan::Vulkan imgui::imgui glm::glm TBB::tbb)