#pragma once
#include "Graphics/Graphics.h"
#include "Graphics/Renderer.h"
#include "Core/Types.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"

#include <vulkan/vulkan.hpp>
#include <array>


namespace Amber
{
namespace Vulkan
{
struct Vertex
{
	Vector3f pos;
	Vector3f color;
	Vector2f texCoord;

	bool operator==(const Vertex& other) const
	{
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}

	static vk::VertexInputBindingDescription getBindingDescription()
	{
		vk::VertexInputBindingDescription bindingDescription = vk::VertexInputBindingDescription()
			.setBinding(0)
			.setStride(sizeof(Vertex))
			.setInputRate(vk::VertexInputRate::eVertex);

		return bindingDescription;
	}

	static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions()
	{
		std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions =
		{
			vk::VertexInputAttributeDescription()
			.setBinding(0)
			.setLocation(0)
			.setFormat(vk::Format::eR32G32B32Sfloat)
			.setOffset(offsetof(Vertex, pos)),
			vk::VertexInputAttributeDescription()
			.setBinding(0)
			.setLocation(1)
			.setFormat(vk::Format::eR32G32B32Sfloat)
			.setOffset(offsetof(Vertex, color)),
			vk::VertexInputAttributeDescription()
			.setBinding(0)
			.setLocation(2)
			.setFormat(vk::Format::eR32G32Sfloat)
			.setOffset(offsetof(Vertex, texCoord))
		};

		return attributeDescriptions;
	}
};

struct UniformBufferObject
{
	Matrix4x4f model;
	Matrix4x4f view;
	Matrix4x4f proj;
};

struct SwapchainBuffer
{
	vk::Image image;
	vk::CommandBuffer commandBuffer;
	vk::CommandBuffer graphicsToPresentCommandBuffer;
	vk::ImageView imageView;
};

class VulkanRenderer : public Renderer
{
private:
	vk::Instance instance;

	bool useValidationLayers = true;
	uint32 enabledExtensionCount;
	uint32 enabledLayerCount;
	std::array<char const*, 64> extensionNames;
	std::array<char const*, 64> enabledLayers;

	VkDebugReportCallbackEXT callback;

	vk::SurfaceKHR surface;

	vk::PhysicalDevice gpu;
	vk::PhysicalDeviceProperties gpuProperties;
	std::unique_ptr<vk::QueueFamilyProperties[]> queueFamilyProperties;
	vk::Device device;

	uint32 queueFamilyCount;

	bool separatePresentQueue;
	vk::Queue graphicsQueue;
	vk::Queue presentQueue;
	uint32 graphicsQueueFamilyIndex;
	uint32 presentQueueFamilyIndex;

	vk::Format swapchainImageFormat;
	uint32 swapchainImageCount;
	vk::SwapchainKHR swapchain;
	vk::Extent2D swapchainExtent;
	std::vector<SwapchainBuffer> swapchainBuffers;
	std::vector<vk::Framebuffer> swapchainFramebuffers;
	// Can be changed to only one buffer with offsets

	vk::ColorSpaceKHR colorSpace;
	vk::PhysicalDeviceMemoryProperties memoryProperties;

	std::vector<Vertex> vertices;
	std::vector<uint32> indices;
	vk::Buffer vertexBuffer;
	vk::Buffer indexBuffer;
	vk::DeviceMemory vertexBufferMemory;
	vk::DeviceMemory indexBufferMemory;

	vk::Buffer uniformStagingBuffer;
	vk::DeviceMemory uniformStagingBufferMemory;
	vk::Buffer uniformBuffer;
	vk::DeviceMemory uniformBufferMemory;

	vk::Image textureImage;
	vk::DeviceMemory textureImageMemory;
	vk::ImageView textureImageView;
	vk::Sampler textureSampler;
	vk::Image depthImage;
	vk::DeviceMemory depthImageMemory;
	vk::ImageView depthImageView;

	vk::DescriptorPool descriptorPool;
	vk::DescriptorSet descriptorSet;

	vk::RenderPass renderPass;

	vk::CommandPool commandPool;

	vk::DescriptorSetLayout descriptorSetLayout;
	vk::PipelineLayout pipelineLayout;
	vk::Pipeline pipeline;

	vk::Semaphore imageAvailableSemaphore;
	vk::Semaphore renderFinishedSemaphore;

	void createInstance();
	void setupDebugCallback();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapchain(int width, int heigth);
	void createImageViews();
	void createRenderPass();
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createDepthResources();
	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();
	void loadModel();
	void createVertexBuffer();
	void createIndexBuffer();
	void createUniformBuffer();
	void createDescriptorPool();
	void createDescriptorSet();
	void createCommandBuffers();
	void createSemaphores();

	void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory);
	void createImage(uint32 width, uint32 height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory);
	void createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags, vk::ImageView& imageView);

	void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
	void copyImage(vk::Image srcImage, vk::Image dstImage, uint32 width, uint32 height);

	vk::CommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(vk::CommandBuffer commandBuffer);

	void transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

	void updateUniformBuffer();

	vk::Format findDepthFormat();
	bool hasStencilComponent(vk::Format format);

	uint32 findMemoryType(uint32 typeFilter, vk::MemoryPropertyFlags properties);
	vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);

	void recreateSwapchain(int newWidth, int newHeight);

public:
	VulkanRenderer();
	~VulkanRenderer();

	virtual void init() override;
	virtual void handleWindowSizeChange(int newWidth, int newHeight) override;
	virtual void renderAll(Time delta) override;
	virtual void finalRenderToMainFramebuffer() override;
};

}
}

namespace std
{
	using namespace Amber;
	template<> struct hash<Vulkan::Vertex>
	{
		size_t operator()(Vulkan::Vertex const& vertex) const
		{
			return ((hash<Vector3f>()(vertex.pos) ^ (hash<Vector3f>()(vertex.color) << 1)) >> 1) ^ (hash<Vector2f>()(vertex.texCoord) << 1);
		}
	};
}