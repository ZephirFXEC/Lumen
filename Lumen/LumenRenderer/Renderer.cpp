//
// Created by enzoc on 30/09/2022.
//

#include "Renderer.hpp"

namespace LumenRender {
    void Renderer::Render() {

        for(uint32_t i = 0; i< m_Image->GetWidth() * m_Image->GetHeight(); i++)
        {
            m_ImageData[i] = Lumen::Random::UInt();
            m_ImageData[i] |= 0xFF000000;
        }

        m_Image->SetData(m_ImageData);
    }


    void Renderer::OnResize(uint32_t width, uint32_t height) {
        if(m_Image) {

            //no resize needed
            if(m_Image->GetWidth() == width && m_Image->GetHeight() == height) {
                return;
            }

            m_Image->Resize(width, height);
        } else {
            m_Image = std::make_shared<Lumen::Image>(width, height, Lumen::ImageFormat::RGBA);
        }

        delete[] m_ImageData;
        m_ImageData = new uint32_t[width * height];
    }
} // LumenRender