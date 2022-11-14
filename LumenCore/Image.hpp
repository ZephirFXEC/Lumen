//
// Created by enzoc on 30/09/2022.
//

#ifndef LUMEN_IMAGE_HPP
#define LUMEN_IMAGE_HPP

#include <string>
#include <vulkan/vulkan.h>
#include "Application.hpp"

namespace Lumen {

    enum class ImageFormat {
        None = 0,
        RGBA,
        RGBA32F
    };

    class Image {
    public:
        explicit Image(std::string_view path);

        Image(uint32_t width, uint32_t height, ImageFormat format, const void *data = nullptr);

        ~Image();

        void SetData(const void *data);

        [[nodiscard]] auto GetDescriptorSet() const -> VkDescriptorSet { return m_DescriptorSet; }

        void Resize(uint32_t width, uint32_t height);

        [[nodiscard]] auto GetWidth() const -> uint32_t { return m_Width; }

        [[nodiscard]] auto GetHeight() const -> uint32_t { return m_Height; }

    private:
        void AllocateMemory();

        void Release();


        uint32_t m_Width = 0, m_Height = 0;

        VkImage m_Image = nullptr;
        VkImageView m_ImageView = nullptr;
        VkDeviceMemory m_Memory = nullptr;
        VkSampler m_Sampler = nullptr;

        ImageFormat m_Format = ImageFormat::None;

        VkBuffer m_StagingBuffer = nullptr;
        VkDeviceMemory m_StagingBufferMemory = nullptr;

        size_t m_AlignedSize = 0;

        VkDescriptorSet m_DescriptorSet = nullptr;

        std::string m_Filepath;
    };

} // Lumen

#endif //LUMEN_IMAGE_HPP