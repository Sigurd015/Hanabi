#include "EditorLayer.h"
#include "EditorResources.h"
#include "SelectionManager.h"

namespace Hanabi
{
	EditorLayer::EditorLayer() : Layer("EditorLayer")
	{}

	void EditorLayer::OnAttach()
	{
		PanelManager::Init();
		PanelManager::RegisterOnSceneOpenCallback(HNB_BIND_EVENT_FN(EditorLayer::OpenScene));
		PanelManager::RegisterOnScenePlayCallback(HNB_BIND_EVENT_FN(EditorLayer::OnScenePlay));
		PanelManager::RegisterOnSceneStopCallback(HNB_BIND_EVENT_FN(EditorLayer::OnSceneStop));

		auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
		if (commandLineArgs.Count > 1)
		{
			auto projectFilePath = commandLineArgs[1];
			OpenProject(projectFilePath);
		}
		else
		{
			if (!OpenProject())
				Application::Get().Close();
		}
	}

	void EditorLayer::OnDetach()
	{}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		PanelManager::OnUpdate(ts);
	}

	void EditorLayer::OnImGuiRender()
	{
		#pragma region Dockspace Settings
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		{
			UI::ScopedStyle windowPadding(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		}	

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = minWinSizeX;
		#pragma endregion	

		#pragma region Menu Bar	
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
				if (ImGui::MenuItem("Open Project...", "Ctrl+O"))
					OpenProject();

				ImGui::Separator();

				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
					NewScene();

				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					SaveScene();

				if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
					SaveSceneAs();

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
					Application::Get().Close();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Debug"))
			{
				ImGui::MenuItem("Show ImGui Debug Window", nullptr, &m_ImGuiDebugWndDraw);

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		#pragma endregion	

		PanelManager::OnImGuiRender();

		if (m_ImGuiDebugWndDraw)
			ImGui::ShowMetricsWindow();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		PanelManager::OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(HNB_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<WindowDropEvent>(HNB_BIND_EVENT_FN(EditorLayer::OnWindowDrop));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shortcuts
		if (e.IsRepeat())
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		switch (e.GetKeyCode())
		{
		case Key::N:
		{
			if (control)
				NewScene();
			break;
		}
		case Key::O:
		{
			if (control)
				OpenProject();
			break;
		}
		case Key::S:
		{
			if (control)
			{
				if (shift)
					SaveSceneAs();
				else
					SaveScene();
			}
			break;
		}
		}
		return false;
	}

	bool EditorLayer::OnWindowDrop(WindowDropEvent& e)
	{
		// TODO: if a project is dropped in, probably open it

		//AssetManager::ImportAsset();

		return true;
	}

	bool EditorLayer::OpenProject()
	{
		std::string filepath = FileDialogs::OpenFile("Hanabi Project (*.hproj)\0*.hproj\0");
		if (filepath.empty())
			return false;

		OpenProject(filepath);
		return true;
	}

	void EditorLayer::OpenProject(const std::filesystem::path& path)
	{
		if (Project::Load(path))
		{
			ScriptEngine::LoadAppAssembly(Project::GetAssetDirectory() / Project::GetActive()->GetConfig().ScriptModulePath);
			AssetHandle startSceneHandle = Project::GetActive()->GetConfig().StartScene;
			OpenScene(startSceneHandle);
			Application::Get().GetWindow().SetWindowTitle(Project::GetProjectName());
			PanelManager::OnOpenProject();
		}
	}

	void EditorLayer::SaveProject()
	{
		//Project::Save();
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		PanelManager::OnSceneChange(m_ActiveScene);
	}

	void EditorLayer::OpenScene(AssetHandle handle)
	{
		Ref<Scene> newScene = nullptr;
		if (AssetManager::IsAssetHandleValid(handle)
			&& AssetManager::GetAssetType(handle) == AssetType::Scene)
		{
			newScene = AssetManager::GetAsset<Scene>(handle);
		}

		if (newScene != nullptr)
		{
			m_EditorScene = newScene;
			m_ActiveScene = m_EditorScene;
			PanelManager::OnSceneChange(m_EditorScene);
		}
	}

	void EditorLayer::SaveScene()
	{
		if (AssetManager::IsAssetHandleValid(m_ActiveScene->Handle))
		{
			AssetImporter::Serialize(m_ActiveScene);
		}
		else
		{
			SaveSceneAs();
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Scenes (*.hscene)\0*.hscene\0");
		if (!filepath.empty())
		{
			if (AssetManager::IsAssetHandleValid(m_ActiveScene->Handle))
				Project::GetEditorAssetManager()->ReloadData(m_ActiveScene->Handle);
			m_ActiveScene->Handle = PanelManager::OnAssetImport(filepath);
			return SaveScene();
		}
	}

	void EditorLayer::OnScenePlay()
	{
		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();

		PanelManager::OnSceneChange(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		m_ActiveScene = m_EditorScene;

		PanelManager::OnSceneChange(m_ActiveScene);
	}
}