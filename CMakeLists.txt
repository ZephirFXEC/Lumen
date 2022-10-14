cmake_minimum_required(VERSION 3.23)
project(Lumen)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
    set(CMAKE_TOOLCHAIN_FILE "F:/repos/vcpkg/scripts/buildsystems/vcpkg.cmake")
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
            Lumen/LumenRenderer/Renderer.hpp
            Lumen/LumenRenderer/Ray.hpp
            Lumen/LumenRenderer/Scene/Scene.cpp
            Lumen/LumenRenderer/Scene/Scene.hpp
            Lumen/LumenRenderer/Scene/Object.cpp
            Lumen/LumenRenderer/Scene/Object.hpp
            Lumen/LumenRenderer/Structure/Triangle_Mesh.cpp
            Lumen/LumenRenderer/Structure/Triangle_Mesh.hpp Lumen/LumenRenderer/Structure/Triangle.cpp Lumen/LumenRenderer/Structure/Triangle.hpp)

    target_link_libraries(Lumen PRIVATE glfw Vulkan::Vulkan imgui::imgui glm::glm TBB::tbb)
endif()