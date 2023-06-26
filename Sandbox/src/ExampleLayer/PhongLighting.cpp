#include "PhongLighting.h"

#include <imgui.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>


PhongLighting::PhongLighting() :Layer("PhongLighting")
{}

void PhongLighting::OnAttach()
{
	Hanabi::FramebufferSpecification fbSpec;
	fbSpec.Attachments = {
		Hanabi::FramebufferTextureFormat::RGBA8F,
		Hanabi::FramebufferTextureFormat::Depth };
	fbSpec.Width = 1920;
	fbSpec.Height = 1080;
	m_Framebuffer = Hanabi::Framebuffer::Create(fbSpec);
	m_RenderPass = Hanabi::RenderPass::Create({ m_Framebuffer });

	m_Camera = Hanabi::EditorCamera(30.0f, 1920.0f / 1080.0f, 0.1f, 1000.0f);

	Hanabi::Ref<Hanabi::MeshSource> temp = Hanabi::CreateRef<Hanabi::MeshSource>("Box.fbx");
	m_Mesh = Hanabi::CreateRef<Hanabi::Mesh>(temp);

	m_DiffuseTexture = Hanabi::Texture2D::Create("assets/textures/Container_Diffuse.png");
	m_SpecularTexture = Hanabi::Texture2D::Create("assets/textures/Container_Specular.png");

	Hanabi::PipelineSpecification spec;
	spec.Layout = m_Mesh->GetVertexBuffer()->GetLayout();
	spec.Shader = Hanabi::Renderer::GetShader("PhongLighting");
	spec.Topology = Hanabi::PrimitiveTopology::Triangles;
	m_Pipeline = Hanabi::Pipeline::Create(spec);

	m_Material = Hanabi::Material::Create(spec.Shader);
	m_Material->SetTexture(m_DiffuseTexture, 0); // 0 is Diffuse slot by default
	m_Material->SetTexture(m_SpecularTexture, 1); // 1 is Specular slot by default

	HNB_INFO("CBModelData Size: {0}", sizeof(CBModel));
	HNB_INFO("CBCameraData Size: {0}", sizeof(CBCamera));
	HNB_INFO("CBSceneData Size: {0}", sizeof(CBScene));
	HNB_INFO("CBPointLightData Size: {0}", sizeof(CBPointLight));
	HNB_INFO("CBSpotLightData Size: {0}", sizeof(CBSpotLight));

	m_ModelDataBuffer = Hanabi::ConstantBuffer::Create(sizeof(CBModel), CBBingdID::MODEL);
	m_CameraDataBuffer = Hanabi::ConstantBuffer::Create(sizeof(CBCamera), CBBingdID::CAMERA);
	m_SceneDataBuffer = Hanabi::ConstantBuffer::Create(sizeof(CBScene), CBBingdID::SCENE);
	m_PointLightDataBuffer = Hanabi::ConstantBuffer::Create(sizeof(CBPointLight), CBBingdID::POINT_LIGHT);
	m_SpotLightDataBuffer = Hanabi::ConstantBuffer::Create(sizeof(CBSpotLight), CBBingdID::SPOT_LIGHT);

	{
		for (size_t i = 0; i < MAX_POINT_LIGHT; i++)
		{
			m_PointLightData.PointLights[i].Constant = 1.0f;
			m_PointLightData.PointLights[i].Exp = 0.045f;
			m_PointLightData.PointLights[i].Linear = 0.0075f;
			m_PointLightData.PointLights[i].Color = { 1.0f, 1.0f, 1.0f };
			m_PointLightData.PointLights[i].Intensity = 1.0f;
			m_PointLightData.PointLights[i].Position = { 0.0f, 0.0f, 0.0f };
		}
	}

	{
		for (size_t i = 0; i < MAX_SPOT_LIGHT; i++)
		{
			m_SpotLightData.SpotLights[i].Color = { 1.0f, 1.0f, 1.0f };
			m_SpotLightData.SpotLights[i].Intensity = 1.0f;
			m_SpotLightData.SpotLights[i].Constant = 1.0f;
			m_SpotLightData.SpotLights[i].Exp = 0.045f;
			m_SpotLightData.SpotLights[i].Linear = 0.0075f;
			m_SpotLightData.SpotLights[i].Cutoff = 0.8f;
			m_SpotLightData.SpotLights[i].Direction = { 0.0f, 0.0f, 1.0f };
		}
	}

	m_Pipeline->SetConstantBuffer(m_ModelDataBuffer);
	m_Pipeline->SetConstantBuffer(m_CameraDataBuffer);
	m_Pipeline->SetConstantBuffer(m_SceneDataBuffer);
	m_Pipeline->SetConstantBuffer(m_PointLightDataBuffer);
	m_Pipeline->SetConstantBuffer(m_SpotLightDataBuffer);
}

