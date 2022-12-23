//
// Created by enzoc on 30/09/2022.
//

#include "Image.hpp"
#include "Application.hpp"
#include <imgui.h>
#include <imgui_impl_vulkan.h>

#define STB_IMAGE_IMPLEMENTATION


namespace Lumen {

    static void check_vk_result(VkResult err, uint32_t line) {
        if (err == 0) { return; }
        fprintf(stderr, "[vulkan] Error: VkResult = %d at %d", err, line);
        if (err < 0) { abort(); }
    }

    namespace Utils {

        static auto GetVulkanMemoryType(VkMemoryPropertyFlags properties, uint32_t type_bits) -> uint32_t {
            VkPhysicalDeviceMemoryProperties prop;
            vkGetPhysicalDeviceMemoryProperties(Application::GetPhysicalDevice(), &prop);
            for (uint32_t i = 0; i < prop.memoryTypeCount; i++) {
                if ((prop.memoryTypes[i].propertyFlags & properties) == properties && ((type_bits & (1 << i)) != 0U)) {
                    return i;
                }
            }

            return 0xffffffff;
        }

        static auto BytesPerPixel(ImageFormat format) -> uint32_t {
            switch (format) {
                case ImageFormat::RGBA:
                    return 4;
                case ImageFormat::RGBA32F:
                    return 16;
                case ImageFormat::None:
                    break;
            }
            return 0;
        }

        static auto LumenFormatToVulkanFormat(ImageFormat format) -> VkFormat {
            switch (format) {
                case ImageFormat::RGBA:
                    return VK_FORMAT_R8G8B8A8_UNORM;
                case ImageFormat::RGBA32F:
                    return VK_FORMAT_R32G32B32A32_SFLOAT;
                case ImageFormat::None:
                    break;
            }
            return static_cast<VkFormat>(0);
        }

    }// namespace Utils
/*
Image::Image(std::string_view path) : m_Filepath(path)
{
  int width = 0;
  int height = 0;
  int channels = 0;
  uint8_t *data = nullptr;

  if (stbi_is_hdr(m_Filepath.c_str()) != 0) {
    data = reinterpret_cast<uint8_t *>(stbi_loadf(m_Filepath.c_str(), &width, &height, &channels, 4));
    m_Format = ImageFormat::RGBA32F;
  } else {
    data = stbi_load(m_Filepath.c_str(), &width, &height, &channels, 4);
    m_Format = ImageFormat::RGBA;
  }

  m_Width = static_cast<uint32_t>(width);
  m_Height = static_cast<uint32_t>(height);

  AllocateMemory();
  SetData(data);
  stbi_image_free(data);
}
*/

    Image::Image(uint32_t width, uint32_t height, ImageFormat format, const void *data)
            : m_Width(width), m_Height(height), m_Format(format) {
        AllocateMemory();
        if (data != nullptr) { SetData(data); }
    }

    Image::~Image() { Release(); }

    void Image::AllocateMemory() {
        VkDevice device = Application::GetDevice();

        VkResult err;

        VkFormat const vulkanFormat = Utils::LumenFormatToVulkanFormat(m_Format);

        // Create the Image
        {
            VkImageCreateInfo const info = {.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                    .imageType = VK_IMAGE_TYPE_2D,
                    .format = vulkanFormat,
                    .extent = {m_Width, m_Height, 1},
                    .mipLevels = 1,
                    .arrayLayers = 1,
                    .samples = VK_SAMPLE_COUNT_1_BIT,
                    .tiling = VK_IMAGE_TILING_OPTIMAL,
                    .usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED};

            err = vkCreateImage(device, &info, nullptr, &m_Image);
            check_vk_result(err, __LINE__);
            VkMemoryRequirements req;
            vkGetImageMemoryRequirements(device, m_Image, &req);

            VkMemoryAllocateInfo const alloc_info = {.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                    .allocationSize = req.size,
                    .memoryTypeIndex = Utils::GetVulkanMemoryType(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                                  req.memoryTypeBits)};

            err = vkAllocateMemory(device, &alloc_info, nullptr, &m_Memory);
            check_vk_result(err, __LINE__);
            err = vkBindImageMemory(device, m_Image, m_Memory, 0);
            check_vk_result(err, __LINE__);
        }

        // Create the Image View:
        {
            VkImageViewCreateInfo const info = {.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                    .image = m_Image,
                    .viewType = VK_IMAGE_VIEW_TYPE_2D,
                    .format = vulkanFormat,
                    .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1}};

            err = vkCreateImageView(device, &info, nullptr, &m_ImageView);
            check_vk_result(err, __LINE__);
        }

