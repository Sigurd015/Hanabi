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

	private:
		inline static VkInstance s_VulkanInstance;

		Ref<VulkanDevice> m_Device;
	};
}