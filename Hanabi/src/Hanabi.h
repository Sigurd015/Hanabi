#pragma once

#include "Hanabi/Core/Base.h"
#include "Hanabi/Core/Assert.h"
#include "Hanabi/Core/Application.h"
#include "Hanabi/Core/Log.h"
#include "Hanabi/Core/Timestep.h"
#include "Hanabi/Math/Math.h"
#include "Hanabi/Utils/PlatformUtils.h"

// ---Layer---------------------
#include "Hanabi/Layer/Layer.h"

// ---ImGui---------------------
#include "Hanabi/ImGui/ImGui.h"
#include "Hanabi/ImGui/ImGuiLayer.h"

// ---Input---------------------
#include "Hanabi/Input/Input.h"
#include "Hanabi/Input/KeyCodes.h"

// ---Event---------------------	
#include "Hanabi/Events/KeyEvent.h"
#include "Hanabi/Events/MouseEvent.h"

// ---Asset---------------------
#include "Hanabi/Asset/Asset.h"
#include "Hanabi/Asset/AssetManager/AssetManagerBase.h"
#include "Hanabi/Asset/AssetManager/AssetManager.h"
#include "Hanabi/Asset/AssetImporter.h"
#include "Hanabi/Asset/AssetMetadata.h"
#include "Hanabi/Asset/AssetSerializer/AssetSerializer.h"

// ---Math---------------------
#include "Hanabi/Math/AABB.h"
#include "Hanabi/Math/Ray.h"

// ---Renderer---------------------
#include "Hanabi/Renderer/RenderPass.h"
#include "Hanabi/Renderer/RendererConfig.h"
#include "Hanabi/Renderer/Renderer.h"
#include "Hanabi/Renderer/RendererAPI.h"
#include "Hanabi/Renderer/Renderer2D.h"
#include "Hanabi/Renderer/SceneRenderer.h"
#include "Hanabi/Renderer/Texture.h"
#include "Hanabi/Renderer/Shader.h"
#include "Hanabi/Renderer/IndexBuffer.h"
#include "Hanabi/Renderer/VertexBuffer.h"
#include "Hanabi/Renderer/PipeLine.h"
#include "Hanabi/Renderer/Camera.h"
#include "Hanabi/Renderer/Framebuffer.h"
#include "Hanabi/Renderer/ConstantBuffer.h"
#include "Hanabi/Renderer/UI/Font.h"
#include "Hanabi/Renderer/Mesh.h"
#include "Hanabi/Renderer/Material.h"
#include "Hanabi/Renderer/MaterialAsset.h"

// ---Editor---------------------
#include "Hanabi/Editor/EditorCamera.h"

// ---Scene---------------------
#include "Hanabi/Scene/Scene.h"
#include "Hanabi/Scene/Components.h"
#include "Hanabi/Scene/Entity.h"

// ---Project---------------------
#include "Hanabi/Project/Project.h"

// ---Scripting---------------------	
#include "Hanabi/Scripting/ScriptEngine.h"