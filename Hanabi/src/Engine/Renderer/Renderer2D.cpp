#include "hnbpch.h"
#include "Renderer2D.h"
#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"
#include "Engine/Core.h"
#include <glm/ext/matrix_transform.hpp>

namespace Hanabi
{
	struct Renderer2DStorage
	{
		Ref<VertexArray> QuadVertexArray;
		Ref<Shader> FlatColorShader;
		Ref<Shader> TextureShader;
	};
	static Renderer2DStorage* s_Data;

	static float vertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};
	static uint32_t indices[6] = { 0, 1, 2, 2,3,0 };

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DStorage();
		//VertexArray
		s_Data->QuadVertexArray = VertexArray::Create();
		//VertexBuffer
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		vertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
			});
		s_Data->QuadVertexArray->AddVertexBuffer(vertexBuffer);
		//IndexBuffer
		s_Data->QuadVertexArray->SetIndexBuffer(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));

		s_Data->FlatColorShader = Shader::Create("assets/shaders/FlatColor.glsl");

		s_Data->TextureShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetUniform("u_Texture", 0);
	}

	void Renderer2D::Shutdown()
	{
		delete s_Data;
	}

	void Renderer2D::BeginScene(const Camera& camera)
	{
		s_Data->FlatColorShader->Bind();
		s_Data->FlatColorShader->SetUniform("u_ViewProjection", camera.GetViewProjectionMatrix());

		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetUniform("u_ViewProjection", camera.GetViewProjectionMatrix());
	}

	void Renderer2D::EndScene()
	{}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const float angle, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, angle, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const float angle, const glm::vec4& color)
	{
		s_Data->FlatColorShader->SetUniform("u_Color", color);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 0, 1.0f)) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->FlatColorShader->SetUniform("u_Transform", transform);

		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const float angle, const Ref<Texture2D>& texture)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, angle, texture);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const float angle, const Ref<Texture2D>& texture)
	{
		s_Data->TextureShader->SetUniform("u_Texture", 0);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 0, 1.0f)) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->TextureShader->SetUniform("u_Transform", transform);
		
		texture->Bind();
		
		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}
}