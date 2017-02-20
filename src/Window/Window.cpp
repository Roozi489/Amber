#include "Window/Window.h"
#include "Window/ContextSettings.h"
#include "Window/Input.h"
#include "Core/Log.h"
#include "Core/String.h"
#include "Core/Globals.h"
#include "Graphics/GL/OpenGL.h"
#include "Graphics/Image.h"
#include "Systems/RenderSystem.h"
#include "Core/Utility.h"
#include "Window/File.h"

#include <unordered_map>

#include <SDL/SDL_syswm.h>
#include <stb/stb_image.h>
#include <vulkan/vulkan.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

namespace Amber
{

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData)
{
	Log::error(stringFormat("Vulkan validation layer: %s", msg));

	return VK_FALSE;
}

template<std::size_t SIZE>
static bool checkLayers(const std::array<const char*, SIZE>& checkNames, const std::vector<vk::LayerProperties>& layers)
{
	for (auto name : checkNames)
	{
		bool found = false;
		for (const auto& layer : layers)
		{
			if (!strcmp(name, layer.layerName))
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			Log::error(stringFormat("Cannot find layer: %s", name));
			return false;
		}
	}
	return true;
}

Window::~Window()
{
}

void Window::create(GraphicsApi graphicsApi, std::string title)
{
	m_graphicsApi = graphicsApi;
	switch (graphicsApi)
	{
	case GraphicsApi::OpenGL:
		m_title = title + " - OpenGL";
		createGLWindow();
		break;
	case GraphicsApi::Vulkan:
		m_title = title + " - Vulkan";
		createVulkanWindow();
		break;
	}
}	

void Window::createGLWindow()
{
	Log::info("Using OpenGL");

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	ContextSettings settings;
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, settings.majorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, settings.minorVersion);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, settings.depthBits);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, settings.stencilBits);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, settings.acceleratedVisual);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, toUnderlyingType(settings.profileMask));
	// TODO: add double buffering setting (its on by default)

	Log::info("Creating window.");
	// TODO: enable window size change
	m_sdlWindow = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_width, m_height, SDL_WINDOW_OPENGL);
	if (!m_sdlWindow)
		criticalError(SDL_GetError());

	setIcon("Textures/amber_icon.png");

	Log::info("Creating context.");
	m_context = SDL_GL_CreateContext(m_sdlWindow);
	if (!m_context)
		criticalError(SDL_GetError());

	SDL_GL_SetSwapInterval(settings.vsyncEnabled);
	SDL_GL_MakeCurrent(m_sdlWindow, m_context);

	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &m_actualGlVersionMajor);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &m_actualGlVersionMinor);

	Log::info(stringFormat("GL Context version: %d.%d", m_actualGlVersionMajor, m_actualGlVersionMinor));

	Log::info("Initializing GLEW.");
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
		criticalError("Failed to initialize GLEW");

	ignoreGlError();
}

void Window::createVulkanWindow()
{
	Log::info("Using Vulkan");

	createInstance();
	setupDebugCallback();
	createWindowAndSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapchain();
	createImageViews();
	createRenderPass();
	createDescriptorSetLayout();
	createGraphicsPipeline();
	createCommandPool();
	createDepthResources();
	createFramebuffers();
	createTextureImage();
	createTextureImageView();
	createTextureSampler();
	loadModel();
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffer();
	createDescriptorPool();
	createDescriptorSet();
	createCommandBuffers();
	createSemaphores();
}

void Window::destroy()
{
}

void Window::update()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			quit();
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				quit();
				break;
			default:
				break;
			}
			break;
		case SDL_KEYUP:
			break;
		case SDL_MOUSEMOTION:
			break;
		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				Log::info(stringFormat("Window %d resized to %dx%d", event.window.windowID, event.window.data1, event.window.data2));
				handleWindowSizeChange(event.window.data1, event.window.data2);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
}

