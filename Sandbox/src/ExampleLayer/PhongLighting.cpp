#include "PhongLighting.h"

#include <imgui.h>
#include <iostream>


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

	m_SceneDataBuffer = Hanabi::ConstantBuffer::Create(sizeof(SceneData), 0);

	m_SceneData = new SceneData();
	m_SceneData->AmbientColor = { 0.1f, 0.1f, 0.1f, 1.0f };
	m_SceneData->Direction = { 0.0f, 0.0f, 0.0f };
	m_Pipeline->SetConstantBuffer(m_SceneDataBuffer);

	Hanabi::Ref<Hanabi::ConstantBuffer> constantBuffer = Hanabi::ConstantBuffer::Create(sizeof(glm::mat4), 1);
	m_Pipeline->SetConstantBuffer(constantBuffer);
}

void PhongLighting::OnDetach()
{}

void PhongLighting::OnUpdate(Hanabi::Timestep ts)
{
	// Resize
	if (Hanabi::FramebufferSpecification spec = m_Framebuffer->GetSpecification();
		m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
		(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
	{
		m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_Camera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
	}

	m_Camera.OnUpdate(ts);
	m_SceneData->ViewProj = m_Camera.GetViewProjection();
	m_SceneDataBuffer->SetData(m_SceneData);

	Hanabi::Renderer::BeginRenderPass(m_RenderPass);

	//Hanabi::SceneRenderer::BeginScene(m_Camera.GetViewProjection());

	float radius = 10.0f;
	float angleIncrement = (glm::pi<float>() * 2) / 9.0f;
	for (size_t i = 0; i < 9; i++)
	{
		float angle = i * angleIncrement;
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(radius * std::cos(angle), 0.0f, radius * std::sin(angle)));
		Hanabi::Renderer::SubmitStaticMesh(m_Mesh, m_Material, m_Pipeline, trans);
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
		if (ImGui::Button("Reset"))
		{
			m_SceneData->AmbientColor = { 0.1f, 0.1f, 0.1f, 1.0f };
		}
		ImGui::DragFloat("##X", &m_SceneData->AmbientColor.x, 0.01f, 0.0f, 1.0f, "%.2f");
		ImGui::DragFloat("##Y", &m_SceneData->AmbientColor.y, 0.01f, 0.0f, 1.0f, "%.2f");
		ImGui::DragFloat("##Z", &m_SceneData->AmbientColor.z, 0.01f, 0.0f, 1.0f, "%.2f");
		ImGui::DragFloat("##W", &m_SceneData->AmbientColor.w, 0.01f, 0.0f, 1.0f, "%.2f");
	}
	ImGui::End();
}