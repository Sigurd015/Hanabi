#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Math/Math.h"
#include "Engine/Utils/PlatformUtils.h"

// ---Layer---------------------
#include "Engine/Layer/Layer.h"
#include "Engine/Layer/ImGui/ImGuiLayer.h"
#include "Engine/Layer/ImGui/ScopedStyleColor.h"

// ---Input---------------------
#include "Engine/Input/Input.h"
#include "Engine/Input/KeyCodes.h"
#include "Engine/Input/MouseCodes.h"

// ---Event---------------------	
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"

// ---Renderer---------------------
#include "Engine/Renderer/RenderPass.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/RendererAPI.h"
#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Renderer/SceneRenderer.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/IndexBuffer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/PipeLine.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Renderer/Framebuffer.h"
#include "Engine/Renderer/ConstantBuffer.h"
#include "Engine/Renderer/UI/Font.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Renderer/MaterialAsset.h"

// ---Scene---------------------
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/SceneSerializer.h"

// ---Project---------------------
#include "Engine/Project/Project.h"

// ---Scripting---------------------	
#include "Engine/Scripting/ScriptEngine.h"