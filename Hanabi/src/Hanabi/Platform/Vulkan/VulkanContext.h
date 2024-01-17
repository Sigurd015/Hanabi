#pragma once
#include "Hanabi/Renderer/RendererContext.h"
#include "Vulkan.h"
#include "VulkanDevice.h"

#include <GLFW/glfw3.h>

namespace Hanabi
{
	class VulkanContext :public RendererContext
	{
	public:
		VulkanContext(GLFWwindow* windowHandle);
		~VulkanContext();

		virtual void SwapBuffer(bool VSync) override;

		static VkInstance GetInstance() { return s_VulkanInstance; }
	private:
		inline static VkInstance s_VulkanInstance;

		VkDebugUtilsMessengerEXT m_DebugUtilsMessenger = VK_NULL_HANDLE;

		Ref<VulkanPhysicalDevice> m_PhysicalDevice;
		Ref<VulkanDevice> m_Device;
	};
}