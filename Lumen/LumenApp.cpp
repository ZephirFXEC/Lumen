#include "../LumenCore/Application.hpp"
#include "../LumenCore/EntryPoint.hpp"
#include "../LumenCore/Image.hpp"
#include "../LumenCore/Random.hpp"
#include "../LumenCore/Timer.hpp"

class ExampleLayer : public Lumen::Layer {
public:
    virtual void OnUIRender() override {
        ImGui::Begin("Property");
        if(ImGui::Button("Render"))
        {
            Render();
        }

        ImGui::Text("Last Render : %.3fms", m_ElapsedTime);
        ImGui::End();

        ImGui::Begin("Viewport");

        m_ViewportWidth = (int) ImGui::GetContentRegionAvail().x;
        m_ViewPortHeight = (int) ImGui::GetContentRegionAvail().y;

        if(m_Image) {
            ImGui::Image(m_Image->GetDescriptorSet(), {(float) m_Image->GetWidth(), (float)m_Image->GetHeight()});
        }

        ImGui::End();

    }


    void Render() {
        Lumen::Timer timer;

        if(!m_Image || m_Image->GetWidth() != m_ViewportWidth || m_Image->GetHeight() != m_ViewPortHeight)
        {
            m_Image = std::make_shared<Lumen::Image>(m_ViewportWidth, m_ViewPortHeight, Lumen::ImageFormat::RGBA);
            delete[] m_ImageData;
            m_ImageData = new uint32_t[m_ViewportWidth * m_ViewPortHeight];
        }

        for(uint32_t i = 0; i< m_ViewportWidth * m_ViewPortHeight; i++)
        {
            m_ImageData[i] = Lumen::Random::UInt();
            m_ImageData[i] |= 0xFF000000;
        }

        m_Image->SetData(m_ImageData);

        m_ElapsedTime = timer.ElapsedMillis();
    }


private:
    std::shared_ptr<Lumen::Image> m_Image;
    uint32_t *m_ImageData;
    int m_ViewportWidth, m_ViewPortHeight;
    float m_ElapsedTime;
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