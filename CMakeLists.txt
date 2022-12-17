cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


project(Lumen CXX)

if (WIN32)

    set(CMAKE_TOOLCHAIN_FILE "F:/repos/vcpkg/scripts/buildsystems/vcpkg.cmake")

    file(GLOB_RECURSE SOURCES "Lumen/*.cpp" "LumenCore/*.cpp")
    file(GLOB_RECURSE HEADERS "Lumen/*.hpp" "LumenCore/*.hpp" "Lumen/Externals/*.h")

    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

    find_package(glfw3 CONFIG REQUIRED)
    find_package(Vulkan REQUIRED)
    find_package(imgui CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
    find_package(tbb CONFIG REQUIRED)

    target_link_libraries(${PROJECT_NAME} PRIVATE glfw Vulkan::Vulkan imgui::imgui glm::glm TBB::tbb TBB::tbbmalloc TBB::tbbmalloc_proxy)

    set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

endif()