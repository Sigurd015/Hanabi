#include "PhongLighting.h"

#include <imgui.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>


PhongLighting::PhongLighting() :Layer("PhongLighting")
{}

void PhongLighting::OnAttach()
{
	m_Camera = Hanabi::EditorCamera(30.0f, 1920.0f / 1080.0f, 0.1f, 1000.0f);

	Hanabi::Ref<Hanabi::MeshSource> temp = Hanabi::CreateRef<Hanabi::MeshSource>("Box.fbx");
	m_Mesh = Hanabi::CreateRef<Hanabi::Mesh>(temp);

	m_DiffuseTexture = Hanabi::Texture2D::Create("assets/textures/Container_Diffuse.png");
	m_SpecularTexture = Hanabi::Texture2D::Create("assets/textures/Container_Specular.png");

	m_Material = Hanabi::Material::Create(Hanabi::Renderer::GetShader("PhongLighting"));
	m_Material->SetTexture(m_DiffuseTexture, 0); // 0 is Diffuse slot by default
	m_Material->SetTexture(m_SpecularTexture, 1); // 1 is Specular slot by default

	m_Environment.PointLightCount = 2;
	m_Environment.SpotLightCount = 2;
	{
		for (size_t i = 0; i < m_Environment.PointLightCount; i++)
		{
			m_Environment.PointLights[i].Constant = 1.0f;
			m_Environment.PointLights[i].Exp = 0.045f;
			m_Environment.PointLights[i].Linear = 0.0075f;
			m_Environment.PointLights[i].Color = { 1.0f, 1.0f, 1.0f };
			m_Environment.PointLights[i].Intensity = 1.0f;
			m_Environment.PointLights[i].Position = { 0.0f, 0.0f, 0.0f };
		}
	}

	{
		for (size_t i = 0; i < m_Environment.SpotLightCount; i++)
		{
			m_Environment.SpotLights[i].Color = { 1.0f, 1.0f, 1.0f };
			m_Environment.SpotLights[i].Intensity = 1.0f;
			m_Environment.SpotLights[i].Constant = 1.0f;
			m_Environment.SpotLights[i].Exp = 0.045f;
			m_Environment.SpotLights[i].Linear = 0.0075f;
			m_Environment.SpotLights[i].Cutoff = 0.8f;
			m_Environment.SpotLights[i].Direction = { 0.0f, 0.0f, 1.0f };
		}
	}

	float radius = 10.0f;
	float angleIncrement = (glm::pi<float>() * 2) / 9.0f;
	for (size_t i = 0; i < 9; i++)
	{
		float angle = i * angleIncrement;
		m_ModelMatrices.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(radius * std::cos(angle), 0.0f, radius * std::sin(angle))));
	}
}

void PhongLighting::OnDetach()
{}

void PhongLighting::OnUpdate(Hanabi::Timestep ts)
{
	// Resize
	Hanabi::SceneRenderer::SetViewportSize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	m_Camera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);

	m_Camera.OnUpdate(ts);
	m_Environment.CameraPosition = m_Camera.GetPosition();
	m_Environment.ViewProjection = m_Camera.GetViewProjection();

	Hanabi::SceneRenderer::BeginScene(m_Environment);

	for (auto& trans : m_ModelMatrices)
	{
		Hanabi::SceneRenderer::SubmitStaticMesh(m_Mesh, m_Material, trans);
	}

	Hanabi::SceneRenderer::EndScene();
}

void PhongLighting::OnEvent(Hanabi::Event& evnet)
{
	m_Camera.OnEvent(evnet);
}

void PhongLighting::OnImGuiRender()
{
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
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	ImGui::PopStyleVar();

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

	UI_Tool();

	ImGui::Begin("Viewport");
	Hanabi::Application::Get().GetImGuiLayer()->BlockEvents(!ImGui::IsWindowHovered());
	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
	ImGui::Image(Hanabi::SceneRenderer::GetFinalResult()->GetColorAttachment(), ImVec2{ m_ViewportSize.x, m_ViewportSize.y });
	ImGui::End();

	ImGui::End();
}