void PhongLighting::OnDetach()
{}

void PhongLighting::OnUpdate(Hanabi::Timestep ts)
{
	// Resize
	m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	m_Camera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);

	m_Camera.OnUpdate(ts);
	m_CameraData.ViewProj = m_Camera.GetViewProjection();
	m_CameraData.CameraPosition = m_Camera.GetPosition();

	m_CameraDataBuffer->SetData(&m_CameraData);
	m_SceneDataBuffer->SetData(&m_SceneData);
	m_PointLightDataBuffer->SetData(&m_PointLightData);
	m_SpotLightDataBuffer->SetData(&m_SpotLightData);

	Hanabi::Renderer::BeginRenderPass(m_RenderPass);

	//Hanabi::SceneRenderer::BeginScene(m_Camera.GetViewProjection());

	float radius = 10.0f;
	float angleIncrement = (glm::pi<float>() * 2) / 9.0f;
	for (size_t i = 0; i < 9; i++)
	{
		float angle = i * angleIncrement;
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(radius * std::cos(angle), 0.0f, radius * std::sin(angle)));
		Hanabi::Renderer::SubmitStaticMesh(m_Mesh, m_Material, m_Pipeline, trans, CBBingdID::MODEL);
		//Hanabi::SceneRenderer::SubmitStaticMesh(m_Mesh, m_Material, trans);
	}

	//Hanabi::SceneRenderer::EndScene();

	Hanabi::Renderer::EndRenderPass(m_RenderPass);
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
	ImGui::Image(m_Framebuffer->GetColorAttachment(), ImVec2{ m_ViewportSize.x, m_ViewportSize.y });
	ImGui::End();

	ImGui::End();
}

