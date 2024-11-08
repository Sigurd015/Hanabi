#ifdef HNB_ENABLE_D3D
#pragma once
#include "Hanabi/Platform/D3D/DXCommon.h"
#include "DX11Context.h"

#include <d3d11.h>

namespace Hanabi
{
	class DX11RenderStates
	{
	public:
		inline static ComPtr <ID3D11DepthStencilState> DSSNoDepthTest = nullptr;
		inline static ComPtr <ID3D11DepthStencilState> DSSLess = nullptr;
		inline static ComPtr <ID3D11DepthStencilState> DSSLessEqual = nullptr;

		inline static ComPtr <ID3D11RasterizerState> RSNoCull = nullptr;
		inline static ComPtr <ID3D11RasterizerState> RSCullBack = nullptr;
		inline static ComPtr <ID3D11RasterizerState> RSCullFront = nullptr;

		inline static ComPtr <ID3D11BlendState> BSNoBlend = nullptr;
		inline static ComPtr <ID3D11BlendState> BSAlpha = nullptr;
		inline static ComPtr <ID3D11BlendState> BSAdditive = nullptr;
		inline static ComPtr <ID3D11BlendState> BSSubtractive = nullptr;

		inline static std::unordered_map<std::string, ComPtr <ID3D11SamplerState>> SamplerStates;

		static void Init()
		{
			// Depth Stencil States
			{
				D3D11_DEPTH_STENCIL_DESC depthTestingDesc = {};
				depthTestingDesc.DepthEnable = false;
				depthTestingDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
				depthTestingDesc.DepthFunc = D3D11_COMPARISON_LESS;
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateDepthStencilState(&depthTestingDesc, DSSNoDepthTest.GetAddressOf()));
			}
			{
				D3D11_DEPTH_STENCIL_DESC depthTestingDesc = {};
				depthTestingDesc.DepthEnable = true;
				depthTestingDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
				depthTestingDesc.DepthFunc = D3D11_COMPARISON_LESS;
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateDepthStencilState(&depthTestingDesc, DSSLess.GetAddressOf()));
			}
			{
				D3D11_DEPTH_STENCIL_DESC depthTestingDesc = {};
				depthTestingDesc.DepthEnable = true;
				depthTestingDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
				depthTestingDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
				depthTestingDesc.StencilEnable = false;
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateDepthStencilState(&depthTestingDesc, DSSLessEqual.GetAddressOf()));
			}

			// Rasterizer States
			// FrontCounterClockwise - Use CCW winding order for front faces
			{
				D3D11_RASTERIZER_DESC rasterizerDesc = {};
				rasterizerDesc.FillMode = D3D11_FILL_SOLID;
				rasterizerDesc.CullMode = D3D11_CULL_NONE;
				rasterizerDesc.FrontCounterClockwise = true;
				rasterizerDesc.DepthClipEnable = true;
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateRasterizerState(&rasterizerDesc, RSNoCull.GetAddressOf()));
			}
			{
				D3D11_RASTERIZER_DESC rasterizerDesc = {};
				rasterizerDesc.FillMode = D3D11_FILL_SOLID;
				rasterizerDesc.CullMode = D3D11_CULL_BACK;
				rasterizerDesc.FrontCounterClockwise = true;
				rasterizerDesc.DepthClipEnable = true;
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateRasterizerState(&rasterizerDesc, RSCullBack.GetAddressOf()));
			}
			{
				D3D11_RASTERIZER_DESC rasterizerDesc = {};
				rasterizerDesc.FillMode = D3D11_FILL_SOLID;
				rasterizerDesc.CullMode = D3D11_CULL_FRONT;
				rasterizerDesc.FrontCounterClockwise = true;
				rasterizerDesc.DepthClipEnable = true;
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateRasterizerState(&rasterizerDesc, RSCullFront.GetAddressOf()));
			}

			// Blend States
			{
				D3D11_BLEND_DESC blendDesc = {};
				blendDesc.AlphaToCoverageEnable = false;
				blendDesc.IndependentBlendEnable = false;
				blendDesc.RenderTarget[0].BlendEnable = false;
				blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
				blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
				blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateBlendState(&blendDesc, BSNoBlend.GetAddressOf()));
			}
			{
				D3D11_BLEND_DESC blendDesc = {};
				blendDesc.AlphaToCoverageEnable = false;
				blendDesc.IndependentBlendEnable = false;
				blendDesc.RenderTarget[0].BlendEnable = true;
				blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateBlendState(&blendDesc, BSAlpha.GetAddressOf()));
			}
			{
				D3D11_BLEND_DESC blendDesc = {};
				blendDesc.AlphaToCoverageEnable = false;
				blendDesc.IndependentBlendEnable = false;
				blendDesc.RenderTarget[0].BlendEnable = true;
				blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
				blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateBlendState(&blendDesc, BSAdditive.GetAddressOf()));
			}
			{
				D3D11_BLEND_DESC blendDesc = {};
				blendDesc.AlphaToCoverageEnable = false;
				blendDesc.IndependentBlendEnable = false;
				blendDesc.RenderTarget[0].BlendEnable = true;
				blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
				blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
				blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateBlendState(&blendDesc, BSSubtractive.GetAddressOf()));
			}

			// Sampler State
			{
				// TODO: Make MaxAnisotropy configurable
				D3D11_SAMPLER_DESC samplerDesc = {};
				samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
				samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
				samplerDesc.MipLODBias = 0.0f;
				samplerDesc.MinLOD = 0.0f;
				samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateSamplerState(&samplerDesc, SamplerStates["u_SSAnisotropicWrap"].GetAddressOf()));
			}
			{
				D3D11_SAMPLER_DESC samplerDesc = {};
				samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
				samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
				samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
				samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateSamplerState(&samplerDesc, SamplerStates["u_SSPointClamp"].GetAddressOf()));
			}		
			{
				D3D11_SAMPLER_DESC samplerDesc = {};
				samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateSamplerState(&samplerDesc, SamplerStates["u_SSLinearWrap"].GetAddressOf()));
			}
			{
				D3D11_SAMPLER_DESC samplerDesc = {};
				samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
				samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
				samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateSamplerState(&samplerDesc, SamplerStates["u_SSLinearClamp"].GetAddressOf()));
			}
		}
	};
}
#endif