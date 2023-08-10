#include "Image.h"

#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"

#include "Application.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Clef {

	namespace Utils {

		static uint32_t GetVulkanMemoryType(VkMemoryPropertyFlags properties, uint32_t type_bits)
		{
			VkPhysicalDeviceMemoryProperties prop;
			vkGetPhysicalDeviceMemoryProperties(Application::getPhysicalDevice(), &prop);
			for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
			{
				if ((prop.memoryTypes[i].propertyFlags & properties) == properties && type_bits & (1 << i))
					return i;
			}

			return 0xffffffff;
		}

		static uint32_t BytesPerPixel(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGBA:    return 4;
			case ImageFormat::RGBA32F: return 16;
			}
			return 0;
		}

		static VkFormat ClefFormatToVulkanFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGBA:    return VK_FORMAT_R8G8B8A8_UNORM;
			case ImageFormat::RGBA32F: return VK_FORMAT_R32G32B32A32_SFLOAT;
			}
			return (VkFormat)0;
		}

	}

	Image::Image(std::string_view path)
		: m_filepath(path)
	{
		int width, height, channels;
		uint8_t* data = nullptr;

		if (stbi_is_hdr(m_filepath.c_str()))
		{
			data = (uint8_t*)stbi_loadf(m_filepath.c_str(), &width, &height, &channels, 4);
			m_format = ImageFormat::RGBA32F;
		}
		else
		{
			data = stbi_load(m_filepath.c_str(), &width, &height, &channels, 4);
			m_format = ImageFormat::RGBA;
		}

		m_width = width;
		m_height = height;

		allocateMemory(m_width * m_height * Utils::BytesPerPixel(m_format));
		setData(data);
		stbi_image_free(data);
	}

	Image::Image(uint32_t width, uint32_t height, ImageFormat format, const void* data)
		: m_width(width), m_height(height), m_format(format)
	{
		allocateMemory(m_width * m_height * Utils::BytesPerPixel(m_format));
		if (data)
			setData(data);
	}

	Image::~Image()
	{
		release();
	}

	void Image::allocateMemory(uint64_t size)
	{
		VkDevice device = Application::getDevice();

		VkResult err;

		VkFormat vulkanFormat = Utils::ClefFormatToVulkanFormat(m_format);

		// Create the Image
		{
			VkImageCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			info.imageType = VK_IMAGE_TYPE_2D;
			info.format = vulkanFormat;
			info.extent.width = m_width;
			info.extent.height = m_height;
			info.extent.depth = 1;
			info.mipLevels = 1;
			info.arrayLayers = 1;
			info.samples = VK_SAMPLE_COUNT_1_BIT;
			info.tiling = VK_IMAGE_TILING_OPTIMAL;
			info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			err = vkCreateImage(device, &info, nullptr, &m_image);
			check_vk_result(err);
			VkMemoryRequirements req;
			vkGetImageMemoryRequirements(device, m_image, &req);
			VkMemoryAllocateInfo alloc_info = {};
			alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			alloc_info.allocationSize = req.size;
			alloc_info.memoryTypeIndex = Utils::GetVulkanMemoryType(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, req.memoryTypeBits);
			err = vkAllocateMemory(device, &alloc_info, nullptr, &m_memory);
			check_vk_result(err);
			err = vkBindImageMemory(device, m_image, m_memory, 0);
			check_vk_result(err);
		}

		// Create the Image View:
		{
			VkImageViewCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			info.image = m_image;
			info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			info.format = vulkanFormat;
			info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			info.subresourceRange.levelCount = 1;
			info.subresourceRange.layerCount = 1;
			err = vkCreateImageView(device, &info, nullptr, &m_imageView);
			check_vk_result(err);
		}

		// Create sampler:
		{
			VkSamplerCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			info.magFilter = VK_FILTER_LINEAR;
			info.minFilter = VK_FILTER_LINEAR;
			info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			info.minLod = -1000;
			info.maxLod = 1000;
			info.maxAnisotropy = 1.0f;
			VkResult err = vkCreateSampler(device, &info, nullptr, &m_sampler);
			check_vk_result(err);
		}

		// Create the Descriptor Set:
		m_descriptorSet = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(m_sampler, m_imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	void Image::release()
	{
		Application::submitResourceFree([sampler = m_sampler, imageView = m_imageView, image = m_image,
			memory = m_memory, stagingBuffer = m_stagingBuffer, stagingBufferMemory = m_stagingBufferMemory]()
			{
				VkDevice device = Application::getDevice();

				vkDestroySampler(device, sampler, nullptr);
				vkDestroyImageView(device, imageView, nullptr);
				vkDestroyImage(device, image, nullptr);
				vkFreeMemory(device, memory, nullptr);
				vkDestroyBuffer(device, stagingBuffer, nullptr);
				vkFreeMemory(device, stagingBufferMemory, nullptr);
			});

		m_sampler = nullptr;
		m_imageView = nullptr;
		m_image = nullptr;
		m_memory = nullptr;
		m_stagingBuffer = nullptr;
		m_stagingBufferMemory = nullptr;
	}

	void Image::setData(const void* data)
	{
		VkDevice device = Application::getDevice();

		size_t upload_size = m_width * m_height * Utils::BytesPerPixel(m_format);

		VkResult err;

		if (!m_stagingBuffer)
		{
			// Create the Upload Buffer
			{
				VkBufferCreateInfo buffer_info = {};
				buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				buffer_info.size = upload_size;
				buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
				buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
				err = vkCreateBuffer(device, &buffer_info, nullptr, &m_stagingBuffer);
				check_vk_result(err);
				VkMemoryRequirements req;
				vkGetBufferMemoryRequirements(device, m_stagingBuffer, &req);
				m_alignedSize = req.size;
				VkMemoryAllocateInfo alloc_info = {};
				alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				alloc_info.allocationSize = req.size;
				alloc_info.memoryTypeIndex = Utils::GetVulkanMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
				err = vkAllocateMemory(device, &alloc_info, nullptr, &m_stagingBufferMemory);
				check_vk_result(err);
				err = vkBindBufferMemory(device, m_stagingBuffer, m_stagingBufferMemory, 0);
				check_vk_result(err);
			}

		}

		// Upload to Buffer
		{
			char* map = NULL;
			err = vkMapMemory(device, m_stagingBufferMemory, 0, m_alignedSize, 0, (void**)(&map));
			check_vk_result(err);
			memcpy(map, data, upload_size);
			VkMappedMemoryRange range[1] = {};
			range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			range[0].memory = m_stagingBufferMemory;
			range[0].size = m_alignedSize;
			err = vkFlushMappedMemoryRanges(device, 1, range);
			check_vk_result(err);
			vkUnmapMemory(device, m_stagingBufferMemory);
		}


		// Copy to Image
		{
			VkCommandBuffer command_buffer = Application::getCommandBuffer(true);

			VkImageMemoryBarrier copy_barrier = {};
			copy_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			copy_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			copy_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			copy_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			copy_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			copy_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			copy_barrier.image = m_image;
			copy_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copy_barrier.subresourceRange.levelCount = 1;
			copy_barrier.subresourceRange.layerCount = 1;
			vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &copy_barrier);

			VkBufferImageCopy region = {};
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.layerCount = 1;
			region.imageExtent.width = m_width;
			region.imageExtent.height = m_height;
			region.imageExtent.depth = 1;
			vkCmdCopyBufferToImage(command_buffer, m_stagingBuffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

			VkImageMemoryBarrier use_barrier = {};
			use_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			use_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			use_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			use_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			use_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			use_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			use_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			use_barrier.image = m_image;
			use_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			use_barrier.subresourceRange.levelCount = 1;
			use_barrier.subresourceRange.layerCount = 1;
			vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &use_barrier);

			Application::flushCommandBuffer(command_buffer);
		}
	}

	void Image::resize(uint32_t width, uint32_t height)
	{
		if (m_image && m_width == width && m_height == height)
			return;

		// TODO: max size?

		m_width = width;
		m_height = height;

		release();
		allocateMemory(m_width * m_height * Utils::BytesPerPixel(m_format));
	}

}