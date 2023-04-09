cmake_minimum_required(VERSION 3.23)
project(Lumen)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)

    find_package(glfw3 CONFIG REQUIRED)
    find_package(Vulkan REQUIRED)
    find_package(imgui CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
    find_package(TBB CONFIG REQUIRED)
    find_package(embree 4 REQUIRED)

    include_directories(D:/DEV/embree-4.0.1.x64.windows/include)

    file(GLOB_RECURSE SOURCES "Lumen/*.cpp" "LumenCore/*.cpp")
    file(GLOB_RECURSE HEADERS "Lumen/*.hpp" "LumenCore/*.hpp" "Lumen/Externals/*.h")


    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

    target_link_libraries(Lumen PRIVATE glfw Vulkan::Vulkan imgui::imgui glm::glm TBB::tbb embree)

    target_compile_definitions(${PROJECT_NAME} PUBLIC __SSE4_2__)
    target_compile_definitions(${PROJECT_NAME} PUBLIC __AVX2__)
    target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)

    set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()