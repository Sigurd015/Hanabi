#pragma once
#include "Hanabi.h"

namespace Hanabi
{
	class Panel
	{
	public:
		Panel() = default;
		virtual	void OnImGuiRender() = 0;
		virtual void OnUpdate(Timestep ts) = 0;
		virtual void OnEvent(Event& e) = 0;
	};

	class PanelManager
	{
	public:
		static void Init();

		static void RegisterOnScenePlayCallback(const std::function<void()>& callback);
		static void RegisterOnSceneStopCallback(const std::function<void()>& callback);
		static void RegisterOnSceneOpenCallback(const std::function<void(AssetHandle)>& callback);

		static void OnImGuiRender();
		static void OnUpdate(Timestep ts);
		static void OnEvent(Event& e);

		static void OnOpenProject();
		static void OnSceneChange(const Ref<Scene>& scene);
		static AssetHandle OnAssetImport(const std::string& path);
	};
}