void Window::display()
{
	static RenderSystem* renderSystem = g_world.getSystem<RenderSystem>();
	
	if (m_graphicsApi == GraphicsApi::OpenGL)
	{
		glViewport(0, 0, m_width, m_height);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		ShaderProgram* program = g_shaderManager.getShaderProgram("texPass");
		program->use();
		renderSystem->getOutColorTexture().activeAndBind(0);
		program->setUniform("tex", 0);
		renderSystem->drawFullscreenQuad();
		program->stopUsing();

		SDL_GL_SwapWindow(m_sdlWindow);
	}
	else if (m_graphicsApi == GraphicsApi::Vulkan)
	{
		// This shouldnt be in dipslay
		updateUniformBuffer();
		// #####################################




		uint32 imageIndex;
		auto result = device.acquireNextImageKHR(swapchain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
		if (result == vk::Result::eErrorOutOfDateKHR)
		{
			recreateSwapchain();
			return;
		}
		// Suboptimal considered here as Success
		else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
		{
			criticalError("Failed to acquire swap chain image");
		}

		vk::Semaphore waitSemaphores[] = { imageAvailableSemaphore };
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
		vk::Semaphore signalSemaphores[] = { renderFinishedSemaphore };

		vk::SubmitInfo submitInfo = vk::SubmitInfo()
			.setWaitSemaphoreCount(1)
			.setPWaitSemaphores(waitSemaphores)
			.setPWaitDstStageMask(waitStages)
			.setCommandBufferCount(1)
			.setPCommandBuffers(&swapchainBuffers[imageIndex].commandBuffer)
			.setSignalSemaphoreCount(1)
			.setPSignalSemaphores(signalSemaphores);

		result = graphicsQueue.submit(1, &submitInfo, VK_NULL_HANDLE);
		assert_amber(result == vk::Result::eSuccess, "Graphics queue submit failed");

		vk::SwapchainKHR swapchains[] = { swapchain };
		vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR()
			.setWaitSemaphoreCount(1)
			.setPWaitSemaphores(signalSemaphores)
			.setSwapchainCount(1)
			.setPSwapchains(swapchains)
			.setPImageIndices(&imageIndex)
			.setPResults(nullptr); // No need to check since only one swapchain is used

		result = presentQueue.presentKHR(&presentInfo);
		if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
		{
			recreateSwapchain();
		}
		else if (result != vk::Result::eSuccess)
		{
			criticalError("Failed to present swap chain image");
		}

		device.waitIdle();
	}
}

void Window::recreateSwapchain()
{
	device.waitIdle();

	createSwapchain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createDepthResources();
	createFramebuffers();
	createCommandBuffers();
}

void Window::createInstance()
{
	char const *const *instanceValidationLayers = nullptr;
	enabledExtensionCount = 0;
	enabledLayerCount = 0;

	auto instanceValidationLayersAlt1 = make_array("VK_LAYER_LUNARG_standard_validation");
	auto instanceValidationLayersAlt2 = make_array
	(
		"VK_LAYER_GOOGLE_threading",
		"VK_LAYER_LUNARG_parameter_validation",
		"VK_LAYER_LUNARG_object_tracker",
		"VK_LAYER_LUNARG_image",
		"VK_LAYER_LUNARG_core_validation",
		"VK_LAYER_LUNARG_swapchain",
		"VK_LAYER_GOOGLE_unique_objects"
	);

	// Look for validation layers
	vk::Bool32 validationFound = VK_FALSE;
	if (useValidationLayers)
	{
		uint32 instanceLayerCount = 0;
		uint32 validationLayerCount = 0;
		auto result = vk::enumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
		assert_amber(result == vk::Result::eSuccess, "Enumerate instance layers failed");

		instanceValidationLayers = instanceValidationLayersAlt1.data();
		if (instanceLayerCount > 0)
		{
			std::vector<vk::LayerProperties> instanceLayers;
			instanceLayers.resize(instanceLayerCount);
			result = vk::enumerateInstanceLayerProperties(&instanceLayerCount, instanceLayers.data());
			assert_amber(result == vk::Result::eSuccess, "Get instance layers failed");

			validationFound = checkLayers(instanceValidationLayersAlt1, instanceLayers);
			if (validationFound)
			{
				instanceValidationLayers = instanceValidationLayersAlt1.data();
				enabledLayerCount = static_cast<uint32>(instanceValidationLayersAlt1.size());
				validationFound = checkLayers(instanceValidationLayersAlt1, instanceLayers);
				validationLayerCount = static_cast<uint32>(instanceValidationLayersAlt1.size());
				for (uint32_t i = 0; i < validationLayerCount; i++)
				{
					enabledLayers[i] = instanceValidationLayers[i];
				}
			}
			else
			{
				// use alternative set of validation layers
				instanceValidationLayers = instanceValidationLayersAlt2.data();
				enabledLayerCount = static_cast<uint32>(instanceValidationLayersAlt2.size());
				validationFound = checkLayers(instanceValidationLayersAlt2, instanceLayers);
				validationLayerCount = static_cast<uint32>(instanceValidationLayersAlt2.size());
				for (uint32_t i = 0; i < validationLayerCount; i++)
				{
					enabledLayers[i] = instanceValidationLayers[i];
				}
			}
		}

		if (!validationFound)
		{
			criticalError("vkEnumerateInstanceLayerProperties failed to find required validation layer.\nvkCreateInstance Failure");
		}

		Log::info(stringFormat("Validation layers (%d):", validationLayerCount));
		for (uint32 i = 0; i < validationLayerCount; ++i)
			Log::info(stringFormat("   %s", enabledLayers[i]));
	}

	// Look for instance extensions
	bool surfaceExtFound = false;
	bool platformSurfaceExtFound = false;
#if defined(VK_USE_PLATFORM_XLIB_KHR)
	bool xlibSurfaceExtFound = false;
#endif
	extensionNames = {};

	uint32 instanceExtensionCount = 0;
	auto result = vk::enumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
	assert_amber(result == vk::Result::eSuccess, "Enumerate instance extension properties failed");

	if (instanceExtensionCount > 0)
	{
		std::unique_ptr<vk::ExtensionProperties[]> instanceExtensions(new vk::ExtensionProperties[instanceExtensionCount]);
		result = vk::enumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, instanceExtensions.get());
		assert_amber(result == vk::Result::eSuccess, "Get instance extension properties failed");

		for (uint32_t i = 0; i < instanceExtensionCount; i++)
		{
			if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instanceExtensions[i].extensionName))
			{
				surfaceExtFound = true;
				extensionNames[enabledExtensionCount++] = VK_KHR_SURFACE_EXTENSION_NAME;
			}
#if defined(VK_USE_PLATFORM_WIN32_KHR)
			if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, instanceExtensions[i].extensionName))
			{
				platformSurfaceExtFound = true;
				extensionNames[enabledExtensionCount++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
			}
#endif
#if defined(VK_USE_PLATFORM_XLIB_KHR)
			if (!strcmp(VK_KHR_XLIB_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName))
			{
				platformSurfaceExtFound = true;
				xlibSurfaceExtFound = true;
				extensionNames[enabledExtensionCount++] = VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
			}
#endif
#if defined(VK_USE_PLATFORM_XCB_KHR)
			if (!strcmp(VK_KHR_XCB_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName))
			{
				platformSurfaceExtFound = true;
				extensionNames[enabledExtensionCount++] = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
			}
#endif
#if defined(VK_USE_PLATFORM_WAYLAND_KHR)
			if (!strcmp(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) {
				platformSurfaceExtFound = true;
				extensionNames[enabledExtensionCount++] = VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
			}
#endif
			assert_amber(enabledExtensionCount < 64, "Too many vulkan extensions");
		}
	}

	if (!surfaceExtFound)
	{
		criticalError("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_SURFACE_EXTENSION_NAME " extension.\nvkCreateInstance Failure");
	}

	if (!platformSurfaceExtFound)
	{
#if defined(VK_USE_PLATFORM_WIN32_KHR)
		criticalError("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_WIN32_SURFACE_EXTENSION_NAME " extension.\nvkCreateInstance Failure");
#elif defined(VK_USE_PLATFORM_XCB_KHR)
		criticalError("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_XCB_SURFACE_EXTENSION_NAME " extension.\nvkCreateInstance Failure");
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
		criticalError("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME " extension.\nvkCreateInstance Failure");
#endif
	}

#if defined(VK_USE_PLATFORM_XLIB_KHR)
	if (use_xlib && !xlibSurfaceExtFound)
	{
		criticalError("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_XLIB_SURFACE_EXTENSION_NAME " extension.\nvkCreateInstance Failure");
	}
#endif

	if (useValidationLayers)
	{
		extensionNames[enabledExtensionCount++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
	}

	auto const appInfo = vk::ApplicationInfo()
		.setPApplicationName("Amber")
		.setApplicationVersion(0)
		.setPEngineName("Amber")
		.setEngineVersion(0)
		.setApiVersion(VK_API_VERSION_1_0);
	auto const instanceInfo = vk::InstanceCreateInfo()
		.setPApplicationInfo(&appInfo)
		.setEnabledLayerCount(enabledLayerCount)
		.setPpEnabledLayerNames(enabledLayers.data())
		.setEnabledExtensionCount(enabledExtensionCount)
		.setPpEnabledExtensionNames(extensionNames.data());


	result = vk::createInstance(&instanceInfo, nullptr, &instance);
	if (result == vk::Result::eErrorIncompatibleDriver)
	{
		criticalError("Cannot find a compatible Vulkan installable client driver (ICD). vkCreateInstance Failure");
	}
	else if (result == vk::Result::eErrorExtensionNotPresent) {
		criticalError("Cannot find a specified extension library. vkCreateInstance Failure");
	}
	else if (result != vk::Result::eSuccess)
	{
		criticalError("vkCreateInstance failed. vkCreateInstance Failure");
	}
}

void Window::setupDebugCallback()
{
	if (useValidationLayers)
	{
		PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback = VK_NULL_HANDLE;
		CreateDebugReportCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));

		if (CreateDebugReportCallback == nullptr)
		{
			Log::error("Unable to setup debug report callback");
		}
		else
		{
			VkDebugReportCallbackCreateInfoEXT createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
			createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
			createInfo.pfnCallback = debugCallback;

			if (CreateDebugReportCallback(instance, &createInfo, nullptr, &callback) != VK_SUCCESS)
			{
				criticalError("Failed to set up debug callback!");
			}
		}
		
		//vk::DebugReportCallbackCreateInfoEXT debugReportCallbackCreateInfo = vk::DebugReportCallbackCreateInfoEXT()
		//	.setFlags(vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning)
		//	.setPfnCallback(debugCallback);

		//vk::DebugReportCallbackEXT callback = nullptr;
		//auto pinfo = reinterpret_cast<const vk::DebugReportCallbackCreateInfoEXT*>(&debugReportCallbackCreateInfo);
		//auto result = instance.createDebugReportCallbackEXT(&debugReportCallbackCreateInfo, nullptr, &callback);
		//assert_amber(result != vk::Result::eSuccess, "CreateDebugReportCallbackEXT failed");
	}
}

void Window::createWindowAndSurface()
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	Log::info("Creating window.");
	m_sdlWindow = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_width, m_height, SDL_WINDOW_RESIZABLE);
	if (!m_sdlWindow)
		criticalError(SDL_GetError());

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);

	if (!SDL_GetWindowWMInfo(m_sdlWindow, &info))
		criticalError(stringFormat("Couldn't get window information: %s.\n Exiting...", SDL_GetError()));

	const char* subsystem = "an unknown system!";
	switch (info.subsystem)
	{
	case SDL_SYSWM_WINDOWS:
		subsystem = "Microsoft Windows";
		break;
	case SDL_SYSWM_UNKNOWN:
		break;
	case SDL_SYSWM_X11:
		subsystem = "X Window System";
		break;
	case SDL_SYSWM_WINRT:
		subsystem = "WinRT";
		break;
	case SDL_SYSWM_DIRECTFB:
		subsystem = "DirectFB";
		break;
	case SDL_SYSWM_COCOA:
		subsystem = "Apple OS X";
		break;
	case SDL_SYSWM_UIKIT:
		subsystem = "UIKit";
		break;
	case SDL_SYSWM_WAYLAND:
		subsystem = "Wayland";
		break;
	case SDL_SYSWM_MIR:
		subsystem = "Mir";
		break;
	case SDL_SYSWM_ANDROID:
		subsystem = "Android";
		break;
	case SDL_SYSWM_VIVANTE:
		subsystem = "Vivante";
		break;
	}

	Log::info(stringFormat("System: %s", subsystem));
	Log::info(stringFormat("SDL Version: %d.%d.%d", info.version.major, info.version.minor, info.version.patch));

	// TODO: support other systems
	if (info.subsystem != SDL_SYSWM_WINDOWS)
		criticalError("Unsupported system. Exititng...");

	setIcon("Textures/amber_icon.png");


	HWND hWnd = info.info.win.window;
	HINSTANCE hInstance = GetModuleHandle(nullptr);


	// Create a WSI surface for the window:
#if defined(VK_USE_PLATFORM_WIN32_KHR)
	{
		auto const createInfo = vk::Win32SurfaceCreateInfoKHR()
			.setHinstance(hInstance)
			.setHwnd(hWnd);

		auto result = instance.createWin32SurfaceKHR(&createInfo, nullptr, &surface);
		assert_amber(result == vk::Result::eSuccess, "Create Win32Surface failed");
	}
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR) && !defined(VK_USE_PLATFORM_XCB_KHR)
	{
		auto const createInfo = vk::WaylandSurfaceCreateInfoKHR()
			.setDisplay(display)
			.setSurface(window);

		auto result = inst.createWaylandSurfaceKHR(&createInfo, nullptr, &surface);
		assert_amber(result == vk::Result::eSuccess, "Create WaylandSurface failed");
	}
