cmake_minimum_required(VERSION 3.24)

set(NAME Lumen)

project(${NAME} VERSION 0.1.0)


# 1. Set VULKAN_SDK_PATH in .env.cmake to target specific vulkan version
if (DEFINED VULKAN_SDK_PATH)
    set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include") # 1.1 Make sure this include path is correct
    set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib") # 1.2 Make sure lib path is correct
    set(Vulkan_FOUND "True")
else()
    find_package(Vulkan REQUIRED) # throws error if could not find Vulkan
    message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")
endif()
if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find Vulkan library!")
else()
    message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")
endif()

file(GLOB_RECURSE SOURCES "Lumen/*.cpp" "LumenCore/*.cpp")
file(GLOB_RECURSE HEADERS "Lumen/*.hpp" "LumenCore/*.hpp" "Lumen/Externals/*.h")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(tbb CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw imgui::imgui glm::glm TBB::tbb ${Vulkan_LIBRARIES})