#pragma once

#include <string>

#include "vulkan/vulkan.h"

namespace Clef {

	enum class ImageFormat
	{
		None = 0,
		RGBA,
		RGBA32F
	};

	class Image
	{
	public:
		Image(std::string_view path);
		Image(uint32_t width, uint32_t height, ImageFormat format, const void* data = nullptr);
		~Image();

		void setData(const void* data);

		inline VkDescriptorSet getDescriptorSet() const { return m_descriptorSet; }

		void resize(uint32_t width, uint32_t height);

		inline uint32_t getWidth() const { return m_width; }
		inline uint32_t getHeight() const { return m_height; }
	
	private:
		void allocateMemory(uint64_t size);
		void release();

	private:
		uint32_t m_width = 0, m_height = 0;

		VkImage m_image = nullptr;
		VkImageView m_imageView = nullptr;
		VkDeviceMemory m_memory = nullptr;
		VkSampler m_sampler = nullptr;

		ImageFormat m_format = ImageFormat::None;

		VkBuffer m_stagingBuffer = nullptr;
		VkDeviceMemory m_stagingBufferMemory = nullptr;

		size_t m_alignedSize = 0;

		VkDescriptorSet m_descriptorSet = nullptr;

		std::string m_filepath;
	};
}
