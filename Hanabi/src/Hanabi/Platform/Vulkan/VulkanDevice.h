#pragma once
#include "Vulkan.h"

#include <unordered_set>

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

		bool IsExtensionSupported(const std::string& extensionName) const;
		VkPhysicalDevice GetVulkanPhysicalDevice() const { return m_PhysicalDevice; }
		VkFormat GetDepthFormat() const { return m_DepthFormat; }

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

		friend class VulkanDevice;
	};

	class VulkanDevice
	{
	public:
		VulkanDevice(const Ref<VulkanPhysicalDevice>& physicalDevice, VkPhysicalDeviceFeatures enabledFeatures);
		~VulkanDevice();

		const Ref<VulkanPhysicalDevice>& GetPhysicalDevice() const { return m_PhysicalDevice; }
		VkDevice GetVulkanDevice() const { return m_LogicalDevice; }
	private:
		VkDevice m_LogicalDevice = nullptr;
		Ref<VulkanPhysicalDevice> m_PhysicalDevice;
		VkPhysicalDeviceFeatures m_EnabledFeatures;

		VkQueue m_GraphicsQueue;
		VkQueue m_ComputeQueue;

		bool m_EnableDebugMarkers = false;
	};
}