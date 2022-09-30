//
// Created by enzoc on 30/09/2022.
//

#ifndef LUMEN_APPLICATION_HPP
#define LUMEN_APPLICATION_HPP

#include "Layer.hpp"

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include <imgui.h>
#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Lumen {

    struct ApplicationSpecification {
        std::string Name = "Walnut App";
        uint32_t Width = 1600;
        uint32_t Height = 900;
    };

    class Application {
    public:
        explicit Application(const ApplicationSpecification &applicationSpecification = ApplicationSpecification());

        ~Application();

        void Run();

        void SetMenubarCallback(const std::function<void()> &menubarCallback) { m_MenubarCallback = menubarCallback; }

        template<typename T>
        void PushLayer() {
            static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
            m_LayerStack.emplace_back(std::make_shared<T>())->OnAttach();
        }

        void PushLayer(const std::shared_ptr<Layer> &layer) {
            m_LayerStack.emplace_back(layer);
            layer->OnAttach();
        }

        void Close();

        static VkInstance GetInstance();

        static VkPhysicalDevice GetPhysicalDevice();

        static VkDevice GetDevice();

        static VkCommandBuffer GetCommandBuffer(bool begin);

        static void FlushCommandBuffer(VkCommandBuffer commandBuffer);

        static void SubmitResourceFree(std::function<void()> &&func);

    private:
        void Init();

        void Shutdown();

    private:
        ApplicationSpecification m_Specification;
        GLFWwindow *m_WindowHandle = nullptr;
        bool m_Running = false;

        std::vector<std::shared_ptr<Layer>> m_LayerStack;
        std::function<void()> m_MenubarCallback;
    };

    // Implemented by CLIENT
    Application *CreateApplication(int argc, char **argv);

} // Lumen

#endif //LUMEN_APPLICATION_HPP