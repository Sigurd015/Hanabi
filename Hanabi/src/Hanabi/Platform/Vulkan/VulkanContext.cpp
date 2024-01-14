#include "hnbpch.h"
#include "VulkanContext.h"

#ifdef HNB_PLATFORM_WINDOWS
#define VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME "VK_KHR_win32_surface"
#elif defined(HNB_PLATFORM_LINUX)
#define VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME "VK_KHR_xcb_surface"
#endif

namespace Hanabi
{
	VulkanContext::VulkanContext(GLFWwindow* windowHandle)
	{
		HNB_CORE_ASSERT(glfwVulkanSupported(), "GLFW must support Vulkan!");

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hanabi";
		appInfo.pEngineName = "Hanabi";
		appInfo.apiVersion = VK_API_VERSION_1_2;

		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &appInfo;
		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.ppEnabledLayerNames = 0;

		std::vector<const char*> instanceExtensions;
		instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		instanceExtensions.push_back(VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME);

#ifndef HNB_DIST
		instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		instanceCreateInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
		instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

#ifndef HNB_DIST
		const char* validationLayerName = "VK_LAYER_KHRONOS_validation";

		uint32_t instanceLayerCount;
		vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
		std::vector<VkLayerProperties> instanceLayerProperties(instanceLayerCount);
		vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProperties.data());
		bool validationLayerPresent = false;

		for (const VkLayerProperties& layer : instanceLayerProperties)
		{
			if (strcmp(layer.layerName, validationLayerName) == 0)
			{
				validationLayerPresent = true;
				break;
			}
		}

		if (validationLayerPresent)
		{
			instanceCreateInfo.enabledLayerCount = 1;
			instanceCreateInfo.ppEnabledLayerNames = &validationLayerName;
		}
		else
		{
			HNB_CORE_ERROR("Validation layer VK_LAYER_KHRONOS_validation not present!");
		}
#endif

		VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &s_VulkanInstance));
		Utils::VulkanLoadDebugUtilsExtensions(s_VulkanInstance);
	}

	VulkanContext::~VulkanContext()
	{
		vkDestroyInstance(s_VulkanInstance, nullptr);
		s_VulkanInstance = nullptr;
	}

	void VulkanContext::SwapBuffer(bool VSync)
	{
	}
}