void PhongLighting::UI_Tool()
{
	if (ImGui::Begin("Tools"))
	{
		ImGui::Text("Directional Light Direction");
		ImGui::DragFloat("##DirectionalLightDirX", &m_SceneData.Light.Direction.x, 0.01f, 0.0f, 1.0f, "%.2f");
		ImGui::DragFloat("##DirectionalLightDirY", &m_SceneData.Light.Direction.y, 0.01f, 0.0f, 1.0f, "%.2f");
		ImGui::DragFloat("##DirectionalLightDirZ", &m_SceneData.Light.Direction.z, 0.01f, 0.0f, 1.0f, "%.2f");
		ImGui::Text("Directional Light Intensity");
		ImGui::DragFloat("##DirectionalLightIntensity", &m_SceneData.Light.Intensity, 0.01f, 0.0f, 1.0f, "%.2f");
		ImGui::Text("Directional Light Color");
		ImGui::ColorEdit3("##DirectionalLightColor", glm::value_ptr(m_SceneData.Light.Color));

		{
			for (size_t i = 0; i < MAX_POINT_LIGHT; i++)
			{
				std::string positionLabel = "Point Light Position - " + std::to_string(i);
				std::string intensityLabel = "Point Light Intensity - " + std::to_string(i);
				std::string colorLabel = "Point Light Color - " + std::to_string(i);
				std::string attenuationLabel = "Point Light Attenuation - " + std::to_string(i);

				ImGui::Text(positionLabel.c_str());
				ImGui::DragFloat(("##PointLightPosX" + std::to_string(i)).c_str(), &m_PointLightData.PointLights[i].Position.x, 0.01f, -10.0f, 10.0f, "%.2f");
				ImGui::DragFloat(("##PointLightPosY" + std::to_string(i)).c_str(), &m_PointLightData.PointLights[i].Position.y, 0.01f, -10.0f, 10.0f, "%.2f");
				ImGui::DragFloat(("##PointLightPosZ" + std::to_string(i)).c_str(), &m_PointLightData.PointLights[i].Position.z, 0.01f, -10.0f, 10.0f, "%.2f");
				ImGui::Text(intensityLabel.c_str());
				ImGui::DragFloat(("##PointLightIntensity" + std::to_string(i)).c_str(), &m_PointLightData.PointLights[i].Intensity, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::Text(colorLabel.c_str());
				ImGui::ColorEdit3(("##PointLightColor" + std::to_string(i)).c_str(), glm::value_ptr(m_PointLightData.PointLights[i].Color));
				ImGui::Text(attenuationLabel.c_str());
				ImGui::DragFloat(("##PointLightAttenuationConstant" + std::to_string(i)).c_str(), &m_PointLightData.PointLights[i].Constant, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::DragFloat(("##PointLightAttenuationExp" + std::to_string(i)).c_str(), &m_PointLightData.PointLights[i].Exp, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::DragFloat(("##PointLightAttenuationLinear" + std::to_string(i)).c_str(), &m_PointLightData.PointLights[i].Linear, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::Separator();
			}
		}
		{
			for (size_t i = 0; i < MAX_SPOT_LIGHT; i++)
			{
				std::string positionLabel = "Spot Light Position - " + std::to_string(i);
				std::string directionLabel = "Spot Light Direction - " + std::to_string(i);
				std::string intensityLabel = "Spot Light Intensity - " + std::to_string(i);
				std::string colorLabel = "Spot Light Color - " + std::to_string(i);

				ImGui::Text(positionLabel.c_str());
				ImGui::DragFloat(("##SpotLightPosX" + std::to_string(i)).c_str(), &m_SpotLightData.SpotLights[i].Position.x, 0.01f, -10.0f, 10.0f, "%.2f");
				ImGui::DragFloat(("##SpotLightPosY" + std::to_string(i)).c_str(), &m_SpotLightData.SpotLights[i].Position.y, 0.01f, -10.0f, 10.0f, "%.2f");
				ImGui::DragFloat(("##SpotLightPosZ" + std::to_string(i)).c_str(), &m_SpotLightData.SpotLights[i].Position.z, 0.01f, -10.0f, 10.0f, "%.2f");
				ImGui::Text(directionLabel.c_str());
				ImGui::DragFloat(("##SpotLightDirX" + std::to_string(i)).c_str(), &m_SpotLightData.SpotLights[i].Direction.x, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::DragFloat(("##SpotLightDirY" + std::to_string(i)).c_str(), &m_SpotLightData.SpotLights[i].Direction.y, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::DragFloat(("##SpotLightDirZ" + std::to_string(i)).c_str(), &m_SpotLightData.SpotLights[i].Direction.z, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::Text(intensityLabel.c_str());
				ImGui::DragFloat(("##SpotLightIntensity" + std::to_string(i)).c_str(), &m_SpotLightData.SpotLights[i].Intensity, 0.01f, 0.0f, 1.0f, "%.2f");
				ImGui::Text(colorLabel.c_str());
				ImGui::ColorEdit3(("##SpotLightColor" + std::to_string(i)).c_str(), glm::value_ptr(m_SpotLightData.SpotLights[i].Color));
				ImGui::Separator();
			}
		}
	}
	ImGui::End();
}