#endif
#if defined(VK_USE_PLATFORM_XLIB_KHR) || defined(VK_USE_PLATFORM_XCB_KHR)
	if (use_xlib)
	{
#if defined(VK_USE_PLATFORM_XLIB_KHR)
		auto const createInfo = vk::XlibSurfaceCreateInfoKHR()
			.setDpy(display)
			.setWindow(xlibWindow);

		auto result = instance.createXlibSurfaceKHR(&createInfo, nullptr, &surface);
		assert_amber(result == vk::Result::eSuccess, "Create XlibSurface failed");
#endif
	}
	else
	{
#if defined(VK_USE_PLATFORM_XCB_KHR)
		auto const createInfo = vk::XcbSurfaceCreateInfoKHR()
			.setConnection(connection)
			.setWindow(xcbWindow);

		auto result = instance.createXcbSurfaceKHR(&createInfo, nullptr, &surface);
		assert_amber(result == vk::Result::eSuccess, "Create XcbSurface failed");
#endif
	}
#endif
}

void Window::pickPhysicalDevice()
{
	uint32 gpuCount;
	auto result = instance.enumeratePhysicalDevices(&gpuCount, nullptr);
	assert_amber(result == vk::Result::eSuccess, "Enumerate GPU failed");
	assert_amber(gpuCount > 0, "No GPU found");

	if (gpuCount > 0)
	{
		Log::info(stringFormat("Found %d GPU(s) supporting Vulkan", gpuCount));

		std::unique_ptr<vk::PhysicalDevice[]> physicalDevices(new vk::PhysicalDevice[gpuCount]);
		result = instance.enumeratePhysicalDevices(&gpuCount, physicalDevices.get());
		assert_amber(result == vk::Result::eSuccess, "Enumerate GPU failed");

		// TODO: Select the best one
		gpu = physicalDevices[0];

		gpu.getProperties(&gpuProperties);

		auto deviceVendorIDToString = [](uint32 id)
		{
			switch (id)
			{
			case 0x1002:
				return "AMD";
			case 0x1010:
				return "ImgTec";
			case 0x10DE:
				return "NVIDIA";
			case 0x13B5:
				return "ARM";
			case 0x5143:
				return "Qualcomm";
			case 0x8086:
				return "INTEL";
			}
			return "Unknown vendor";
		};

		Log::info(stringFormat("Selected device: %s %s", deviceVendorIDToString(gpuProperties.vendorID), gpuProperties.deviceName));
	}
	else
	{
		criticalError("vkEnumeratePhysicalDevices reported zero accessible devices.\nvkEnumeratePhysicalDevices Failure");
	}

	// Look for device extensions
	uint32 deviceExtensionCount = 0;
	bool swapchainExtFound = false;
	enabledExtensionCount = 0;
	extensionNames = {};

	result = gpu.enumerateDeviceExtensionProperties(nullptr, &deviceExtensionCount, nullptr);
	assert_amber(result == vk::Result::eSuccess, "Enumerate GPU extension properties failed");

	if (deviceExtensionCount > 0)
	{
		std::unique_ptr<vk::ExtensionProperties[]> deviceExtensions(new vk::ExtensionProperties[deviceExtensionCount]);
		result = gpu.enumerateDeviceExtensionProperties(nullptr, &deviceExtensionCount, deviceExtensions.get());
		assert_amber(result == vk::Result::eSuccess, "Get GPU extension properties failed");

		for (uint32_t i = 0; i < deviceExtensionCount; i++)
		{
			if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, deviceExtensions[i].extensionName))
			{
				swapchainExtFound = true;
				extensionNames[enabledExtensionCount++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
			}
			
			assert_amber(enabledExtensionCount < 64, "Too many vulkan extensions");
		}
	}

	if (!swapchainExtFound)
	{
		criticalError("vkEnumerateDeviceExtensionProperties failed to find the " VK_KHR_SWAPCHAIN_EXTENSION_NAME " extension.\nvkCreateInstance Failure");
	}

	gpu.getQueueFamilyProperties(&queueFamilyCount, nullptr);
	assert_amber(queueFamilyCount >= 1, "vk::Device::getQueueFamilyProperties returned no properties");

	queueFamilyProperties.reset(new vk::QueueFamilyProperties[queueFamilyCount]);
	gpu.getQueueFamilyProperties(&queueFamilyCount, queueFamilyProperties.get());

	// Iterate over each queue to learn whether it supports presenting:
	std::unique_ptr<vk::Bool32[]> supportsPresent(new vk::Bool32[queueFamilyCount]);
	for (uint32 i = 0; i < queueFamilyCount; i++)
	{
		gpu.getSurfaceSupportKHR(i, surface, &supportsPresent[i]);

	}
	for (uint32 i = 0; i < queueFamilyCount; i++)
	{
		if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
		{
			if (graphicsQueueFamilyIndex == std::numeric_limits<uint32>::max())
			{
				graphicsQueueFamilyIndex = i;
			}

			if (supportsPresent[i] == VK_TRUE)
			{
				graphicsQueueFamilyIndex = i;
				presentQueueFamilyIndex = i;
				break;
			}
		}
	}

	if (presentQueueFamilyIndex == std::numeric_limits<uint32>::max())
	{
		// If didn't find a queue that supports both graphics and present, then find a separate present queue.
		for (uint32_t i = 0; i < queueFamilyCount; ++i)
		{
			if (supportsPresent[i] == VK_TRUE)
			{
				presentQueueFamilyIndex = i;
				break;
			}
		}
	}

	// Generate error if could not find both a graphics and a present queue
	if (graphicsQueueFamilyIndex == std::numeric_limits<uint32>::max() || presentQueueFamilyIndex == std::numeric_limits<uint32>::max())
	{
		criticalError("Could not find both graphics and present queues\nSwapchain Initialization Failure");
	}

	graphicsQueueFamilyIndex = graphicsQueueFamilyIndex;
	presentQueueFamilyIndex = presentQueueFamilyIndex;
	separatePresentQueue = (graphicsQueueFamilyIndex != presentQueueFamilyIndex);

	// Query fine-grained feature support for this device
	vk::PhysicalDeviceFeatures physDevFeatures;
	gpu.getFeatures(&physDevFeatures);
}

