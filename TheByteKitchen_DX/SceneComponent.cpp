//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) The Byte Kitchen. All rights reserved
//// Some parts: Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "SceneComponent.h"

using namespace TheByteKitchen::DX;

SceneComponent::SceneComponent() { }

SceneComponent::~SceneComponent() { }

void SceneComponent::Initialize()
{
	m_graphicsDeviceManager = GraphicsDeviceManager::Current();

	m_d3dDevice = m_graphicsDeviceManager->D3DDevice();
	m_d3dContext = m_graphicsDeviceManager->D3DContext();
	m_featureLevel = m_graphicsDeviceManager->FeatureLevel();
	m_renderTargetView = m_graphicsDeviceManager->RenderTargetView();
	m_depthStencilView = m_graphicsDeviceManager->DepthStencilView();

	m_wicFactory = m_graphicsDeviceManager->WicFactory();
}

void SceneComponent::LoadContent()
{
	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(MVPConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);

	ThrowIfFailed( 
		m_d3dDevice->CreateBuffer( &constantBufferDesc, nullptr, &m_constantBuffer )
		);

	CD3D11_SAMPLER_DESC samplerDesc(
		D3D11_FILTER_ANISOTROPIC,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		0.0f,
		m_featureLevel > D3D_FEATURE_LEVEL_9_1 ? 4 : 2,
		D3D11_COMPARISON_NEVER,
		nullptr,
		0,
		D3D11_FLOAT32_MAX
		);

	ThrowIfFailed(
		m_d3dDevice->CreateSamplerState( &samplerDesc, &m_sampler )
		);
} 

void SceneComponent::Update(SceneTime time) { }

void SceneComponent::Render(SceneTime sceneTime) { }