void PhongLighting::UI_Tool()
{
	if (ImGui::Begin("Tools"))
	{
		ImGui::Text("Directional Light Direction");
		ImGui::DragFloat("##DirectionalLightDirX", &m_Environment.DirLight.Direction.x, 0.01f, 0.0f, 1.0f, "%.2f");
		ImGui::DragFloat("##DirectionalLightDirY", &m_Environment.DirLight.Direction.y, 0.01f, 0.0f, 1.0f, "%.2f");
		ImGui::DragFloat("##DirectionalLightDirZ", &m_Environment.DirLight.Direction.z, 0.01f, 0.0f, 1.0f, "%.2f");
		ImGui::Text("Directional Light Intensity");
		ImGui::DragFloat("##DirectionalLightIntensity", &m_Environment.DirLight.Intensity, 0.01f, 0.0f, 1.0f, "%.2f");
		ImGui::Text("Directional Light Color");
		ImGui::ColorEdit3("##DirectionalLightColor", glm::value_ptr(m_Environment.DirLight.Color));

		{
			for (size_t i = 0; i < m_Environment.PointLightCount; i++)
			{
				std::string positionLabel = "Point Light Position - " + std::to_string(i);
				std::string intensityLabel = "Point Light Intensity - " + std::to_string(i);
				std::string colorLabel = "Point Light Color - " + std::to_string(i);
				std::string attenuationLabel = "Point Light Attenuation - " + std::to_string(i);

				ImGui::Text(positionLabel.c_str());
				ImGui::DragFloat(("##PointLightPosX" + std::to_string(i)).c_str(), &m_Environment.PointLights[i].Position.x, 0.01f, -10.0f, 10.0f, "%.2f");
				ImGui::DragFloat(("##PointLightPosY" + std::to_string(i)).c_str(), &m_Environment.PointLights[i].Position.y, 0.01f, -10.0f, 10.0f, "%.2f");
				ImGui::DragFloat(("##PointLightPosZ" + std::to_string(i)).c_str(), &m_Environment.PointLights[i].Position.z, 0.01f, -10.0f, 10.0f, "%.2f");
				ImGui::Text(intensityLabel.c_str());
				ImGui::DragFloat(("##PointLightIntensity" + std::to_string(i)).c_str(), &m_Environment.PointLights[i].Intensity, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::Text(colorLabel.c_str());
				ImGui::ColorEdit3(("##PointLightColor" + std::to_string(i)).c_str(), glm::value_ptr(m_Environment.PointLights[i].Color));
				ImGui::Text(attenuationLabel.c_str());
				ImGui::DragFloat(("##PointLightAttenuationConstant" + std::to_string(i)).c_str(), &m_Environment.PointLights[i].Constant, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::DragFloat(("##PointLightAttenuationExp" + std::to_string(i)).c_str(), &m_Environment.PointLights[i].Exp, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::DragFloat(("##PointLightAttenuationLinear" + std::to_string(i)).c_str(), &m_Environment.PointLights[i].Linear, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::Separator();
			}
		}
		{
			for (size_t i = 0; i < m_Environment.SpotLightCount; i++)
			{
				std::string positionLabel = "Spot Light Position - " + std::to_string(i);
				std::string directionLabel = "Spot Light Direction - " + std::to_string(i);
				std::string intensityLabel = "Spot Light Intensity - " + std::to_string(i);
				std::string colorLabel = "Spot Light Color - " + std::to_string(i);

				ImGui::Text(positionLabel.c_str());
				ImGui::DragFloat(("##SpotLightPosX" + std::to_string(i)).c_str(), &m_Environment.SpotLights[i].Position.x, 0.01f, -10.0f, 10.0f, "%.2f");
				ImGui::DragFloat(("##SpotLightPosY" + std::to_string(i)).c_str(), &m_Environment.SpotLights[i].Position.y, 0.01f, -10.0f, 10.0f, "%.2f");
				ImGui::DragFloat(("##SpotLightPosZ" + std::to_string(i)).c_str(), &m_Environment.SpotLights[i].Position.z, 0.01f, -10.0f, 10.0f, "%.2f");
				ImGui::Text(directionLabel.c_str());
				ImGui::DragFloat(("##SpotLightDirX" + std::to_string(i)).c_str(), &m_Environment.SpotLights[i].Direction.x, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::DragFloat(("##SpotLightDirY" + std::to_string(i)).c_str(), &m_Environment.SpotLights[i].Direction.y, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::DragFloat(("##SpotLightDirZ" + std::to_string(i)).c_str(), &m_Environment.SpotLights[i].Direction.z, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::Text(intensityLabel.c_str());
				ImGui::DragFloat(("##SpotLightIntensity" + std::to_string(i)).c_str(), &m_Environment.SpotLights[i].Intensity, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::Text(colorLabel.c_str());
				ImGui::ColorEdit3(("##SpotLightColor" + std::to_string(i)).c_str(), glm::value_ptr(m_Environment.SpotLights[i].Color));
				ImGui::Separator();
			}
		}
	}
	ImGui::End();
}