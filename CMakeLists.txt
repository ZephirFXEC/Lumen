cmake_minimum_required(VERSION 3.24.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(NAME Lumen)
project(${NAME} VERSION 0.1.0 LANGUAGES CXX)

file(GLOB_RECURSE SOURCES "Lumen/*.cpp" "LumenCore/*.cpp")
file(GLOB_RECURSE HEADERS "Lumen/*.hpp" "LumenCore/*.hpp" "Lumen/Externals/*.h")

add_executable(${NAME} ${SOURCES} ${HEADERS})
target_compile_features(${NAME} PUBLIC cxx_std_20)

find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(tbb CONFIG REQUIRED)
find_package(Vulkan REQUIRED)


target_link_libraries(${NAME} PUBLIC Vulkan::Vulkan glfw imgui::imgui glm::glm TBB::tbb)
set_property(TARGET ${NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)