#pragma once
#include "Engine/Asset/Asset.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Renderer.h"

namespace Hanabi
{
	class EnvMapAsset : public Asset
	{
	public:
		EnvMapAsset() :m_EnvMap(Renderer::GetTexture<TextureCube>("BlackCube")) {}
		EnvMapAsset(Ref<TextureCube> envMap) :m_EnvMap(envMap) {}
		~EnvMapAsset() = default;

		void Invalidate();

		void SetTopHandle(AssetHandle handle) { m_TopHandle = handle; }
		void SetBottomHandle(AssetHandle handle) { m_BottomHandle = handle; }
		void SetLeftHandle(AssetHandle handle) { m_LeftHandle = handle; }
		void SetRightHandle(AssetHandle handle) { m_RightHandle = handle; }
		void SetFrontHandle(AssetHandle handle) { m_FrontHandle = handle; }
		void SetBackHandle(AssetHandle handle) { m_BackHandle = handle; }

		Ref<TextureCube> GetEnvMap() const { return m_EnvMap; }

		AssetHandle GetTopHandle() const { return m_TopHandle; }
		AssetHandle GetBottomHandle() const { return m_BottomHandle; }
		AssetHandle GetLeftHandle() const { return m_LeftHandle; }
		AssetHandle GetRightHandle() const { return m_RightHandle; }
		AssetHandle GetFrontHandle() const { return m_FrontHandle; }
		AssetHandle GetBackHandle() const { return m_BackHandle; }

		static AssetType GetStaticType() { return AssetType::EnvMap; }
		virtual AssetType GetAssetType() const override { return GetStaticType(); }
	private:
		Ref<TextureCube> m_EnvMap;

		std::array<Buffer, 6> m_TextureData;

		AssetHandle m_TopHandle = 0;
		AssetHandle m_BottomHandle = 0;
		AssetHandle m_LeftHandle = 0;
		AssetHandle m_RightHandle = 0;
		AssetHandle m_FrontHandle = 0;
		AssetHandle m_BackHandle = 0;
	};
}