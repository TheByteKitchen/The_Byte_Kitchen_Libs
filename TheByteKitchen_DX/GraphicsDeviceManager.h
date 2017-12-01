//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) The Byte Kitchen. All rights reserved

#pragma once

#include "pch.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;

namespace TheByteKitchen
{
	namespace DX
	{
		///<summary>
		/// Singleton class that exposes essential objects through a static property
		///</summary>
		class GraphicsDeviceManager
		{
		public:
			static void CreateGraphicsDeviceManager(CoreWindow^ coreWindow, SwapChainBackgroundPanel^ swapChainPanel=nullptr);

			// Reference to the singleton GraphicsDeviceManager
			static GraphicsDeviceManager* Current()
			{
				if (instance)
					return instance;
				else 
					throw std::exception("GraphicsDeviceManager object not created, use the 'CreateGraphicsDeviceManager' factory method first");
			}

			std::vector <IDXGIAdapter*> EnumerateAdapters(void);

			// Direct3D Properties
			ComPtr<ID3D11Device2> D3DDevice()
			{
				return (m_d3dDevice) ? m_d3dDevice : CreateDirect3DObjects(), m_d3dDevice;
			}

			ComPtr<ID3D11DeviceContext2> D3DContext()
			{
				return (m_d3dContext.Get()) ? m_d3dContext : CreateDirect3DObjects(), m_d3dContext;
			}

			ComPtr<IDXGISwapChain1> SwapChain()
			{
				auto p = m_swapChain.Get();

				return (m_swapChain.Get()) ? m_swapChain : CreateDirect3DObjects(), m_swapChain;
			}

			D3D_FEATURE_LEVEL FeatureLevel()
			{
				return (m_featureLevel) ? m_featureLevel : CreateDirect3DObjects(), m_featureLevel;
			}

			ComPtr<ID3D11RenderTargetView> RenderTargetView()
			{
				return (m_renderTargetView.Get()) ? m_renderTargetView : CreateDirect3DObjects(), m_renderTargetView;
			}

			ComPtr<ID3D11DepthStencilView> DepthStencilView()
			{
				return (m_depthStencilView.Get()) ? m_depthStencilView : CreateDirect3DObjects(), m_depthStencilView;
			}

			// Direct2D objects
			ComPtr<ID2D1Factory1> D2D1Factory()
			{
				return (m_d2dFactory.Get()) ? m_d2dFactory : CreateDirect2DObjects(), m_d2dFactory;
			}

			ComPtr<ID2D1Device> D2D1Device()
			{
				return (m_d2dDevice.Get()) ? m_d2dDevice : CreateDirect2DObjects(), m_d2dDevice;
			}

			ComPtr<ID2D1DeviceContext> D2D1Context()
			{
				return (m_d2dContext.Get()) ? m_d2dContext : CreateDirect2DObjects(), m_d2dContext;
			}

			ComPtr<ID2D1Bitmap1> D2D1Bitmap()
			{
				return (m_d2dTargetBitmap.Get()) ? m_d2dTargetBitmap : CreateDirect2DObjects(), m_d2dTargetBitmap;
			}

			// DirectWrite objects
			ComPtr<IDWriteFactory1> DWriteFactory()
			{
				return (m_dwriteFactory.Get()) ? m_dwriteFactory : CreateDirectWObjects(), m_dwriteFactory;
			}

			// WIC objects
			ComPtr<IWICImagingFactory2> WicFactory()
			{
				return (m_wicFactory.Get()) ? m_wicFactory : CreateWICObjects(), m_wicFactory;
			}

			Windows::Foundation::Rect WindowBounds()
			{
				return (m_windowBounds.Width) ? m_windowBounds : CreateDirect3DObjects(), m_windowBounds;
			}

		private:
			static GraphicsDeviceManager*					instance;
			SwapChainBackgroundPanel^						m_swapChainPanel;
			Platform::Agile<Windows::UI::Core::CoreWindow>  m_window;
			Windows::Foundation::Rect                       m_windowBounds;
			Windows::Foundation::Size                       m_renderTargetSize;
			float                                           m_dpi;

			// Direct2D Objects
			Microsoft::WRL::ComPtr<ID2D1Factory2>           m_d2dFactory;
			Microsoft::WRL::ComPtr<ID2D1Device1>             m_d2dDevice;
			Microsoft::WRL::ComPtr<ID2D1DeviceContext1>      m_d2dContext;
			Microsoft::WRL::ComPtr<ID2D1Bitmap1>            m_d2dTargetBitmap;

			// DirectWrite Object
			Microsoft::WRL::ComPtr<IDWriteFactory2>         m_dwriteFactory;

			// Windows Imaging Component Object
			Microsoft::WRL::ComPtr<IWICImagingFactory2>     m_wicFactory;

			// Direct3D Objects
			Microsoft::WRL::ComPtr<ID3D11Device2>           m_d3dDevice;
			Microsoft::WRL::ComPtr<ID3D11DeviceContext2>    m_d3dContext;
			Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;
			D3D_FEATURE_LEVEL                               m_featureLevel;
			D3D11_VIEWPORT									m_screenViewport;
			unsigned int                                    m_numBuffers;

			void SetDpi( float dpi );
			void UpdateForWindowSizeChange( void );
			void CreateDirect3DObjects( void );
			void CreateDirect2DObjects( void );
			void CreateDirectWObjects( void );
			void CreateWICObjects( void );

			GraphicsDeviceManager( CoreWindow^ coreWindow );
			GraphicsDeviceManager( CoreWindow^ coreWindow, SwapChainBackgroundPanel^ swapChainPanel );
			~GraphicsDeviceManager( void );
		};
	}
}