void Window::createLogicalDevice()
{
	float const priorities[1] = { 0.0 };

	vk::DeviceQueueCreateInfo queues[2];
	queues[0].setQueueFamilyIndex(graphicsQueueFamilyIndex);
	queues[0].setQueueCount(1);
	queues[0].setPQueuePriorities(priorities);

	auto deviceInfo = vk::DeviceCreateInfo()
		.setQueueCreateInfoCount(1)
		.setPQueueCreateInfos(queues)
		.setEnabledLayerCount(0)
		.setPpEnabledLayerNames(nullptr)
		.setEnabledExtensionCount(enabledExtensionCount)
		.setPpEnabledExtensionNames((const char *const *)extensionNames.data())
		.setPEnabledFeatures(nullptr);

	if (separatePresentQueue)
	{
		queues[1].setQueueFamilyIndex(presentQueueFamilyIndex);
		queues[1].setQueueCount(1);
		queues[1].setPQueuePriorities(priorities);
		deviceInfo.setQueueCreateInfoCount(2);
	}

	auto result = gpu.createDevice(&deviceInfo, nullptr, &device);
	assert_amber(result == vk::Result::eSuccess, "Create device failed");

	device.getQueue(graphicsQueueFamilyIndex, 0, &graphicsQueue);
	if (!separatePresentQueue)
	{
		presentQueue = graphicsQueue;
	}
	else
	{
		device.getQueue(presentQueueFamilyIndex, 0, &presentQueue);
	}

	// Get the list of VkFormat's that are supported:
	uint32 formatCount;
	result = gpu.getSurfaceFormatsKHR(surface, &formatCount, nullptr);
	assert_amber(result == vk::Result::eSuccess, "Get surface formats failed");

	std::unique_ptr<vk::SurfaceFormatKHR[]> surfaceFormats(new vk::SurfaceFormatKHR[formatCount]);
	result = gpu.getSurfaceFormatsKHR(surface, &formatCount, surfaceFormats.get());
	assert_amber(result == vk::Result::eSuccess, "Enumerate surface formats failed");

	// If the format list includes just one entry of VK_FORMAT_UNDEFINED, the surface has no preferred swapchainImageFormat. Otherwise, at least one supported swapchainImageFormat will be returned.
	if (formatCount == 1 && surfaceFormats[0].format == vk::Format::eUndefined)
	{
		swapchainImageFormat = vk::Format::eB8G8R8A8Unorm;
	}
	else
	{
		assert_amber(formatCount >= 1, "Unexpected prefered format");
		swapchainImageFormat = surfaceFormats[0].format;
	}
	colorSpace = surfaceFormats[0].colorSpace;

	// Get Memory information and properties
	gpu.getMemoryProperties(&memoryProperties);
}

void Window::createSwapchain()
{
	vk::SwapchainKHR oldSwapchain = swapchain;

	// Check the surface capabilities and formats
	vk::SurfaceCapabilitiesKHR surfCapabilities;
	auto result = gpu.getSurfaceCapabilitiesKHR(surface, &surfCapabilities);
	assert_amber(result == vk::Result::eSuccess, "Get surface capabilities failed");

	uint32 presentModeCount;
	result = gpu.getSurfacePresentModesKHR(surface, &presentModeCount, nullptr);
	assert_amber(result == vk::Result::eSuccess, "Get surface present modes failed");

	std::unique_ptr<vk::PresentModeKHR[]> presentModes(new vk::PresentModeKHR[presentModeCount]);
	result = gpu.getSurfacePresentModesKHR(surface, &presentModeCount, presentModes.get());
	assert_amber(result == vk::Result::eSuccess, "Enumerate surface present modes failed");

	if (surfCapabilities.currentExtent.width == std::numeric_limits<uint32_t>::max())
	{
		// If the surface size is undefined, the size is set to the size of the images requested.
		swapchainExtent.width = m_width;
		swapchainExtent.height = m_height;
	}
	else
	{
		// If the surface size is defined, the swap chain size must match
		swapchainExtent = surfCapabilities.currentExtent;
		m_width = surfCapabilities.currentExtent.width;
		m_height = surfCapabilities.currentExtent.height;
	}

	// TODO: add support for other modes (tripple buffering)
	// The FIFO present mode is guaranteed by the spec to be supported
	vk::PresentModeKHR swapchainPresentMode = vk::PresentModeKHR::eFifo;

	// Determine the number of VkImage's to use in the swap chain (only 1 image at a time, besides the images being displayed and queued for display)
	uint32 desiredNumberOfSwapchainImages = surfCapabilities.minImageCount + 1;
	// If maxImageCount is 0, we can ask for as many images as we want, otherwise we're limited to maxImageCount
	if ((surfCapabilities.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCapabilities.maxImageCount))
	{
		// Settle for fewer images than desired
		desiredNumberOfSwapchainImages = surfCapabilities.maxImageCount;
	}

	vk::SurfaceTransformFlagBitsKHR preTransform;
	if (surfCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity)
	{
		preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
	}
	else
	{
		preTransform = surfCapabilities.currentTransform;
	}

	// TODO: read about sharing mode
	auto const swapchainCreateInfo =
		vk::SwapchainCreateInfoKHR()
		.setSurface(surface)
		.setMinImageCount(desiredNumberOfSwapchainImages)
		.setImageFormat(swapchainImageFormat)
		.setImageColorSpace(colorSpace)
		.setImageExtent({ swapchainExtent.width, swapchainExtent.height })
		.setImageArrayLayers(1)
		.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
		.setImageSharingMode(vk::SharingMode::eExclusive)
		.setQueueFamilyIndexCount(0)
		.setPQueueFamilyIndices(nullptr)
		.setPreTransform(preTransform)
		.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
		.setPresentMode(swapchainPresentMode)
		.setClipped(true)
		.setOldSwapchain(oldSwapchain);

	result = device.createSwapchainKHR(&swapchainCreateInfo, nullptr, &swapchain);
	assert_amber(result == vk::Result::eSuccess, "Create swapchain failed");

	// If we just re-created an existing swapchain destroy the old swapchain
	// Note: destroying the swapchain also cleans up all its associated presentable images once the platform is done with them.
	if (oldSwapchain)
	{
		device.destroySwapchainKHR(oldSwapchain, nullptr);
	}
}

void Window::createImageViews()
{
	auto result = device.getSwapchainImagesKHR(swapchain, &swapchainImageCount, nullptr);
	assert_amber(result == vk::Result::eSuccess, "Get swapchain images failed");

	std::unique_ptr<vk::Image[]> swapchainImages(new vk::Image[swapchainImageCount]);
	result = device.getSwapchainImagesKHR(swapchain, &swapchainImageCount, swapchainImages.get());
	assert_amber(result == vk::Result::eSuccess, "Enumerate swapchain images failed");

	swapchainBuffers.resize(swapchainImageCount);

	for (uint32_t i = 0; i < swapchainImageCount; ++i)
	{
		createImageView(swapchainImages[i], swapchainImageFormat, vk::ImageAspectFlagBits::eColor, swapchainBuffers[i].imageView);

		//auto const colorImageViewCreateInfo = vk::ImageViewCreateInfo()
		//	.setImage(swapchainImages[i])
		//	.setViewType(vk::ImageViewType::e2D)
		//	.setFormat(swapchainImageFormat)
		//	.setComponents(vk::ComponentMapping())
		//	.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

		//swapchainBuffers[i].image = swapchainImages[i];

		//result = device.createImageView(&colorImageViewCreateInfo, nullptr, &swapchainBuffers[i].imageView);
		//assert_amber(result == vk::Result::eSuccess, "Create image view failed");
	}
}

