//
// Created by enzoc on 30/09/2022.
//

#ifndef LUMEN_APPLICATION_HPP
#define LUMEN_APPLICATION_HPP

#include "Layer.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <imgui.h>
#include <vulkan/vulkan.h>


struct GLFWwindow;

namespace Lumen {

struct ApplicationSpecification
{
    std::string Name = std::string("Lumen Render");
    int Width = 1600;
    int Height = 900;
};

class Application
{
public:
  explicit Application(ApplicationSpecification applicationSpecification = ApplicationSpecification());

  ~Application();

  static auto Get() -> Application &;

  void Run();

  void Close();

  void SetMenubarCallback(const std::function<void()> &menubarCallback) { m_MenubarCallback = menubarCallback; }

  static auto GetTime() -> float;

  [[nodiscard]] __forceinline auto GetWindowHandle() const & -> GLFWwindow * { return m_WindowHandle; }


  template<typename T> void PushLayer()
  {
    static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
    m_LayerStack.emplace_back(std::make_shared<T>())->OnAttach();
  }

  void PushLayer(const std::shared_ptr<Layer> &layer)
  {
    m_LayerStack.emplace_back(layer);
    layer->OnAttach();
  }


  static auto GetInstance() -> VkInstance;

  static auto GetPhysicalDevice() -> VkPhysicalDevice;

  static auto GetDevice() -> VkDevice;

  static auto GetCommandBuffer(bool begin) -> VkCommandBuffer;

  static void FlushCommandBuffer(VkCommandBuffer commandBuffer);

  static void SubmitResourceFree(std::function<void()> &&func);

private:
  void Init();
  void Shutdown();


  ApplicationSpecification m_Specification;
  GLFWwindow *m_WindowHandle = nullptr;
  bool m_Running = false;

  float m_TimeStep = 0.0F;
  float m_FrameTime = 0.0F;
  float m_LastFrameTime = 0.0F;

  std::vector<std::shared_ptr<Layer>> m_LayerStack;
  std::function<void()> m_MenubarCallback;
};

// Implemented by CLIENT
auto CreateApplication(int argc, char **argv) -> Application *;

}// namespace Lumen

#endif// LUMEN_APPLICATION_HPP