        // Create sampler:
        {
            VkSamplerCreateInfo const info = {.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                    .magFilter = VK_FILTER_LINEAR,
                    .minFilter = VK_FILTER_LINEAR,
                    .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
                    .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                    .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                    .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                    .mipLodBias = 0.0F,
                    .anisotropyEnable = VK_TRUE,
                    .maxAnisotropy = 1.0F,
                    .compareEnable = VK_FALSE,
                    .compareOp = VK_COMPARE_OP_ALWAYS,
                    .minLod = -1000.0F,
                    .maxLod = 1000.0F,
                    .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
                    .unnormalizedCoordinates = VK_FALSE};

            err = vkCreateSampler(device, &info, nullptr, &m_Sampler);
            check_vk_result(err, __LINE__);
        }

        // Create the Descriptor Set:
        m_DescriptorSet = ImGui_ImplVulkan_AddTexture(m_Sampler, m_ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    void Image::Release() {
        Application::SubmitResourceFree([sampler = m_Sampler,
                                                imageView = m_ImageView,
                                                image = m_Image,
                                                memory = m_Memory,
                                                stagingBuffer = m_StagingBuffer,
                                                stagingBufferMemory = m_StagingBufferMemory]() {
            VkDevice device = Application::GetDevice();

            vkDestroySampler(device, sampler, nullptr);
            vkDestroyImageView(device, imageView, nullptr);
            vkDestroyImage(device, image, nullptr);
            vkFreeMemory(device, memory, nullptr);
            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);
        });

        m_Sampler = nullptr;
        m_ImageView = nullptr;
        m_Image = nullptr;
        m_Memory = nullptr;
        m_StagingBuffer = nullptr;
        m_StagingBufferMemory = nullptr;
    }

    void Image::SetData(const void *data) {
        VkDevice device = Application::GetDevice();

        size_t const upload_size = m_Width * m_Height * Utils::BytesPerPixel(m_Format);

        VkResult err;

        if (m_StagingBuffer == nullptr) {
            // Create the Upload Buffer
            {
                VkBufferCreateInfo const buffer_info = {.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                        .size = upload_size,
                        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                        .sharingMode = VK_SHARING_MODE_EXCLUSIVE};

                err = vkCreateBuffer(device, &buffer_info, nullptr, &m_StagingBuffer);
                check_vk_result(err, __LINE__);
                VkMemoryRequirements req;
                vkGetBufferMemoryRequirements(device, m_StagingBuffer, &req);
                m_AlignedSize = req.size;

                VkMemoryAllocateInfo const alloc_info = {.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                        .allocationSize = req.size,
                        .memoryTypeIndex = Utils::GetVulkanMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                                                      req.memoryTypeBits)};

                err = vkAllocateMemory(device, &alloc_info, nullptr, &m_StagingBufferMemory);
                check_vk_result(err, __LINE__);
                err = vkBindBufferMemory(device, m_StagingBuffer, m_StagingBufferMemory, 0);
                check_vk_result(err, __LINE__);
            }
        }

        // Upload to Buffer
        {
            char *map = nullptr;
            err = vkMapMemory(device, m_StagingBufferMemory, 0, m_AlignedSize, 0, reinterpret_cast<void **>(&map));
            check_vk_result(err, __LINE__);
            memcpy(map, data, upload_size);
            VkMappedMemoryRange range[1] = {};
            range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            range[0].memory = m_StagingBufferMemory;
            range[0].size = m_AlignedSize;
            err = vkFlushMappedMemoryRanges(device, 1, range);
            check_vk_result(err, __LINE__);
            vkUnmapMemory(device, m_StagingBufferMemory);
        }


        // Copy to Image
        {
            VkCommandBuffer command_buffer = Application::GetCommandBuffer(true);

            VkImageMemoryBarrier const copy_barrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                    .srcAccessMask = 0,
                    .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                    .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                    .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .image = m_Image,
                    .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1}};

            vkCmdPipelineBarrier(command_buffer,
                                 VK_PIPELINE_STAGE_HOST_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 0,
                                 0,
                                 nullptr,
                                 0,
                                 nullptr,
                                 1,
                                 &copy_barrier);

            VkBufferImageCopy const region = {.bufferOffset = 0,
                    .bufferRowLength = 0,
                    .bufferImageHeight = 0,
                    .imageSubresource = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                            .mipLevel = 0,
                            .baseArrayLayer = 0,
                            .layerCount = 1},
                    .imageOffset = {0, 0, 0},
                    .imageExtent = {m_Width, m_Height, 1}};

            vkCmdCopyBufferToImage(command_buffer, m_StagingBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                                   &region);

            VkImageMemoryBarrier const use_barrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                    .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                    .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                    .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .image = m_Image,
                    .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1}};

            vkCmdPipelineBarrier(command_buffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                 0,
                                 0,
                                 nullptr,
                                 0,
                                 nullptr,
                                 1,
                                 &use_barrier);

            Application::FlushCommandBuffer(command_buffer);
        }
    }

    void Image::Resize(uint32_t width, uint32_t height) {
        if ((m_Image != nullptr) && m_Width == width && m_Height == height) { return; }

        // TODO: max size?


        m_Width = width;
        m_Height = height;

        Release();
        AllocateMemory();
    }
}// namespace Lumen