void Window::createRenderPass()
{
	vk::AttachmentDescription colorAttachmentDescription = vk::AttachmentDescription()
		.setFormat(swapchainImageFormat)
		.setSamples(vk::SampleCountFlagBits::e1)
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eStore)
		.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStencilStoreOp(vk::AttachmentStoreOp::eStore)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

	vk::AttachmentDescription depthAttachmentDescription = vk::AttachmentDescription()
		.setFormat(findDepthFormat())
		.setSamples(vk::SampleCountFlagBits::e1)
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setInitialLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
		.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::AttachmentReference colorAttachmentReference = vk::AttachmentReference()
		.setAttachment(0)
		.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

	vk::AttachmentReference depthAttachmentReference = vk::AttachmentReference()
		.setAttachment(1)
		.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

	// Does have more settings
	vk::SubpassDescription subpassDescription = vk::SubpassDescription()
		.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
		.setColorAttachmentCount(1)
		.setPColorAttachments(&colorAttachmentReference)
		.setPDepthStencilAttachment(&depthAttachmentReference);

	vk::SubpassDependency subpassDependency = vk::SubpassDependency()
		.setSrcSubpass(VK_SUBPASS_EXTERNAL)
		.setDstSubpass(0)
		.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
		.setSrcAccessMask(vk::AccessFlags())
		.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
		.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);

	std::array<vk::AttachmentDescription, 2> attachments = { colorAttachmentDescription, depthAttachmentDescription };
	vk::RenderPassCreateInfo renderPassCreateInfo = vk::RenderPassCreateInfo()
		.setAttachmentCount(static_cast<uint32>(attachments.size()))
		.setPAttachments(attachments.data())
		.setSubpassCount(1)
		.setPSubpasses(&subpassDescription)
		.setDependencyCount(1)
		.setPDependencies(&subpassDependency);

	auto result = device.createRenderPass(&renderPassCreateInfo, nullptr, &renderPass);
	assert_amber(result == vk::Result::eSuccess, "Create render pass failed");
}

void Window::createDescriptorSetLayout()
{
	vk::DescriptorSetLayoutBinding uboLayoutBinding = vk::DescriptorSetLayoutBinding()
		.setBinding(0)
		.setDescriptorCount(1)
		.setDescriptorType(vk::DescriptorType::eUniformBuffer)
		.setStageFlags(vk::ShaderStageFlagBits::eVertex)
		.setPImmutableSamplers(nullptr);

	vk::DescriptorSetLayoutBinding samplerLayoutBinding = vk::DescriptorSetLayoutBinding()
		.setBinding(1)
		.setDescriptorCount(1)
		.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
		.setStageFlags(vk::ShaderStageFlagBits::eFragment)
		.setPImmutableSamplers(nullptr);

	std::array<vk::DescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

	vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo()
		.setBindingCount(static_cast<uint32>(bindings.size()))
		.setPBindings(bindings.data());

	auto result = device.createDescriptorSetLayout(&descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);
	assert_amber(result == vk::Result::eSuccess, "Create Descriptor set layout failed");
}

