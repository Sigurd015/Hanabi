#pragma once
#include "Vulkan.h"

namespace Hanabi
{
	class VulkanPhysicalDevice
	{
	public:
		struct QueueFamilyIndices
		{
			int32_t Graphics = -1;
			int32_t Compute = -1;
			int32_t Transfer = -1;
		};
		VulkanPhysicalDevice();
		~VulkanPhysicalDevice();

		static Ref<VulkanPhysicalDevice> Select();
	private:
		VkFormat FindDepthFormat() const;
		QueueFamilyIndices GetQueueFamilyIndices(int queueFlags);

		VkPhysicalDevice m_PhysicalDevice;
		VkPhysicalDeviceProperties m_Properties;
		VkPhysicalDeviceFeatures m_Features;
		VkPhysicalDeviceMemoryProperties m_MemoryProperties;
		std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
		std::unordered_set<std::string> m_SupportedExtensions;
		std::vector<VkDeviceQueueCreateInfo> m_QueueCreateInfos;
		QueueFamilyIndices m_QueueFamilyIndices;
		VkFormat m_DepthFormat = VK_FORMAT_UNDEFINED;
	};

	class VulkanDevice
	{
	public:
		VulkanDevice(const Ref<VulkanPhysicalDevice>& physicalDevice, VkPhysicalDeviceFeatures enabledFeatures);
		~VulkanDevice();
	};
}