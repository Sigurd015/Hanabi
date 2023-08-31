#include "hnbpch.h"
#include "EnvMapAsset.h"
#include "Engine/Asset/AssetManager/AssetManager.h"

namespace Hanabi
{
	// CubeMap Order
	// 0: Right
	// 1: Left
	// 2: Top
	// 3: Bottom
	// 4: Front
	// 5: Back

	void EnvMapAsset::Invalidate()
	{
		TextureSpecification spec;

		if (AssetManager::IsAssetHandleValid(m_RightHandle))
		{
			Ref<Texture2D> tex = AssetManager::GetAsset<Texture2D>(m_RightHandle);
			m_TextureData[0] = tex->GetWriteableBuffer();
			spec = tex->GetSpecification();
		}
		else
			return;

		if (AssetManager::IsAssetHandleValid(m_LeftHandle))
		{
			Ref<Texture2D> tex = AssetManager::GetAsset<Texture2D>(m_LeftHandle);
			m_TextureData[1] = tex->GetWriteableBuffer();
			spec = tex->GetSpecification();
		}
		else
			return;

		if (AssetManager::IsAssetHandleValid(m_TopHandle))
		{
			Ref<Texture2D> tex = AssetManager::GetAsset<Texture2D>(m_TopHandle);
			m_TextureData[2] = tex->GetWriteableBuffer();
			spec = tex->GetSpecification();
		}
		else
			return;

		if (AssetManager::IsAssetHandleValid(m_BottomHandle))
		{
			Ref<Texture2D> tex = AssetManager::GetAsset<Texture2D>(m_BottomHandle);
			m_TextureData[3] = tex->GetWriteableBuffer();
			spec = tex->GetSpecification();
		}
		else
			return;

		if (AssetManager::IsAssetHandleValid(m_FrontHandle))
		{
			Ref<Texture2D> tex = AssetManager::GetAsset<Texture2D>(m_FrontHandle);
			m_TextureData[4] = tex->GetWriteableBuffer();
			spec = tex->GetSpecification();
		}
		else
			return;

		if (AssetManager::IsAssetHandleValid(m_BackHandle))
		{
			Ref<Texture2D> tex = AssetManager::GetAsset<Texture2D>(m_BackHandle);
			m_TextureData[5] = tex->GetWriteableBuffer();
			spec = tex->GetSpecification();
		}
		else
			return;

		m_EnvMap = TextureCube::Create(spec, m_TextureData);
	}
}