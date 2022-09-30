#include "LumenCore/Application.hpp"
#include "LumenCore/EntryPoint.hpp"
#include "LumenCore/Image.hpp"

class ExampleLayer : public Lumen::Layer {
public:
    virtual void OnUIRender() override {
        ImGui::Begin("Hello");
        ImGui::Button("Button");
        ImGui::End();

        ImGui::ShowDemoWindow();
    }
};

Lumen::Application *Lumen::CreateApplication(int argc, char **argv) {
    Lumen::ApplicationSpecification spec;
    spec.Name = "Walnut Example";

    Lumen::Application *app = new Lumen::Application(spec);
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