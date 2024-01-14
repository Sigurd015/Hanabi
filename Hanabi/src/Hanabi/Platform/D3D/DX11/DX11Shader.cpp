#include "hnbpch.h"

#ifdef HNB_ENABLE_D3D
#include "DX11Shader.h"
#include "DX11Context.h"

namespace Hanabi
{
	void DX11Shader::Bind() const
	{
		// Unbind all shaders
		DX11Context::GetDeviceContext()->VSSetShader(nullptr, nullptr, 0);
		DX11Context::GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);
		DX11Context::GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);
		DX11Context::GetDeviceContext()->CSSetShader(nullptr, nullptr, 0);

		// Bind the shaders
		DX11Context::GetDeviceContext()->VSSetShader(m_VertexShader.Get(), nullptr, 0);
		DX11Context::GetDeviceContext()->GSSetShader(m_GeometryShader.Get(), nullptr, 0);
		DX11Context::GetDeviceContext()->PSSetShader(m_PixelShader.Get(), nullptr, 0);
		DX11Context::GetDeviceContext()->CSSetShader(m_ComputeShader.Get(), nullptr, 0);
	}

	DX11Shader::~DX11Shader()
	{
		m_VertexShaderBlob.Reset();
		m_VertexShader.Reset();
		m_GeometryShader.Reset();
		m_PixelShader.Reset();
		m_ComputeShader.Reset();
	}
}
#endif