void Window::createGraphicsPipeline()
{
	// Shaders
	// --------
	auto vertShaderCode = readFile("Shaders/vk_triangle.vert.spv");
	auto fragShaderCode = readFile("Shaders/vk_triangle.frag.spv");

	auto createShaderModule = [&](const std::vector<char>& code)
	{
		//The buffer with the code can be freed immediately after creating the shader module.
		vk::ShaderModuleCreateInfo shaderModuleCreateInfo = vk::ShaderModuleCreateInfo()
			.setCodeSize(code.size())
			.setPCode((uint32*)code.data());

		vk::ShaderModule module;
		auto result = device.createShaderModule(&shaderModuleCreateInfo, nullptr, &module);
		assert_amber(result == vk::Result::eSuccess, "Create shader module failed");

		return module;
	};

	vk::ShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	vk::ShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	vk::PipelineShaderStageCreateInfo const pipelineShaderStageCreateInfo[2] =
	{
		vk::PipelineShaderStageCreateInfo()
		.setStage(vk::ShaderStageFlagBits::eVertex)
		.setModule(vertShaderModule)
		.setPName("main"),
		vk::PipelineShaderStageCreateInfo()
		.setStage(vk::ShaderStageFlagBits::eFragment)
		.setModule(fragShaderModule)
		.setPName("main")
	};

	// Pipeline
	// ---------
	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = vk::PipelineVertexInputStateCreateInfo()
		.setVertexBindingDescriptionCount(1)
		.setPVertexBindingDescriptions(&bindingDescription)
		.setVertexAttributeDescriptionCount(static_cast<uint32>(attributeDescriptions.size()))
		.setPVertexAttributeDescriptions(attributeDescriptions.data());

	vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = vk::PipelineInputAssemblyStateCreateInfo()
		.setTopology(vk::PrimitiveTopology::eTriangleList)
		.setPrimitiveRestartEnable(VK_FALSE);

	vk::Viewport viewport = vk::Viewport()
		.setX(0.0f)
		.setY(0.0f)
		.setWidth(static_cast<float>(swapchainExtent.width))
		.setHeight(static_cast<float>(swapchainExtent.height))
		.setMinDepth(0.0f)
		.setMaxDepth(1.0f);

	vk::Rect2D scissor = vk::Rect2D({ 0, 0 }, swapchainExtent);

	vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = vk::PipelineViewportStateCreateInfo()
		.setViewportCount(1)
		.setPViewports(&viewport)
		.setScissorCount(1)
		.setPScissors(&scissor);

	vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = vk::PipelineRasterizationStateCreateInfo()
		.setDepthClampEnable(VK_FALSE)
		.setRasterizerDiscardEnable(VK_FALSE)
		.setPolygonMode(vk::PolygonMode::eFill)
		.setLineWidth(1.0f)
		.setCullMode(vk::CullModeFlagBits::eBack)
		.setFrontFace(vk::FrontFace::eCounterClockwise)
		.setDepthBiasEnable(VK_FALSE)
		.setDepthBiasConstantFactor(0.0f)
		.setDepthBiasClamp(0.0f)
		.setDepthBiasSlopeFactor(0.0f);

	vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = vk::PipelineMultisampleStateCreateInfo()
		.setSampleShadingEnable(VK_FALSE)
		.setRasterizationSamples(vk::SampleCountFlagBits::e1)
		.setMinSampleShading(1.0f)
		.setPSampleMask(nullptr)
		.setAlphaToCoverageEnable(VK_FALSE)
		.setAlphaToOneEnable(VK_FALSE);

	vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = vk::PipelineDepthStencilStateCreateInfo()
		.setDepthTestEnable(VK_TRUE)
		.setDepthWriteEnable(VK_TRUE)
		.setDepthCompareOp(vk::CompareOp::eLess)
		.setDepthBoundsTestEnable(VK_FALSE)
		.setStencilTestEnable(VK_FALSE);

	vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = vk::PipelineColorBlendAttachmentState()
		.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
		.setBlendEnable(VK_FALSE)
		.setSrcColorBlendFactor(vk::BlendFactor::eOne)
		.setDstColorBlendFactor(vk::BlendFactor::eZero)
		.setColorBlendOp(vk::BlendOp::eAdd)
		.setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
		.setDstAlphaBlendFactor(vk::BlendFactor::eZero)
		.setAlphaBlendOp(vk::BlendOp::eAdd);

	vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = vk::PipelineColorBlendStateCreateInfo()
		.setLogicOpEnable(VK_FALSE)
		.setLogicOp(vk::LogicOp::eCopy)
		.setAttachmentCount(1)
		.setPAttachments(&pipelineColorBlendAttachmentState)
		.setBlendConstants({ 0.0f, 0.0f, 0.0f, 0.0f });

	//vk::DynamicState dynamicStates[] =
	//{
	//	vk::DynamicState::eViewport,
	//	vk::DynamicState::eLineWidth
	//};

	//vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = vk::PipelineDynamicStateCreateInfo()
	//	.setDynamicStateCount(2)
	//	.setPDynamicStates(dynamicStates);

	vk::DescriptorSetLayout setLayouts[] = { descriptorSetLayout };
	vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()
		.setSetLayoutCount(1)
		.setPSetLayouts(setLayouts);
		//.setPushConstantRangeCount(0)
		//.setPPushConstantRanges(nullptr);

	auto result = device.createPipelineLayout(&pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
	assert_amber(result == vk::Result::eSuccess, "Create pipeline layout failed");

	vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo = vk::GraphicsPipelineCreateInfo()
		.setStageCount(2)
		.setPStages(pipelineShaderStageCreateInfo)
		.setPVertexInputState(&pipelineVertexInputStateCreateInfo)
		.setPInputAssemblyState(&pipelineInputAssemblyStateCreateInfo)
		.setPViewportState(&pipelineViewportStateCreateInfo)
		.setPRasterizationState(&pipelineRasterizationStateCreateInfo)
		.setPMultisampleState(&pipelineMultisampleStateCreateInfo)
		.setPDepthStencilState(&pipelineDepthStencilStateCreateInfo)
		.setPColorBlendState(&pipelineColorBlendStateCreateInfo)
		.setPDynamicState(nullptr)
		.setLayout(pipelineLayout)
		.setRenderPass(renderPass)
		.setSubpass(0)
		.setBasePipelineHandle(VK_NULL_HANDLE)
		.setBasePipelineIndex(-1);

	result = device.createGraphicsPipelines(VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline);
	assert_amber(result == vk::Result::eSuccess, "Create graphics pipelines failed");
}

void Window::createFramebuffers()
{
	swapchainFramebuffers.resize(swapchainBuffers.size(), vk::Framebuffer());

	for (size_t i = 0; i < swapchainBuffers.size(); i++)
	{
		std::array<vk::ImageView, 2> attachmentImageViews =
		{
			swapchainBuffers[i].imageView,
			depthImageView
		};

		vk::FramebufferCreateInfo framebufferCreateInfo = vk::FramebufferCreateInfo()
			.setRenderPass(renderPass)
			.setAttachmentCount(static_cast<uint32>(attachmentImageViews.size()))
			.setPAttachments(attachmentImageViews.data())
			.setWidth(swapchainExtent.width)
			.setHeight(swapchainExtent.height)
			.setLayers(1);

		auto result = device.createFramebuffer(&framebufferCreateInfo, nullptr, &swapchainFramebuffers[i]);
		assert_amber(result == vk::Result::eSuccess, "Create framebuffer failed");
	}
}

void Window::createCommandPool()
{
	vk::CommandPoolCreateInfo commandPoolCreateInfo = vk::CommandPoolCreateInfo()
		.setQueueFamilyIndex(graphicsQueueFamilyIndex)
		.setFlags(vk::CommandPoolCreateFlags());

	auto result = device.createCommandPool(&commandPoolCreateInfo, nullptr, &commandPool);
	assert_amber(result == vk::Result::eSuccess, "Create command pool failed");
}

void Window::createDepthResources()
{
	vk::Format depthFormat = findDepthFormat();

	createImage(swapchainExtent.width, swapchainExtent.height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, depthImage, depthImageMemory);
	createImageView(depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth, depthImageView);

	transitionImageLayout(depthImage, depthFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
}

void Window::createTextureImage()
{
	int texWidth, texHeight, texChannels;

	stbi_uc* pixels = stbi_load("textures/chalet.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels)
	{
		criticalError("failed to load texture image!");
	}

	vk::Image stagingImage;
	vk::DeviceMemory stagingImageMemory;
	createImage(texWidth, texHeight, vk::Format::eR8G8B8A8Unorm, vk::ImageTiling::eLinear, vk::ImageUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingImage, stagingImageMemory);

	void* data = device.mapMemory(stagingImageMemory, 0, imageSize);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	device.unmapMemory(stagingImageMemory);

	stbi_image_free(pixels);

	createImage(texWidth, texHeight, vk::Format::eR8G8B8A8Unorm, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal, textureImage, textureImageMemory);

	transitionImageLayout(stagingImage, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::ePreinitialized, vk::ImageLayout::eTransferSrcOptimal);
	transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::ePreinitialized, vk::ImageLayout::eTransferDstOptimal);
	copyImage(stagingImage, textureImage, texWidth, texHeight);

	transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
}

void Window::createTextureImageView()
{
	createImageView(textureImage, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor, textureImageView);
}

void Window::createTextureSampler()
{
	vk::SamplerCreateInfo samplerInfo = vk::SamplerCreateInfo()
		.setMagFilter(vk::Filter::eLinear)
		.setMinFilter(vk::Filter::eLinear)
		.setAddressModeU(vk::SamplerAddressMode::eRepeat)
		.setAddressModeV(vk::SamplerAddressMode::eRepeat)
		.setAddressModeW(vk::SamplerAddressMode::eRepeat)
		.setAnisotropyEnable(VK_TRUE)
		.setMaxAnisotropy(16.f)
		.setBorderColor(vk::BorderColor::eIntOpaqueBlack)
		.setUnnormalizedCoordinates(VK_FALSE)
		.setCompareEnable(VK_FALSE)
		.setCompareOp(vk::CompareOp::eAlways)
		.setMipmapMode(vk::SamplerMipmapMode::eLinear)
		.setMipLodBias(0.f)
		.setMinLod(0.f)
		.setMaxLod(0.f);

	auto result = device.createSampler(&samplerInfo, nullptr, &textureSampler);
	assert_amber(result == vk::Result::eSuccess, "Create sampler failed");
}

void Window::loadModel()
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, "models/chalet.obj"))
	{
		criticalError("failed to load model. " + err);
	}

	std::unordered_map<Vertex, int> uniqueVertices;

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			Vertex vertex = {};

			vertex.pos =
			{
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord =
			{
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}
}

void Window::createVertexBuffer()
{
	vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	vk::Buffer stagingBuffer = vk::Buffer();
	vk::DeviceMemory stagingBufferMemory = vk::DeviceMemory();
	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

	void* data = device.mapMemory(stagingBufferMemory, 0, bufferSize, vk::MemoryMapFlags());
	memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
	device.unmapMemory(stagingBufferMemory);
	// Since cache coherent memory was requested, there is no need to flush memory (memory ranges)

	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, vertexBufferMemory);
	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	device.destroyBuffer(stagingBuffer, nullptr);
	device.freeMemory(stagingBufferMemory);
}

void Window::createIndexBuffer()
{
	vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	vk::Buffer stagingBuffer = vk::Buffer();
	vk::DeviceMemory stagingBufferMemory = vk::DeviceMemory();
	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

	void* data = device.mapMemory(stagingBufferMemory, 0, bufferSize, vk::MemoryMapFlags());
	memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
	device.unmapMemory(stagingBufferMemory);

	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indexBufferMemory);

	copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	device.destroyBuffer(stagingBuffer, nullptr);
	device.freeMemory(stagingBufferMemory);
}

void Window::createUniformBuffer()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, uniformStagingBuffer, uniformStagingBufferMemory);
	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, uniformBuffer, uniformBufferMemory);
}

void Window::createDescriptorPool()
{
	std::array<vk::DescriptorPoolSize, 2> descriptorPoolSizes = 
	{
		vk::DescriptorPoolSize()
			.setType(vk::DescriptorType::eUniformBuffer)
			.setDescriptorCount(1),
		vk::DescriptorPoolSize()
			.setType(vk::DescriptorType::eCombinedImageSampler)
			.setDescriptorCount(1)
	};

	vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo = vk::DescriptorPoolCreateInfo()
		.setPoolSizeCount(static_cast<uint32>(descriptorPoolSizes.size()))
		.setPPoolSizes(descriptorPoolSizes.data())
		.setMaxSets(1);

	auto result = device.createDescriptorPool(&descriptorPoolCreateInfo, nullptr, &descriptorPool);
	assert_amber(result == vk::Result::eSuccess, "Create descriptor pool pool failed");
}

