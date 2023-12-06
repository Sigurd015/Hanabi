#include "PanelManager.h"
#include "SceneHierarchyPanel.h"
#include "ContentBrowserPanel.h"
#include "ViewPortPanel.h"
#include "RendererSettingPanel.h"
#include "SelectionManager.h"

#include <unordered_map>
#include <memory>

namespace Hanabi
{
	static std::unordered_map<std::string, Ref<Panel>> m_Panels;

	template<typename T>
	static inline Ref<T> GetPanel(const std::string& name)
	{
		static_assert(std::is_base_of<Panel, T>::value, "PanelManager::GetPanel only works for types derived from Panel");

		if (m_Panels.find(name) != m_Panels.end())
			return std::static_pointer_cast<T>(m_Panels[name]);
		else
		{
			HNB_CORE_ASSERT(false, "PanelManager::Panel not found!");
			return nullptr;
		}
	}

	void PanelManager::Init()
	{
		m_Panels["SceneHierarchy"] = CreateRef<SceneHierarchyPanel>();
		m_Panels["ContentBrowser"] = CreateRef<ContentBrowserPanel>();
		m_Panels["ViewPort"] = CreateRef<ViewPortPanel>();
		m_Panels["RendererSetting"] = CreateRef<RendererSettingPanel>();
	}

	void PanelManager::RegisterOnScenePlayCallback(const std::function<void()>& callback)
	{
		GetPanel<ViewPortPanel>("ViewPort")->RegisterOnScenePlayCallback(callback);
	}

	void PanelManager::RegisterOnSceneStopCallback(const std::function<void()>& callback)
	{
		GetPanel<ViewPortPanel>("ViewPort")->RegisterOnSceneStopCallback(callback);
	}

	void PanelManager::RegisterOnSceneOpenCallback(const std::function<void(AssetHandle)>& callback)
	{
		GetPanel<ViewPortPanel>("ViewPort")->RegisterOnSceneOpenCallback(callback);
	}

	void PanelManager::OnImGuiRender()
	{
		for (auto& panel : m_Panels)
		{
			panel.second->OnImGuiRender();
		}
	}

	void PanelManager::OnUpdate(Timestep ts)
	{
		for (auto& panel : m_Panels)
		{
			panel.second->OnUpdate(ts);
		}
	}

	void PanelManager::OnEvent(Event& e)
	{
		for (auto& panel : m_Panels)
		{
			panel.second->OnEvent(e);
		}
	}

	void PanelManager::OnOpenProject()
	{
		SelectionManager::SetSelectedEntity({});
		GetPanel<ContentBrowserPanel>("ContentBrowser")->SetContext(Project::GetProjectDirectory());
	}

	void PanelManager::OnSceneChange(const Ref<Scene>& scene)
	{
		SelectionManager::SetSelectedEntity({});
		GetPanel<SceneHierarchyPanel>("SceneHierarchy")->SetContext(scene);
		GetPanel<ViewPortPanel>("ViewPort")->SetContext(scene);
	}

	AssetHandle PanelManager::OnAssetImport(const std::string& path)
	{
		return GetPanel<ContentBrowserPanel>("ContentBrowser")->ImportAsset(path);
	}
}
