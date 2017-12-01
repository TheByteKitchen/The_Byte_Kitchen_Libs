//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) The Byte Kitchen. All rights reserved
//// Some parts: Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include "GraphicsDeviceManager.h"
#include "IComponent.h"
#include "SceneTime.h"
#include "ConstantBufferFormats.h"

namespace TheByteKitchen
{
	namespace DX
	{
		class SceneComponent : public IComponent
		{
		public:
			SceneComponent();
			virtual ~SceneComponent();

			virtual void Initialize();
			virtual void LoadContent();
			virtual void Update(SceneTime time);
			virtual void Render(SceneTime time);

		protected:
			GraphicsDeviceManager*								m_graphicsDeviceManager;

			// Declare DirectWrite & Windows Imaging Component Objects
			Microsoft::WRL::ComPtr<IWICImagingFactory2>			m_wicFactory;

			// Direct3D Objects (Convenience)
			Microsoft::WRL::ComPtr<ID3D11Device2>				m_d3dDevice;
			Microsoft::WRL::ComPtr<ID3D11DeviceContext2>		m_d3dContext;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_renderTargetView;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		m_depthStencilView;
			Microsoft::WRL::ComPtr<ID3D11SamplerState>			m_sampler;
			Microsoft::WRL::ComPtr<ID3D11Buffer>				m_constantBuffer;
			D3D_FEATURE_LEVEL									m_featureLevel;

		};
	}
}