void Window::createDescriptorSet()
{
	vk::DescriptorSetLayout layouts[] = { descriptorSetLayout };
	vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo = vk::DescriptorSetAllocateInfo()
		.setDescriptorPool(descriptorPool)
		.setDescriptorSetCount(1)
		.setPSetLayouts(layouts);

	auto result = device.allocateDescriptorSets(&descriptorSetAllocateInfo, &descriptorSet);
	assert_amber(result == vk::Result::eSuccess, "Allocate descriptor set failed");

	vk::DescriptorBufferInfo descriptorBufferInfo = vk::DescriptorBufferInfo()
		.setBuffer(uniformBuffer)
		.setOffset(0)
		.setRange(sizeof(UniformBufferObject));

	vk::DescriptorImageInfo descriptorImageInfo = vk::DescriptorImageInfo()
		.setImageView(textureImageView)
		.setSampler(textureSampler);

	std::array<vk::WriteDescriptorSet, 2> descriptorWrites =
	{
		vk::WriteDescriptorSet()
			.setDstSet(descriptorSet)
			.setDstBinding(0)
			.setDstArrayElement(0)
			.setDescriptorType(vk::DescriptorType::eUniformBuffer)
			.setDescriptorCount(1)
			.setPBufferInfo(&descriptorBufferInfo),
		vk::WriteDescriptorSet()
			.setDstSet(descriptorSet)
			.setDstBinding(1)
			.setDstArrayElement(0)
			.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
			.setDescriptorCount(1)
			.setPImageInfo(&descriptorImageInfo)
	};

	device.updateDescriptorSets(static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void Window::createCommandBuffers()
{
	// TODO: add free

	vk::CommandBufferAllocateInfo commandBufferAllocateInfo = vk::CommandBufferAllocateInfo()
		.setCommandPool(commandPool)
		.setLevel(vk::CommandBufferLevel::ePrimary)
		.setCommandBufferCount(1);

	for (uint32 i = 0; i < swapchainImageCount; i++)
	{
		auto result = device.allocateCommandBuffers(&commandBufferAllocateInfo, &swapchainBuffers[i].commandBuffer);
		assert_amber(result == vk::Result::eSuccess, "Allocate command buffers failed");
	}

	if (separatePresentQueue)
	{
		vk::CommandPoolCreateInfo presentCommandPoolCreateInfo = vk::CommandPoolCreateInfo()
			.setQueueFamilyIndex(presentQueueFamilyIndex)
			.setFlags(vk::CommandPoolCreateFlags());

		vk::CommandPool presentCommandPool;
		auto result = device.createCommandPool(&presentCommandPoolCreateInfo, nullptr, &presentCommandPool);
		assert_amber(result == vk::Result::eSuccess, "Create command pool failed");

		vk::CommandBufferAllocateInfo graphicsToPresentCommandBufferAllocateInfo = vk::CommandBufferAllocateInfo()
			.setCommandPool(presentCommandPool)
			.setLevel(vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount(1);

		for (uint32 i = 0; i < swapchainImageCount; i++)
		{
			result = device.allocateCommandBuffers(&graphicsToPresentCommandBufferAllocateInfo, &swapchainBuffers[i].graphicsToPresentCommandBuffer);
			assert_amber(result == vk::Result::eSuccess, "Allocate command buffers failed");
		}
	}

	for (size_t i = 0; i < swapchainBuffers.size(); i++)
	{
		vk::CommandBufferBeginInfo commandBufferBeginInfo = vk::CommandBufferBeginInfo()
			.setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse)
			.setPInheritanceInfo(nullptr);

		swapchainBuffers[i].commandBuffer.begin(&commandBufferBeginInfo);

		std::array<vk::ClearValue,2> clearColors = 
		{
			vk::ClearColorValue(std::array<float, 4>{ 0.2f, 0.2f, 0.2f, 1.0f }),
			vk::ClearDepthStencilValue(1.f, 0)
		};

		vk::RenderPassBeginInfo renderPassBeginInfo = vk::RenderPassBeginInfo()
			.setRenderPass(renderPass)
			.setFramebuffer(swapchainFramebuffers[i])
			.setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(static_cast<uint32>(m_width), static_cast<uint32>(m_height))))
			.setClearValueCount(static_cast<uint32>(clearColors.size()))
			.setPClearValues(clearColors.data());

		swapchainBuffers[i].commandBuffer.beginRenderPass(&renderPassBeginInfo, vk::SubpassContents::eInline);

		swapchainBuffers[i].commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

		vk::Buffer vertexBuffers[] = { vertexBuffer };
		vk::DeviceSize offsets[] = { 0 };

		swapchainBuffers[i].commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
		swapchainBuffers[i].commandBuffer.bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint32);
		swapchainBuffers[i].commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

		swapchainBuffers[i].commandBuffer.drawIndexed(static_cast<uint32>(indices.size()), 1, 0, 0, 0);

		swapchainBuffers[i].commandBuffer.endRenderPass();

		// TODO: should return VkResult but returns void?
		swapchainBuffers[i].commandBuffer.end();
	}
}

void Window::createSemaphores()
{
	vk::SemaphoreCreateInfo semaphoreCreateInfo = vk::SemaphoreCreateInfo();

	auto result = device.createSemaphore(&semaphoreCreateInfo, nullptr, &imageAvailableSemaphore);
	assert_amber(result == vk::Result::eSuccess, "Semaphore creation failed");
	result = device.createSemaphore(&semaphoreCreateInfo, nullptr, &renderFinishedSemaphore);
	assert_amber(result == vk::Result::eSuccess, "Semaphore creation failed");
}

void Window::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory)
{
	vk::BufferCreateInfo bufferCreateInfo = vk::BufferCreateInfo()
		.setSize(size)
		.setUsage(usage)
		.setSharingMode(vk::SharingMode::eExclusive);

	auto result = device.createBuffer(&bufferCreateInfo, nullptr, &buffer);
	assert_amber(result == vk::Result::eSuccess, "Create buffer failed");

	vk::MemoryRequirements memoryRequirements;
	device.getBufferMemoryRequirements(buffer, &memoryRequirements);

	vk::MemoryAllocateInfo memoryAllocateInfo = vk::MemoryAllocateInfo()
		.setAllocationSize(memoryRequirements.size)
		.setMemoryTypeIndex(findMemoryType(memoryRequirements.memoryTypeBits, properties));

	result = device.allocateMemory(&memoryAllocateInfo, nullptr, &bufferMemory);
	assert_amber(result == vk::Result::eSuccess, "Allocate memory failed");

	device.bindBufferMemory(buffer, bufferMemory, 0);
}

void Window::createImage(uint32 width, uint32 height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory)
{
	vk::ImageCreateInfo imageCreateInfo = vk::ImageCreateInfo()
		.setImageType(vk::ImageType::e2D)
		.setExtent(vk::Extent3D(width, height, 1))
		.setMipLevels(1)
		.setArrayLayers(1)
		.setFormat(format)
		.setTiling(tiling)
		.setInitialLayout(vk::ImageLayout::ePreinitialized)
		.setUsage(usage)
		.setSharingMode(vk::SharingMode::eExclusive)
		.setSamples(vk::SampleCountFlagBits::e1)
		.setFlags(vk::ImageCreateFlags());

	auto result = device.createImage(&imageCreateInfo, nullptr, &image);
	assert_amber(result == vk::Result::eSuccess, "Create image failed");

	vk::MemoryRequirements memoryRequirements;
	device.getImageMemoryRequirements(image, &memoryRequirements);

	vk::MemoryAllocateInfo memoryAllocateInfo = vk::MemoryAllocateInfo()
		.setAllocationSize(memoryRequirements.size)
		.setMemoryTypeIndex(findMemoryType(memoryRequirements.memoryTypeBits, properties));


	result = device.allocateMemory(&memoryAllocateInfo, nullptr, &imageMemory);
	assert_amber(result == vk::Result::eSuccess, "Allocate image memory failed");

	device.bindImageMemory(image, imageMemory, 0);
}

