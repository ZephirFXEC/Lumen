#include "../LumenCore/Layer.hpp"
#include "../LumenCore/Application.hpp"
#include "../LumenCore/EntryPoint.hpp"
#include "../LumenCore/Image.hpp"
#include "../LumenCore/Timer.hpp"
#include "LumenRenderer/Renderer.hpp"
#include "LumenRenderer/Camera.hpp"

class ExampleLayer : public Lumen::Layer {
public:
    ExampleLayer()
    : m_Camera(45.0f, 0.1f, 100.0f) {}

    void OnUpdate(float ts) override {
        m_Camera.OnUpdate(ts);
    }

    void OnUIRender() override {
        ImGui::Begin("Property");
        if (ImGui::Button("Render")) {
            Render();
        }

        ImGui::Text("Last Render : %.3fms", m_ElapsedTime);
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport");

        m_ViewportWidth = (int) ImGui::GetContentRegionAvail().x;
        m_ViewPortHeight = (int) ImGui::GetContentRegionAvail().y;

        auto image = m_Renderer.GetFinalImage();

        if (image) {
            ImGui::Image(image->GetDescriptorSet(), { (float) image->GetWidth(), (float) image->GetHeight() }, ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::End();
        ImGui::PopStyleVar();

        Render();
    }


    void Render() {
        Lumen::Timer timer;

        m_Renderer.OnResize(m_ViewportWidth, m_ViewPortHeight);
        m_Camera.OnResize(m_ViewportWidth, m_ViewPortHeight);

        m_Renderer.Render(m_Camera);

        m_ElapsedTime = timer.ElapsedMillis();
    }


private:
    LumenRender::Renderer m_Renderer;
    LumenRender::Camera m_Camera;

    int m_ViewportWidth{}, m_ViewPortHeight{};
    float m_ElapsedTime{};
};

Lumen::Application *Lumen::CreateApplication(int argc, char **argv) {
    Lumen::ApplicationSpecification spec;
    spec.Name = "Lumen";

    auto *app = new Lumen::Application(spec);
    app->PushLayer<ExampleLayer>();
    app->SetMenubarCallback([app]() {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit")) {
                app->Close();
            }
            ImGui::EndMenu();
        }
    });
    return app;
}