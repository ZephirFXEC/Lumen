cmake_minimum_required(VERSION 3.24)

set(NAME Lumen)

project(${NAME} VERSION 0.1.0)



set(MoltenVK_DIR "/Users/ecrema/VulkanSDK/1.3.236.0/MoltenVK")

if(EXISTS ${MoltenVK_DIR})
    set(Vulkan_INCLUDE_DIR "${MoltenVK_DIR}/include") # 1.1 Make sure this include path is correct
    set(Vulkan_LIBRARIES "${MoltenVK_DIR}/dylib") # 1.2 Make sure lib path is correct
    set(Vulkan_FOUND "True")
else()
    set(Vulkan_FOUND "False")
endif()

if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find Vulkan library!")
else()
    message(STATUS "Using vulkan include dir: ${Vulkan_INCLUDE_DIR}")
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

target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw imgui::imgui glm::glm TBB::tbb)