void Window::createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags, vk::ImageView& imageView)
{
	vk::ImageViewCreateInfo imageViewCreateInfo = vk::ImageViewCreateInfo()
		.setImage(image)
		.setViewType(vk::ImageViewType::e2D)
		.setFormat(format)
		.setSubresourceRange(vk::ImageSubresourceRange(aspectFlags, 0, 1, 0, 1));

	auto result = device.createImageView(&imageViewCreateInfo, nullptr, &imageView);
	assert_amber(result == vk::Result::eSuccess, "Create image view failed");
}

void Window::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
	// TODO: create separate command pool for this with VK_COMMAND_POOL_CREATE_TRANSIENT_BIT


	vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

	vk::BufferCopy copyRegion = vk::BufferCopy()
		.setSrcOffset(0)
		.setDstOffset(0)
		.setSize(size);

	commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}

void Window::copyImage(vk::Image srcImage, vk::Image dstImage, uint32 width, uint32 height)
{
	vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

	vk::ImageSubresourceLayers subResource = vk::ImageSubresourceLayers()
		.setAspectMask(vk::ImageAspectFlagBits::eColor)
		.setBaseArrayLayer(0)
		.setMipLevel(0)
		.setLayerCount(1);

	vk::ImageCopy region = vk::ImageCopy()
		.setSrcSubresource(subResource)
		.setDstSubresource(subResource)
		.setSrcOffset(vk::Offset3D())
		.setDstOffset(vk::Offset3D())
		.setExtent(vk::Extent3D(width, height, 1));

	commandBuffer.copyImage(srcImage, vk::ImageLayout::eTransferSrcOptimal, dstImage, vk::ImageLayout::eTransferDstOptimal, 1, &region);

	endSingleTimeCommands(commandBuffer);
}

vk::CommandBuffer Window::beginSingleTimeCommands()
{
	// TODO: create separate command pool for this with VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
	vk::CommandBufferAllocateInfo commandBufferAllocateInfo = vk::CommandBufferAllocateInfo()
		.setLevel(vk::CommandBufferLevel::ePrimary)
		.setCommandPool(commandPool)
		.setCommandBufferCount(1);

	vk::CommandBuffer commandBuffer;
	auto result = device.allocateCommandBuffers(&commandBufferAllocateInfo, &commandBuffer);
	assert_amber(result == vk::Result::eSuccess, "Allocate command buffer failed");

	vk::CommandBufferBeginInfo cmdBufferBeginInfo = vk::CommandBufferBeginInfo()
		.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);


	commandBuffer.begin(&cmdBufferBeginInfo);

	return commandBuffer;
}

void Window::endSingleTimeCommands(vk::CommandBuffer commandBuffer)
{
	commandBuffer.end();

	vk::SubmitInfo submitInfo = vk::SubmitInfo()
		.setCommandBufferCount(1)
		.setPCommandBuffers(&commandBuffer);

	graphicsQueue.submit(1, &submitInfo, VK_NULL_HANDLE);
	// Fence could be used here to allow multiple copies
	graphicsQueue.waitIdle();

	device.freeCommandBuffers(commandPool, 1, &commandBuffer);
}

void Window::transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
	vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

	vk::ImageMemoryBarrier barrier = vk::ImageMemoryBarrier()
		.setOldLayout(oldLayout)
		.setNewLayout(newLayout)
		.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
		.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
		.setImage(image);

	vk::ImageAspectFlags aspectMask;
	if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		aspectMask = vk::ImageAspectFlagBits::eDepth;

		if (hasStencilComponent(format))
		{
			aspectMask |= vk::ImageAspectFlagBits::eStencil;
		}
	}
	else
	{
		aspectMask = vk::ImageAspectFlagBits::eColor;
	}

	barrier.setSubresourceRange(vk::ImageSubresourceRange(aspectMask, 0, 1, 0, 1));

	// TODO: ALL_THE_TRANSITIONS.jpg
	if (oldLayout == vk::ImageLayout::ePreinitialized && newLayout == vk::ImageLayout::eTransferSrcOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
	}
	else if (oldLayout == vk::ImageLayout::ePreinitialized && newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
	}
	else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout ==  vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
	}
	else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlags();
		barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
	}
	else
	{
		criticalError("Unsupported image layout transition!");
	}

	commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTopOfPipe, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);

	endSingleTimeCommands(commandBuffer);
}

void Window::updateUniformBuffer()
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f;

	UniformBufferObject ubo = {};
	ubo.model = Math::rotate(Matrix4x4f::Identity, time * toRadians(90.0f) / 2, Vector3f(0.0f, 0.0f, 1.0f));
	ubo.view = Math::lookAt(Vector3f(2.0f, 2.0f, 2.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f));
	ubo.proj = Math::perspectiveFov(toRadians(45.0f), swapchainExtent.width / (float)swapchainExtent.height, 0.1f, 10.0f);

	// in Vulkan the Y coordinate of the clip coordinates is inverted
	ubo.proj[1][1] *= -1;

	void* data = device.mapMemory(uniformStagingBufferMemory, 0, sizeof(ubo), vk::MemoryMapFlags());
	memcpy(data, &ubo, sizeof(ubo));
	device.unmapMemory(uniformStagingBufferMemory);

	copyBuffer(uniformStagingBuffer, uniformBuffer, sizeof(ubo));
}

vk::Format Window::findDepthFormat()
{
	return findSupportedFormat({ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
		vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

bool Window::hasStencilComponent(vk::Format format)
{
	return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

uint32 Window::findMemoryType(uint32 typeFilter, vk::MemoryPropertyFlags properties)
{
	vk::PhysicalDeviceMemoryProperties memoryProperties;
	gpu.getMemoryProperties(&memoryProperties);

	for (uint32 i = 0; i < memoryProperties.memoryTypeCount; ++i)
	{
		if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	criticalError("Failed to find suitable memory type");
	return -1;
}

vk::Format Window::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
	for (vk::Format format : candidates)
	{
		vk::FormatProperties properties = gpu.getFormatProperties(format);
		if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}

	criticalError("Failed to find supported fromat");
}

void Window::setIcon(const std::string& path)
{
	SDL_Surface* iconSurface = loadSDL_SurfaceFromFile(path.c_str());
	SDL_SetWindowIcon(m_sdlWindow, iconSurface);
}

SDL_Window* Window::getSdlWindow() const
{
	return m_sdlWindow;
}

SDL_GLContext Window::getContext() const
{
	return m_context;
}

GraphicsApi Window::getGraphicsApi() const
{
	return m_graphicsApi;
}

void Window::showCursor(bool value)
{
	SDL_ShowCursor(value);
}

void Window::setCursorPosition(int x, int y)
{
	SDL_WarpMouseInWindow(g_window.getSdlWindow(), x, y);
	Input::m_mouseX = Input::m_lastMouseX = x;
	Input::m_mouseY = Input::m_lastMouseY = y;
}

void Window::handleWindowSizeChange(int newWidth, int newHeight)
{
	m_width = newWidth;
	m_height = newHeight;

	if (m_graphicsApi == GraphicsApi::Vulkan)
	{
		recreateSwapchain();
	}
}

void messageBox(const char* message)
{
	messageBox("Amber", message);
}

void messageBox(const std::string& message)
{
	messageBox("Amber", message.c_str());
}

void messageBox(const std::string& title, const std::string& message)
{
	messageBox(title.c_str(), message.c_str());
}

void messageBox(const char* title, const char* message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, g_window.getSdlWindow());
}

void quit()
{
	SDL_GL_DeleteContext(g_window.getContext());
	SDL_DestroyWindow(g_window.getSdlWindow());
	SDL_Quit();
	exit(0);
}

__declspec(noreturn) void _criticalError(const char* message, const char* file, int line, const char* function)
{
	std::stringstream sstream;
	sstream << message << std::endl << file << ":" << line << "  -  " << function;
	Log::error(sstream.str());
	messageBox("Critical error", sstream.str());
	std::exit(1);
}

__declspec(noreturn) void _criticalError(const std::string& message, const char* file, int line, const char* function)
{
	_criticalError(message.c_str(), file, line, function);
}

}
