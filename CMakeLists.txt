cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


project(Lumen CXX)

if (WIN32)

    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    set(CMAKE_TOOLCHAIN_FILE "F:/repos/vcpkg/scripts/buildsystems/vcpkg.cmake")

    file(GLOB_RECURSE SOURCES "Lumen/*.cpp" "LumenCore/*.cpp")
    file(GLOB_RECURSE HEADERS "Lumen/*.hpp" "LumenCore/*.hpp" "Lumen/Externals/*.h")

    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

    find_package(glfw3 CONFIG REQUIRED)
    find_package(Vulkan REQUIRED)
    find_package(imgui CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
    find_package(tbb CONFIG REQUIRED)

    target_link_libraries(${PROJECT_NAME} PRIVATE glfw Vulkan::Vulkan imgui::imgui glm::glm TBB::tbb)

    set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

endif()